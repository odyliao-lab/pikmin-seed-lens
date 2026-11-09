#include "x64_backend.h"
#include "entry_context.h"
#include "identity_policy.h"
#include "supervised_load.h"
#include "bridge_runtime.h"
#include "Injector/KittyInjector.hpp"
#include <dirent.h>
#include <algorithm>
namespace {
int gamePid=0,traceTid=0;std::string gameStart,threadStart;bool armed=false;
std::string readFile(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
bool gameIdentity(int pid,const std::string& start){
    struct stat proc{},app{};
    return seed_root::game_command(readFile("/proc/"+std::to_string(pid)+"/cmdline"))&&seed_root::proc_start(pid)==start
        &&!stat(("/proc/"+std::to_string(pid)).c_str(),&proc)&&!stat("/data/user/0/com.nianticlabs.pikmin",&app)
        &&proc.st_uid==app.st_uid&&app.st_uid>=10000&&app.st_uid<100000;
}
std::vector<int> candidates(int pid){
    std::vector<int> result;auto path="/proc/"+std::to_string(pid)+"/task";DIR* dir=opendir(path.c_str());if(!dir)return result;
    while(auto* e=readdir(dir)){char* end=nullptr;long t=strtol(e->d_name,&end,10);if(!end||*end||t<=1||t>INT_MAX)continue;
        auto prefix=path+"/"+e->d_name;
        if(readFile(prefix+"/comm")=="UnityMain\n"&&readFile(prefix+"/wchan")=="0")result.push_back(int(t));
    }closedir(dir);std::sort(result.begin(),result.end());if(result.size()>8)result.clear();return result;
}
int run(int pid,const std::string& start,const std::string& host,int socket,int api){
    if(seed_bridge::runtime_backend()==seed_bridge::Backend::Unsupported||android_get_device_api_level()!=api||!gameIdentity(pid,start))return 10;
    KittyMemoryMgr mgr{};if(!mgr.initialize(pid,EK_MEM_OP_SYSCALL,true))return 11;
    bool acquired=false;
    for(int round=0;round<12&&!acquired;++round){
        if(!gameIdentity(pid,start))return 12;
        for(int tid:candidates(pid)){
            auto ts=seed_root::proc_start(tid);
            if(!seed_root::thread_owner(tid,pid,ts))continue;
            mgr.trace=KittyTraceMgr(tid,0,true,250);
            if(!mgr.trace.seize(PTRACE_O_TRACESYSGOOD))continue;
            if(!mgr.trace.stop())return 13; // worker exit detaches; no remote write yet
            siginfo_t info{};user_regs_struct regs{};
            bool own=mgr.trace.getSignalInfo(&info)&&info.si_signo==SIGTRAP&&info.si_code==((PTRACE_EVENT_STOP<<8)|SIGTRAP);
            if(!own){
                // No syscall/function has run; preserve a concurrent signal.
                int sig=info.si_signo;ptrace(PTRACE_DETACH,tid,nullptr,reinterpret_cast<void*>(static_cast<intptr_t>(sig)));
                return 14;
            }
            if(mgr.trace.getRegs(&regs)&&static_cast<int64_t>(regs.orig_rax)==-1&&seed_root::thread_owner(tid,pid,ts)){
                gamePid=pid;gameStart=start;traceTid=tid;threadStart=ts;acquired=true;break;
            }
            if(!mgr.trace.detach())return 15;
        }
        if(!acquired)usleep(19000+round*1700);
    }
    if(!acquired)return 16;
    if(!seed_root::worker_phase(socket,'A')||!gameIdentity(pid,start)||!seed_root::thread_owner(traceTid,pid,threadStart))return 17;
    // Explicit fail-stop is armed only after the parent durably records CALLING.
    if(!seed_root::worker_phase(socket,'C'))return 18;
    if(!mgr.trace.setOptions(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL))return 19;
    armed=true;mgr.trace.setRemoteCallTimeout(2000);
    inject_elf_config_t cfg{};cfg.sdk=api;cfg.timeout=2000;cfg.seize=true;
    cfg.package="com.nianticlabs.pikmin";cfg.selinux_state=Utils::selinux_state();
    KittyInjector injector;if(!injector.init(&mgr,cfg))return 20;
    auto result=injector.inject(host);
    if(!result.is_valid())return 21; // host constructor launches its own JNI worker
    if(!seed_root::worker_phase(socket,'R'))return 22;
    if(!mgr.trace.detach())return 23;
    armed=false;
    if(!seed_root::worker_phase(socket,'D'))return 24;
    return 0;
}
}
bool seed_game_trace_admitted(int tid){
    return armed&&tid==traceTid&&gameIdentity(gamePid,gameStart)&&seed_root::thread_owner(tid,gamePid,threadStart);
}
int seed_x64_load(int pid,const std::string& start,const std::string& host,seed_root::LoadAttempt& attempt){
    if(seed_bridge::runtime_backend()==seed_bridge::Backend::Unsupported||!gameIdentity(pid,start))return 40;
    const int api=android_get_device_api_level();
    int target=int(syscall(SYS_pidfd_open,pid,0));if(target<0||!gameIdentity(pid,start)){if(target>=0)close(target);return 41;}
    auto result=seed_root::supervised_load(target,attempt,[&](int socket){return run(pid,start,host,socket,api);});close(target);
    switch(result){
        case seed_root::SupervisedResult::Returned:puts("NATIVEBRIDGE_HOST_RETURNED_NOT_READY");return 0;
        case seed_root::SupervisedResult::UnknownContained:puts("UNKNOWN_PRESERVED_EXACT_GAME_STOPPED_REOPEN_REQUIRED");return 42;
        case seed_root::SupervisedResult::ContainmentUnconfirmed:puts("UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY");return 43;
        default:puts("NATIVEBRIDGE_REFUSED_LEFT_OFF_NO_RETRY");return 44;
    }
}
