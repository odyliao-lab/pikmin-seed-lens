#include "arm64_backend.h"
#include "arm64_context.h"
#include "identity_policy.h"
#include "supervised_load.h"
#include "Injector/KittyInjector.hpp"
#include <dirent.h>
#include <sstream>
#include <algorithm>
namespace {
#ifdef SEED_ARM64_RESEARCH_PROBE
// Separate test binary only. No runtime target switch; cannot select the game.
constexpr const char* packageName="dev.ody.seedlensarmprobe";
constexpr const char* threadName="SeedLensProbe\n";
bool targetCommand(std::string_view value){
    std::string_view name(packageName);
    return value.size()>name.size()&&value.starts_with(name)&&
        value.find_first_not_of('\0',name.size())==value.npos;
}
#else
constexpr const char* packageName="com.nianticlabs.pikmin";
constexpr const char* threadName="UnityMain\n";
bool targetCommand(std::string_view value){return seed_root::game_command(value);}
#endif
int gamePid=0,traceTid=0;std::string gameStart,threadStart;bool armed=false;
std::string readFile(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
std::string procStart(int tid){
    auto text=readFile("/proc/"+std::to_string(tid)+"/stat");auto end=text.rfind(')');if(end==text.npos)return {};
    std::istringstream fields(text.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(fields>>value))return {};return value;
}
bool threadOwner(int tid,int pid,const std::string& start){
    if(tid<=1||pid<=1||!seed_root::start_ticks(start)||procStart(tid)!=start)return false;
    std::istringstream status(readFile("/proc/"+std::to_string(tid)+"/status"));std::string line;
    while(std::getline(status,line))if(line.starts_with("Tgid:")){
        std::istringstream in(line.substr(5));int actual=0;return (in>>actual)&&actual==pid;
    }return false;
}
bool identity(int pid,const std::string& start){
    struct stat proc{},app{};
    return targetCommand(readFile("/proc/"+std::to_string(pid)+"/cmdline"))&&procStart(pid)==start&&
        !stat(("/proc/"+std::to_string(pid)).c_str(),&proc)&&!stat((std::string("/data/user/0/")+packageName).c_str(),&app)&&
        proc.st_uid==app.st_uid&&app.st_uid>=10000&&app.st_uid<100000;
}
std::vector<int> candidates(int pid){
    auto path="/proc/"+std::to_string(pid)+"/task";DIR* dir=opendir(path.c_str());std::vector<int> result;
    if(!dir)return result;
    while(auto* entry=readdir(dir)){char* end=nullptr;auto tid=strtol(entry->d_name,&end,10);
        if(!end||*end||tid<=1||tid>INT_MAX)continue;
        auto prefix=path+"/"+entry->d_name;
        if(readFile(prefix+"/comm")==threadName&&readFile(prefix+"/wchan")=="0")result.push_back(int(tid));
    }closedir(dir);std::sort(result.begin(),result.end());if(result.size()>8)result.clear();return result;
}
int run(int pid,const std::string& start,const std::string& payload,int api,int socket){
    if(!identity(pid,start)||!seed_arm64::hardwareQualified())return 10;
    KittyMemoryMgr mgr{};if(!mgr.initialize(pid,EK_MEM_OP_SYSCALL,true))return 11;
    bool acquired=false;seed_arm64::Snapshot baseline;
    for(int round=0;round<12&&!acquired;++round){
        if(!identity(pid,start))return 12;
        for(int tid:candidates(pid)){
            auto ts=procStart(tid);if(!threadOwner(tid,pid,ts))continue;
            mgr.trace=KittyTraceMgr(tid,0,true,250);
            if(!mgr.trace.seize(PTRACE_O_TRACESYSGOOD))continue;
            if(!mgr.trace.stop())return 13;
            siginfo_t info{};
            if(!mgr.trace.getSignalInfo(&info))return 14;
            if(info.si_signo!=SIGTRAP||info.si_code!=((PTRACE_EVENT_STOP<<8)|SIGTRAP)){
                ptrace(PTRACE_DETACH,tid,nullptr,reinterpret_cast<void*>(static_cast<intptr_t>(info.si_signo)));return 15;
            }
            if(baseline.capture(mgr.trace)&&threadOwner(tid,pid,ts)){
                gamePid=pid;gameStart=start;traceTid=tid;threadStart=ts;acquired=true;break;
            }
            if(!mgr.trace.detach())return 16;
        }
        if(!acquired)usleep(19000+round*1700);
    }
    if(!acquired)return 17;
    if(!seed_root::worker_phase(socket,'A')||!identity(pid,start)||!threadOwner(traceTid,pid,threadStart))return 18;
    if(!seed_root::worker_phase(socket,'C'))return 19;
    if(!mgr.trace.setOptions(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL))return 20;
    armed=true;mgr.trace.setRemoteCallTimeout(2000);
    inject_elf_config_t cfg{};cfg.sdk=api;cfg.timeout=2000;cfg.seize=true;
    cfg.package=packageName;cfg.selinux_state=Utils::selinux_state();
    KittyInjector injector;if(!injector.init(&mgr,cfg))return 21;
    auto loaded=injector.inject(payload);
    if(!loaded.is_valid()||!loaded.pJNI_OnLoad||!loaded.pJvm)return 22;
    seed_arm64::Snapshot restored;
    if(!identity(pid,start)||!threadOwner(traceTid,pid,threadStart)||
        !restored.capture(mgr.trace)||!baseline.same(restored))return 25;
    if(!seed_root::worker_phase(socket,'R')||!mgr.trace.detach())return 23;
    armed=false;
    return seed_root::worker_phase(socket,'D')?0:24;
}
}
bool seed_game_trace_admitted(int tid){
    return armed&&tid==traceTid&&identity(gamePid,gameStart)&&threadOwner(tid,gamePid,threadStart);
}
int seed_arm64_load(int pid,const std::string& start,const std::string& payload,int api,seed_root::LoadAttempt& attempt){
    if(!identity(pid,start)||api<28||api>35)return 40;
    if(!seed_arm64::hardwareQualified()){puts("ARM64_CPU_STATE_UNQUALIFIED_LEFT_OFF");return 45;}
    int fd=int(syscall(SYS_pidfd_open,pid,0));
    if(fd<0||!identity(pid,start)){if(fd>=0)close(fd);puts("ARM64_PIDFD_UNAVAILABLE_LEFT_OFF");return 41;}
    auto result=seed_root::supervised_load(fd,attempt,[&](int socket){return run(pid,start,payload,api,socket);});close(fd);
    switch(result){
        case seed_root::SupervisedResult::Returned:puts("ARM64_RETURNED_REQUIRES_STATUS");return 0;
        case seed_root::SupervisedResult::UnknownContained:puts("UNKNOWN_PRESERVED_EXACT_GAME_STOPPED_REOPEN_REQUIRED");return 42;
        case seed_root::SupervisedResult::ContainmentUnconfirmed:puts("UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY");return 43;
        default:puts("ARM64_REFUSED_LEFT_OFF_NO_RETRY");return 44;
    }
}
