#pragma once
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <string>
#include <string_view>
#include <vector>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

namespace seed_root {
inline bool start_ticks(std::string_view s){
    if(s.empty()||s.size()>20||s.front()=='0')return false;
    for(char c:s)if(c<'0'||c>'9')return false;
    return s.size()<20||s<="18446744073709551615";
}
inline bool boot_id(std::string_view s){
    if(s.size()!=36)return false;
    for(size_t i=0;i<s.size();i++){
        if(i==8||i==13||i==18||i==23){if(s[i]!='-')return false;}
        else if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return false;
    }
    return true;
}
inline bool completed_load_record(std::string_view text,std::string_view boot,int pid,
        std::string_view start,uid_t uid){
    if(text.size()>2048||!boot_id(boot)||pid<=1||!start_ticks(start)||uid<10000||uid>=100000)return false;
    auto end=text.find('\n');if(end==text.npos)return false;
    auto header=text.substr(0,end);std::vector<std::string_view> fields;size_t at=0;
    for(size_t i=0;i<=header.size();++i)if(i==header.size()||header[i]=='\t'){fields.push_back(header.substr(at,i-at));at=i+1;}
    if(fields.size()!=9||fields[0]!="v2"||fields[1]!="BEGIN"||fields[2]!=boot
       ||fields[3]!=std::to_string(pid)||fields[4]!=start||fields[5]!=std::to_string(uid)
       ||!start_ticks(fields[6])||fields[6]=="1"||fields[6].size()>10
       ||(fields[6].size()==10&&fields[6]>"2147483647")||!start_ticks(fields[7])
       ||(fields[8]!="arm64"&&fields[8]!="nativebridge"))return false;
    return text.substr(end+1)=="v2\t0\tPREPARED\nv2\t1\tATTACHING\nv2\t2\tATTACHED\nv2\t3\tCALLING\nv2\t4\tRETURNED\nv2\t5\tDETACHED\nv2\t6\tRETURNED_UNVERIFIED\n";
}
inline bool owned_file(int fd,uid_t owner){
    struct stat st{};
    return fd>=0&&!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_uid==owner
        &&st.st_nlink==1&&(st.st_mode&0077)==0;
}
// Lock stays open for the complete operation; never unlink/replace the inode.
// This must be acquired BEFORE any privileged staging or control-file mutation.
class LoadLease {
    int dir_=-1,lock_=-1;bool held_=false;
public:
    explicit LoadLease(const char* directory,uid_t owner=0){
        dir_=open(directory,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
        struct stat st{};
        if(dir_<0||fstat(dir_,&st)||st.st_uid!=owner||(st.st_mode&0022))return;
        lock_=openat(dir_,"load.lock",O_CREAT|O_RDWR|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK,0600);
        if(!owned_file(lock_,owner)||flock(lock_,LOCK_EX|LOCK_NB))return;
        // A pre-existing lock must name the same inode we actually acquired.
        struct stat named{},opened{};
        if(fstatat(dir_,"load.lock",&named,AT_SYMLINK_NOFOLLOW)||fstat(lock_,&opened)
           ||named.st_dev!=opened.st_dev||named.st_ino!=opened.st_ino)return;
        held_=fsync(dir_)==0;
    }
    ~LoadLease(){if(lock_>=0)close(lock_);if(dir_>=0)close(dir_);}
    LoadLease(const LoadLease&)=delete;LoadLease& operator=(const LoadLease&)=delete;
    bool held()const{return held_;}
    int directory()const{return held_?dir_:-1;}
};
enum class LoadPhase { Prepared, Attaching, Attached, Calling, Returned, Detached, ReturnedUnverified, Unknown };
inline const char* phase_name(LoadPhase p){
    switch(p){
        case LoadPhase::Prepared:return "PREPARED";
        case LoadPhase::Attaching:return "ATTACHING";
        case LoadPhase::Attached:return "ATTACHED";
        case LoadPhase::Calling:return "CALLING";
        case LoadPhase::Returned:return "RETURNED";
        case LoadPhase::Detached:return "DETACHED";
        case LoadPhase::ReturnedUnverified:return "RETURNED_UNVERIFIED";
        default:return "UNKNOWN";
    }
}
// Existing v1 "attempted" files, partial v2 files and UNKNOWN all block another
// attempt for the same boot/PID/start. Never clear, rewrite or infer success.
class LoadAttempt {
    int fd_=-1;bool durable_=false,finished_=false;unsigned seq_=0;
    LoadPhase phase_=LoadPhase::Prepared;
    bool append(std::string_view line){
        if(fd_<0)return false;
        size_t n=0;
        while(n<line.size()){
            auto wrote=write(fd_,line.data()+n,line.size()-n);
            if(wrote<0&&errno==EINTR)continue;
            if(wrote<=0)return false;
            n+=static_cast<size_t>(wrote);
        }
        return fsync(fd_)==0;
    }
public:
    LoadAttempt(const LoadLease& lease,std::string_view boot,int gamePid,std::string_view gameStart,
            uid_t gameUid,int ownerPid,std::string_view ownerStart,std::string_view backend){
        if(!lease.held()||!boot_id(boot)||gamePid<=1||ownerPid<=1||!start_ticks(gameStart)
           ||!start_ticks(ownerStart)||gameUid<10000||gameUid>=100000
           ||(backend!="arm64"&&backend!="nativebridge"))return;
        std::string name="attempt-"+std::string(boot)+"-"+std::to_string(gamePid)+"-"+std::string(gameStart);
        fd_=openat(lease.directory(),name.c_str(),O_CREAT|O_EXCL|O_APPEND|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
        if(fd_<0)return;
        auto header="v2\tBEGIN\t"+std::string(boot)+"\t"+std::to_string(gamePid)+"\t"+std::string(gameStart)
            +"\t"+std::to_string(gameUid)+"\t"+std::to_string(ownerPid)+"\t"+std::string(ownerStart)
            +"\t"+std::string(backend)+"\n";
        durable_=append(header)&&append("v2\t0\tPREPARED\n")&&fsync(lease.directory())==0;
    }
    ~LoadAttempt(){
        if(fd_>=0){if(!finished_)append("v2\tEND\tUNKNOWN\n");close(fd_);}
    }
    LoadAttempt(const LoadAttempt&)=delete;LoadAttempt& operator=(const LoadAttempt&)=delete;
    bool ready()const{return durable_&&!finished_;}
    bool record(LoadPhase next){
        if(!ready()||next==LoadPhase::Unknown||static_cast<int>(next)!=static_cast<int>(phase_)+1)return false;
        // Failed append latches closed; no later action can use this attempt.
        durable_=append("v2\t"+std::to_string(++seq_)+"\t"+phase_name(next)+"\n");
        if(!durable_)return false;
        phase_=next;finished_=next==LoadPhase::ReturnedUnverified;return true;
    }
};
}
