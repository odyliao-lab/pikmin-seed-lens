// Actual KittyTrace overlay + candidate Context. No target arguments: fork-only.
#include "../app/arm64_context.h"
#include "../app/supervised_load.h"
#include "Injector/KittyInjector.hpp"
#include <atomic>
#include <new>
#include <thread>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sstream>
using namespace seed_arm64;
using Clock=std::chrono::steady_clock;
static int admitted=-1;
bool seed_game_trace_admitted(int pid){return pid>1&&pid==admitted;}
struct Shared{std::atomic<unsigned> beat{0};std::atomic<bool> done{false},unexpected{false};};
struct Child{
    pid_t pid=-1;int fd=-1;bool reaped=false;
    bool reap(bool normal=false){
        int status=0;auto end=Clock::now()+std::chrono::milliseconds(750);
        while(Clock::now()<end){auto got=waitpid(pid,&status,__WALL|WNOHANG);
            if(got==pid&&(WIFEXITED(status)||WIFSIGNALED(status))){reaped=true;return !normal||(WIFEXITED(status)&&!WEXITSTATUS(status));}
            if(got<0&&errno==ECHILD){reaped=true;return !normal;}
            if(got<0&&errno!=EINTR)return false;usleep(1000);
        }return false;
    }
    bool dispose(){if(pid<=0||reaped)return true;
        if(fd>=0)syscall(SYS_pidfd_send_signal,fd,SIGKILL,nullptr,0);else kill(pid,SIGKILL);return reap();}
    ~Child(){dispose();if(fd>=0)close(fd);}
};
__attribute__((naked,noinline)) static void gadget(){__asm__ volatile("svc #0\n.inst 0x00000000");}
__attribute__((naked,noinline)) static void spin(){__asm__ volatile("movi v0.16b,#0x3c\n1: yield\nb 1b");}
__attribute__((naked,noinline)) static void fault(){__asm__ volatile(".inst 0x00000000");}
__attribute__((noinline)) static uintptr_t changeFp(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,
    uintptr_t e,uintptr_t f,uintptr_t g,uintptr_t h){
    volatile unsigned char bytes[8192];for(size_t i=0;i<sizeof(bytes);++i)bytes[i]=static_cast<unsigned char>(i);
    __asm__ volatile("movi v0.16b,#0x5a\nmovi v31.16b,#0xa5\nmov x9,#0x400000\nmsr fpcr,x9\nmov x9,#1\nmsr fpsr,x9"
        :::"v0","v31","x9","memory");
    return a+2*b+3*c+4*d+5*e+6*f+7*g+8*h+bytes[255];
}
static bool test(int mode){
    size_t page=sysconf(_SC_PAGESIZE),size=65536;
    auto* shared=static_cast<Shared*>(mmap(nullptr,sizeof(Shared),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(shared==MAP_FAILED)return false;new(shared)Shared;
    void* map=mmap(nullptr,size+2*page,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    struct Unmap{void* p;size_t n;Shared* s;~Unmap(){if(p!=MAP_FAILED)munmap(p,n);munmap(s,sizeof(Shared));}} unmap{map,size+2*page,shared};
    if(map==MAP_FAILED)return false;uintptr_t base=reinterpret_cast<uintptr_t>(map)+page;
    if(mprotect(reinterpret_cast<void*>(base),size,PROT_READ|PROT_WRITE))return false;
    auto parent=getpid();Child child;child.pid=fork();if(child.pid<0)return false;
    if(!child.pid){if(prctl(PR_SET_PDEATHSIG,SIGKILL)||getppid()!=parent)_exit(3);
        signal(SIGUSR1,SIG_IGN);alarm(5);while(!shared->done.load())shared->beat.fetch_add(1);_exit(0);}
    child.fd=int(syscall(SYS_pidfd_open,child.pid,0));if(child.fd<0)return false;
    auto until=Clock::now()+std::chrono::milliseconds(500);
    while(!shared->beat.load()&&Clock::now()<until)usleep(1000);
    if(!shared->beat.load())return false;
    KittyTraceMgr trace(child.pid,0,true,100,reinterpret_cast<uintptr_t>(&gadget));
    if(!trace.seize(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL)||!trace.stop())return false;
    admitted=child.pid;Snapshot before;if(!before.capture(trace))return false;
    Context context(trace);std::array<uintptr_t,6> sysargs{};std::array<uintptr_t,8> fnargs{1,2,3,4,5,6,7,8};
    if(context.syscall(-1,sysargs,100).state!=State::Refused||context.poisoned())return false;
    if(context.function(reinterpret_cast<uintptr_t>(&changeFp),fnargs,base,size,0).state!=State::Refused||context.poisoned())return false;
    std::thread signaler;
    if(mode==3||mode==7)signaler=std::thread([fd=child.fd](){usleep(20000);syscall(SYS_pidfd_send_signal,fd,SIGUSR1,nullptr,0);});
    auto begin=Clock::now();Result result;
    if(mode<=4)result=context.syscall(mode==0?SYS_getpid:mode==4?SYS_exit:SYS_ppoll,sysargs,100);
    else result=context.function(mode==5?reinterpret_cast<uintptr_t>(&changeFp):mode==8?reinterpret_cast<uintptr_t>(&fault):reinterpret_cast<uintptr_t>(&spin),fnargs,base,size,100);
    if(signaler.joinable())signaler.join();
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-begin).count();
    printf("ARM64_CONTEXT mode=%d state=%d restored=%d signal=%d elapsed_ms=%lld\n",mode,int(result.state),result.restored,result.signal,static_cast<long long>(elapsed));
    if(elapsed>1000)return false;
    bool normal=mode==0||mode==5;
    if(normal){
        if(result.state!=State::Returned||!result.restored||context.poisoned()||result.value!=(mode==0?child.pid:459)){
            printf("NORMAL_RESULT_MISMATCH value=%lld expected=%lld\n",static_cast<long long>(result.value),static_cast<long long>(mode==0?child.pid:459));return false;}
        // Exercise the real overlay wrappers, including mmap/mprotect/munmap,
        // and prove a restored syscall slot admits a subsequent function.
        auto pidCall=trace.callSyscall(SYS_getpid);
        if(pidCall.result.val!=child.pid){printf("WRAPPER_PID_MISMATCH value=%lld\n",static_cast<long long>(pidCall.result.val));return false;}
        auto called=trace.callFunction(reinterpret_cast<uintptr_t>(&changeFp),uintptr_t(1),uintptr_t(2),uintptr_t(3),uintptr_t(4),uintptr_t(5),uintptr_t(6),uintptr_t(7),uintptr_t(8));
        if(called.status!=KT_RP_CALL_SUCCESS||called.result.val!=459){printf("WRAPPER_CALL_MISMATCH status=%d value=%lld\n",int(called.status),static_cast<long long>(called.result.val));return false;}
        Snapshot after;if(!after.capture(trace)||!before.same(after)){puts("WRAPPER_SNAPSHOT_MISMATCH");return false;}
        auto count=shared->beat.load();if(!trace.detach())return false;
        until=Clock::now()+std::chrono::milliseconds(500);
        while(shared->beat.load()==count&&Clock::now()<until)usleep(1000);
        if(shared->beat.load()==count)return false;
        shared->done.store(true);return child.reap(true);
    }
    if(result.state!=State::Unknown||result.restored||!context.poisoned()||
       context.syscall(SYS_getpid,sysargs,100).state!=State::Refused)return false;
    // A signal interrupting ppoll can first surface the kernel's internal
    // restart result at syscall-exit, before signal-delivery stop. Both are
    // UNKNOWN and must not be replayed or counted as a returned syscall.
    if(mode==3&&result.signal!=SIGUSR1&&!(result.signal==(SIGTRAP|0x80)&&result.value<=-512&&result.value>=-516))return false;
    if(mode==7&&result.signal!=SIGUSR1)return false;
    if(mode==8&&result.signal!=SIGILL)return false;
    return child.dispose();
}
static std::string startTicks(int pid){
    std::ifstream file("/proc/"+std::to_string(pid)+"/stat");std::string text;
    std::getline(file,text);auto end=text.rfind(')');if(end==text.npos)return {};
    std::istringstream fields(text.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(fields>>value))return {};return value;
}
static bool supervised(int mode){
    auto* shared=static_cast<Shared*>(mmap(nullptr,sizeof(Shared),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(shared==MAP_FAILED)return false;new(shared)Shared;
    struct Unmap{Shared* p;~Unmap(){munmap(p,sizeof(Shared));}} unmap{shared};
    auto parent=getpid();Child child;child.pid=fork();if(child.pid<0)return false;
    if(!child.pid){if(prctl(PR_SET_PDEATHSIG,SIGKILL)||getppid()!=parent)_exit(3);
#ifdef SEED_ARM64_PRODUCT_BUDGET_FIXTURE
        alarm(20); // Outer fixture failsafe, beyond the unchanged 15s supervisor.
#else
        alarm(8);
#endif
        while(!shared->done.load())shared->beat.fetch_add(1);_exit(0);}
    child.fd=int(syscall(SYS_pidfd_open,child.pid,0));if(child.fd<0)return false;
    auto until=Clock::now()+std::chrono::milliseconds(500);
    while(!shared->beat.load()&&Clock::now()<until)usleep(1000);
    if(!shared->beat.load())return false;
    char directory[]="/data/local/tmp/seed-arm64-supervise-XXXXXX";
    if(!mkdtemp(directory))return false;
    seed_root::LoadLease lease(directory,getuid());if(!lease.held())return false;
    auto boot="12345678-1234-1234-1234-123456789abc";
    seed_root::LoadAttempt attempt(lease,boot,child.pid,startTicks(child.pid),10001,getpid(),startTicks(getpid()),"arm64");
    auto result=seed_root::supervised_load(child.fd,attempt,[&](int socket){
        KittyMemoryMgr manager;
        if(mode==4&&!manager.initialize(child.pid,EK_MEM_OP_SYSCALL,true))return 8;
#ifdef SEED_ARM64_PRODUCT_BUDGET_FIXTURE
        manager.trace=KittyTraceMgr(child.pid,0,true,2000,reinterpret_cast<uintptr_t>(&gadget));
#else
        manager.trace=KittyTraceMgr(child.pid,0,true,mode==4?2000:100,reinterpret_cast<uintptr_t>(&gadget));
#endif
        auto& trace=manager.trace;
        if(!trace.seize(PTRACE_O_TRACESYSGOOD)||!trace.stop())return 9;
        Snapshot before;if(!before.capture(trace))return 10;
        if(!seed_root::worker_phase(socket,'A')||!seed_root::worker_phase(socket,'C')||
           !trace.setOptions(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL))return 11;
        admitted=child.pid;
        if(mode==1)trace.callFunction(reinterpret_cast<uintptr_t>(&spin));
        else if(mode==2)trace.callSyscall(SYS_ppoll,uintptr_t(0),uintptr_t(0),uintptr_t(0),uintptr_t(0),uintptr_t(0));
        else if(mode==3)trace.callFunction(reinterpret_cast<uintptr_t>(&fault));
        else if(mode==4){
            char exe[4096]{};auto n=readlink("/proc/self/exe",exe,sizeof(exe)-1);
            if(n<=0||n==sizeof(exe)-1)return 15;
            std::string path=std::string(exe,size_t(n))+".fixture.so";
            struct stat file{};
            if(lstat(path.c_str(),&file)||!S_ISREG(file.st_mode)||file.st_uid!=0||
                (file.st_mode&077)!=0||file.st_size<4096||file.st_size>1048576)return 16;
            inject_elf_config_t cfg{};cfg.sdk=KittyUtils::Android::getSDK();cfg.timeout=2000;
            cfg.selinux_state=Utils::selinux_state();cfg.free=true;cfg.seize=true;
            bool called=false;
            cfg.afterEntryPoint=[&](inject_elf_info_t& loaded){
                auto entry=loaded.elf.findSymbol("SeedArm64Fixture");
                if(!entry)return;
                auto call=trace.callFunction(entry,uintptr_t(123));
                called=call.status==KT_RP_CALL_SUCCESS&&call.result.val==(uintptr_t(123)^uintptr_t(0x53454544));
            };
            KittyInjector injector;
            if(!injector.init(&manager,cfg))return 17;
            auto loaded=injector.inject(path);
            Snapshot after;
            if(!loaded.is_valid()||loaded.pJNI_OnLoad||loaded.pJvm||!called||
                !after.capture(trace)||!before.same(after))return 18;
            std::ifstream maps("/proc/"+std::to_string(child.pid)+"/maps");std::string line;
            while(std::getline(maps,line))if(line.find(path)!=line.npos)return 19;
            puts("ARM64_ACTUAL_INJECTOR_LOAD_CALL_UNLOAD_PASS_NO_JNI");
        }
        else{
            auto call=trace.callFunction(reinterpret_cast<uintptr_t>(&changeFp),uintptr_t(1),uintptr_t(2),uintptr_t(3),uintptr_t(4),uintptr_t(5),uintptr_t(6),uintptr_t(7),uintptr_t(8));
            Snapshot after;if(call.result.val!=459||!after.capture(trace)||!before.same(after))return 12;
        }
        if(mode!=0&&mode!=4){shared->unexpected.store(true);return 13;}
        if(!seed_root::worker_phase(socket,'R')||!trace.detach()||!seed_root::worker_phase(socket,'D'))return 14;
        return 0;
#ifdef SEED_ARM64_PRODUCT_BUDGET_FIXTURE
    }); // Production default remains 15000ms; no product timeout changes.
#else
    },3000);
#endif
    pollfd exited{child.fd,POLLIN,0};bool dead=poll(&exited,1,0)>0&&(exited.revents&POLLIN);
    bool good=(mode==0||mode==4)?result==seed_root::SupervisedResult::Returned&&!dead:
        result==seed_root::SupervisedResult::UnknownContained&&dead;
    printf("ARM64_OVERLAY_SUPERVISED mode=%d result=%d target_dead=%d\n",mode,int(result),dead);
    if(!good||shared->unexpected.load())return false;
    if(mode==0||mode==4){auto count=shared->beat.load();until=Clock::now()+std::chrono::milliseconds(500);
        while(shared->beat.load()==count&&Clock::now()<until)usleep(1000);
        if(shared->beat.load()==count)return false;shared->done.store(true);return child.reap(true);}
    return child.reap();
}
int main(int argc,char**){
    if(argc!=1)return 2;setbuf(stdout,nullptr);
#ifdef SEED_ARM64_PRODUCT_BUDGET_FIXTURE
    alarm(90); // Whole five-case fixture, not an individual loader budget.
#else
    alarm(30);
#endif
    if(!hardwareQualified()){puts("ARM64_CONTEXT_HARDWARE_UNQUALIFIED");return 77;}
#ifdef SEED_ARM64_PRODUCT_BUDGET_FIXTURE
    // A separate executable. It does not replace or relabel the 100ms stress
    // suite. Same fork-only targets, snapshot checks and unknown containment.
    (void)&test;
    for(int mode=0;mode<5;++mode)if(!supervised(mode)){puts("ARM64_PRODUCT_BUDGET_FAIL");return 1;}
    puts("ARM64_PRODUCT_BUDGET_PASS 5 own-child cases; NOT game acceptance");
#else
    // modes 1 and 2 intentionally repeat syscall timeout after fresh child state.
    for(int round=0;round<3;++round)for(int mode=0;mode<9;++mode)if(!test(mode)){
        puts("ARM64_CONTEXT_FAIL");return 1;}
    for(int mode=0;mode<5;++mode)if(!supervised(mode)){puts("ARM64_OVERLAY_SUPERVISION_FAIL");return 1;}
    puts("ARM64_CONTEXT_PASS 27 context + 5 supervised overlay/loader cases; NOT game acceptance");
#endif
}
