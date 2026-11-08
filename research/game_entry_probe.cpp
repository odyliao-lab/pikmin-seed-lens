// Deliberately no library loading, remote calls, register writes or game reads.
// Requires a currently owned maintenance pause and drained local scan pending.
#include "../app/entry_context.h"
#include "../app/identity_policy.h"
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <climits>
#include <cstdio>
#include <ctime>
#include <vector>
static std::string read(const std::string& p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
static bool game(int pid,const std::string& start){
    struct stat proc{},app{};char abi[PROP_VALUE_MAX]{},sdk[PROP_VALUE_MAX]{},bridge[PROP_VALUE_MAX]{};
    __system_property_get("ro.product.cpu.abi",abi);__system_property_get("ro.build.version.sdk",sdk);__system_property_get("ro.dalvik.vm.native.bridge",bridge);
    return std::string(abi)=="x86_64"&&std::string(sdk)=="35"&&std::string(bridge)=="libnb.so"
      &&seed_root::game_command(read("/proc/"+std::to_string(pid)+"/cmdline"))&&seed_root::proc_start(pid)==start
      &&!stat(("/proc/"+std::to_string(pid)).c_str(),&proc)&&!stat("/data/user/0/com.nianticlabs.pikmin",&app)
      &&proc.st_uid==app.st_uid&&app.st_uid>=10000;
}
static bool paused(const std::string& owner){
    const std::string base="/data/adb/modules/pikmin_scanner_agent/";struct stat pending{},file{};
    if(lstat((base+"pause.until").c_str(),&file)||!S_ISREG(file.st_mode)||file.st_uid||file.st_mode&0077)return false;
    auto expected=owner+"\n";char* end=nullptr;long long until=strtoll(owner.c_str(),&end,10);
    return end&&!*end&&until>time(nullptr)+10&&until<time(nullptr)+1800
      &&read(base+"pause.until")==expected&&(stat((base+"scan.pending").c_str(),&pending)?errno==ENOENT:pending.st_size==0);
}
int main(int argc,char**argv){
    setbuf(stdout,nullptr);if(getuid()!=0||argc!=4)return 2;
    char* end=nullptr;long n=strtol(argv[1],&end,10);if(!end||*end||n<=1||n>INT_MAX)return 2;
    int pid=int(n);std::string start=argv[2],owner=argv[3];if(!game(pid,start)||!paused(owner))return 3;
    std::vector<int> tids;auto path="/proc/"+std::to_string(pid)+"/task";DIR* dir=opendir(path.c_str());if(!dir)return 4;
    while(auto* e=readdir(dir)){char* tail=nullptr;long t=strtol(e->d_name,&tail,10);if(!tail||*tail||t<=1||t>INT_MAX)continue;
        // Translated/runtime workers inherit UnityMain's comm. Do not assume
        // that name is unique; only sample currently non-sleeping candidates.
        // wchan is a hint, never the authorization to call: GETREGSET below is
        // the actual user-context evidence after a pinned, owned ptrace stop.
        if(read(path+"/"+e->d_name+"/comm")=="UnityMain\n"&&read(path+"/"+e->d_name+"/wchan")=="0")tids.push_back(int(t));}
    closedir(dir);if(tids.empty()||tids.size()>8)return 6;
    unsigned totalEligible=0;
    for(int tid:tids){auto threadStart=seed_root::proc_start(tid);
    unsigned eligible=0,kernel=0;
    for(int i=0;i<12;++i){
        if(!game(pid,start)||!paused(owner))return 7;
        seed_root::StoppedEntry entry;
        if(!entry.acquire(tid,pid,threadStart)){if(!entry.release())return 8;puts("ENTRY_REFUSED_NO_REMOTE_WRITE");return 9;}
        user_regs_struct regs{};bool user=entry.user_context(regs);
        if(!entry.release())return 8;
        if(user)++eligible;else ++kernel;
        usleep(19000+i*1700);
    }
    printf("ENTRY_OBSERVATION pid=%d tid=%d user=%u kernel_or_unreadable=%u; NO_CALL_NO_LOAD\n",pid,tid,eligible,kernel);
    totalEligible+=eligible;
    }
    return totalEligible?0:10;
}
