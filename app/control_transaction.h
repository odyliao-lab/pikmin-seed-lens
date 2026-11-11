#pragma once
#include "load_journal.h"
#include <sys/random.h>
#include <sstream>
#include <vector>
#include <charconv>
#include <chrono>
#include "game_profiles.h"
#include "display_contract.h"

namespace seed_root {
inline bool write_all_sync(int fd,std::string_view bytes){
    size_t at=0;while(at<bytes.size()){
        auto n=write(fd,bytes.data()+at,bytes.size()-at);
        if(n<0&&errno==EINTR)continue;if(n<=0)return false;at+=static_cast<size_t>(n);
    }return fsync(fd)==0;
}
inline std::string control_token(){
    unsigned char bytes[16];if(getrandom(bytes,sizeof(bytes),0)!=sizeof(bytes))return {};
    constexpr char hex[]="0123456789abcdef";std::string out;
    for(auto b:bytes){out+=hex[b>>4];out+=hex[b&15];}return out;
}
// Cooperative native operations share LoadLease for the entire ON/load/OFF
// transaction. Unique temp+receipt; never truncate a shared control.new or lock.
// Receipts say COMMITTED, not "display restored" or "hooks ready".
inline bool commit_control(const LoadLease& lease,int parent,const char* leaf,bool on,
        int gamePid,std::string_view gameStart,uid_t owner=0){
    if(!lease.held()||parent<0||!leaf||!*leaf||std::string_view(leaf).find('/')!=std::string_view::npos
       ||gamePid<=1||!start_ticks(gameStart))return false;
    struct stat directory{},old{};
    if(fstat(parent,&directory)||!S_ISDIR(directory.st_mode))return false;
    bool existed=fstatat(parent,leaf,&old,AT_SYMLINK_NOFOLLOW)==0;
    if(!existed&&errno!=ENOENT)return false;
    if(existed&&(!S_ISREG(old.st_mode)||old.st_uid!=owner||old.st_nlink!=1||(old.st_mode&0022)||old.st_size>7))return false;
    auto token=control_token();if(token.empty())return false;
    auto name="control-"+token,temporary=".seed-control-"+token;
    int audit=openat(lease.directory(),name.c_str(),O_CREAT|O_EXCL|O_APPEND|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
    if(audit<0)return false;
    struct Close{int fd;~Close(){if(fd>=0)close(fd);}} auditOwner{audit};
    auto begin="v1\tINTENT\t"+std::to_string(gamePid)+"\t"+std::string(gameStart)+"\t"+(on?"on":"off")+"\n";
    if(!write_all_sync(audit,begin)||fsync(lease.directory()))return false;
    int temp=openat(parent,temporary.c_str(),O_CREAT|O_EXCL|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
    if(temp<0)return false;Close tempOwner{temp};
    if(!owned_file(temp,owner)||!write_all_sync(temp,on?"on\n":"off\n")||fchmod(temp,0644)||fsync(temp))return false;
    struct stat current{};
    bool still=fstatat(parent,leaf,&current,AT_SYMLINK_NOFOLLOW)==0;
    if((!still&&errno!=ENOENT)||still!=existed||(still&&(current.st_dev!=old.st_dev||current.st_ino!=old.st_ino)))return false;
    if(renameat(parent,temporary.c_str(),parent,leaf)||fsync(parent))return false;
    return write_all_sync(audit,"v1\tCOMMITTED_NOT_EFFECT_VERIFIED\n");
}
inline bool decimal(std::string_view s,uint64_t& out){
    if(s.empty()||s.size()>20)return false;
    auto result=std::from_chars(s.data(),s.data()+s.size(),out);
    return result.ec==std::errc{}&&result.ptr==s.data()+s.size();
}
// v1 retains its existing field order and appends /proc/self start ticks. Old
// APK awk readers tolerate the extension; new activation requires that proof.
// No freshness inferred from a filename, a loaded mapping or a prior process.
inline bool fresh_renderer_status(std::string_view text,int pid,std::string_view start,int64_t now,std::string_view expectedVersion="156.1"){
    bool known=false;for(const auto& profile:seed_preview::game_profiles)known=known||expectedVersion==profile.version;
    if(!known)return false;
    if(pid<=1||!start_ticks(start)||text.empty()||text.size()>512||text.back()!='\n'||text.find('\n')!=text.size()-1)return false;
    std::vector<std::string_view> fields;size_t at=0;
    for(size_t i=0;i<text.size();++i)if(text[i]=='\t'||text[i]=='\n'){fields.push_back(text.substr(at,i-at));at=i+1;}
    if(fields.size()!=11||fields[0]!="v1"||fields[3]!=expectedVersion||fields[10]!=start
       ||(fields[4]!="ready"&&fields[4]!="waiting-cards"))return false;
    uint64_t stamp=0,p=0;
    if(!decimal(fields[1],stamp)||!decimal(fields[2],p)||p!=static_cast<uint64_t>(pid)||now<0
       ||stamp>static_cast<uint64_t>(now)+2000||stamp+5000<static_cast<uint64_t>(now))return false;
    for(size_t i=5;i<=9;++i){uint64_t value=0;if(!decimal(fields[i],value)||(i==5&&value>1))return false;}
    return true;
}
// Game157's four-group status must come from this exact APK build. A previous
// standalone renderer can share filenames/PID but cannot authorize new features.
inline bool fresh_display_status(std::string_view text,int pid,std::string_view start,int64_t now,std::string_view version){
    if(version!="157.0")return fresh_renderer_status(text,pid,start,now,version);
    const auto suffix=std::string("\t")+seed_lens::display_build+"\n";
    if(!text.ends_with(suffix))return false;
    auto canonical=std::string(text.substr(0,text.size()-suffix.size()))+"\n";
    return fresh_renderer_status(canonical,pid,start,now,version);
}
}
