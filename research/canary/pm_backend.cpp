// Research-only derivative of the 0.3.1 x64 backend. Self probe only.
#include "pm_backend.h"
#include "../api32_user_context.h"
#include "identity.h"
#include <android/api-level.h>
#include "../../app/entry_context.h"
#include "../../app/identity_policy.h"
#include "../../app/supervised_load.h"
#include "Injector/KittyInjector.hpp"
#include <dirent.h>
#include <algorithm>
namespace {
int probePid=0,traceTid=0;std::string probeStart,threadStart;bool armed=false;
std::string readFile(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
bool probeIdentity(int pid,const std::string& start){
    struct stat proc{},app{};
    return canary_command(readFile("/proc/"+std::to_string(pid)+"/cmdline"))&&seed_root::proc_start(pid)==start
        &&!stat(("/proc/"+std::to_string(pid)).c_str(),&proc)&&!stat("/data/user/0/dev.ody.seedlensprobe",&app)
        &&proc.st_uid==app.st_uid&&app.st_uid>=10000&&app.st_uid<100000;
}
std::vector<int> candidates(int pid){
    std::vector<int> result;auto path="/proc/"+std::to_string(pid)+"/task";DIR* dir=opendir(path.c_str());if(!dir)return result;
    while(auto* e=readdir(dir)){char* end=nullptr;long t=strtol(e->d_name,&end,10);if(!end||*end||t<=1||t>INT_MAX)continue;
        auto prefix=path+"/"+e->d_name;
        if(readFile(prefix+"/comm")=="SeedLensProbe\n"&&readFile(prefix+"/wchan")=="0")result.push_back(int(t));
    }closedir(dir);std::sort(result.begin(),result.end());if(result.size()>8)result.clear();return result;
}
int refused(int stage){printf("PROBE_PM_WORKER_REFUSED stage=%d errno=%d\n",stage,errno);return stage;}
int run(int pid,const std::string& start,const std::string& host,int socket,bool timeoutTest){
    if(!probeIdentity(pid,start))return refused(10);
    KittyMemoryMgr mgr{};if(!mgr.initialize(pid,EK_MEM_OP_SYSCALL,true))return refused(11);
    bool acquired=false;
    for(int round=0;round<12&&!acquired;++round){
        if(!probeIdentity(pid,start))return refused(12);
        auto available=candidates(pid);
        printf("PROBE_PM_CANDIDATES round=%d count=%zu\n",round,available.size());
        for(int tid:available){
            auto ts=seed_root::proc_start(tid);
            if(!seed_root::thread_owner(tid,pid,ts))continue;
            mgr.trace=KittyTraceMgr(tid,0,true,250);
            if(!mgr.trace.seize(PTRACE_O_TRACESYSGOOD)){printf("PROBE_PM_SEIZE_REFUSED errno=%d\n",errno);continue;}
            if(!mgr.trace.stop())return refused(13); // worker exit detaches; no remote write yet
            siginfo_t info{};user_regs_struct regs{};
            bool own=mgr.trace.getSignalInfo(&info)&&info.si_signo==SIGTRAP&&info.si_code==((PTRACE_EVENT_STOP<<8)|SIGTRAP);
            if(!own){
                // No syscall/function has run; preserve a concurrent signal.
                int sig=info.si_signo;ptrace(PTRACE_DETACH,tid,nullptr,reinterpret_cast<void*>(static_cast<intptr_t>(sig)));
                return refused(14);
            }
            bool haveRegs=mgr.trace.getRegs(&regs),sameOwner=seed_root::thread_owner(tid,pid,ts);
            printf("PROBE_PM_CONTEXT regs=%d orig_rax=%lld owner=%d\n",haveRegs,static_cast<long long>(regs.orig_rax),sameOwner);
            if(haveRegs&&seed_research::user_context_registers(regs,tid)&&sameOwner){
                probePid=pid;probeStart=start;traceTid=tid;threadStart=ts;acquired=true;break;
            }
            if(!mgr.trace.detach())return refused(15);
        }
        if(!acquired)usleep(19000+round*1700);
    }
    if(!acquired)return refused(16);
    if(!seed_root::worker_phase(socket,'A')||!probeIdentity(pid,start)||!seed_root::thread_owner(traceTid,pid,threadStart))return 17;
    // Explicit fail-stop is armed only after the parent durably records CALLING.
    if(!seed_root::worker_phase(socket,'C'))return 18;
    if(!mgr.trace.setOptions(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL))return 19;
    armed=true;mgr.trace.setRemoteCallTimeout(2000);
    if(timeoutTest){puts("PROBE_PM_TIMEOUT_ARMED_AFTER_DURABLE_CALLING");for(;;)pause();}
    inject_elf_config_t cfg{};cfg.sdk=SEED_PROBE_API;cfg.timeout=2000;cfg.seize=true;
    cfg.package="dev.ody.seedlensprobe";cfg.selinux_state=Utils::selinux_state();
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
    return armed&&tid==traceTid&&probeIdentity(probePid,probeStart)&&seed_root::thread_owner(tid,probePid,threadStart);
}
int seed_probe_pm_load(int pid,const std::string& start,const std::string& host,seed_root::LoadAttempt& attempt,bool timeoutTest){
    if(android_get_device_api_level()!=SEED_PROBE_API||!probeIdentity(pid,start))return 40;
    int target=int(syscall(SYS_pidfd_open,pid,0));if(target<0||!probeIdentity(pid,start)){if(target>=0)close(target);return 41;}
    auto result=seed_root::supervised_load(target,attempt,[&](int socket){return run(pid,start,host,socket,timeoutTest);},timeoutTest?3000:15000);close(target);
    switch(result){
        case seed_root::SupervisedResult::Returned:puts("PROBE_PM_HOST_RETURNED_NOT_READY");return 0;
        case seed_root::SupervisedResult::UnknownContained:puts("UNKNOWN_PRESERVED_EXACT_PROBE_CONTAINED");return 42;
        case seed_root::SupervisedResult::ContainmentUnconfirmed:puts("UNKNOWN_CONTAINMENT_UNCONFIRMED_DO_NOT_RETRY");return 43;
        default:puts("PROBE_PM_REFUSED_NO_RETRY");return 44;
    }
}
