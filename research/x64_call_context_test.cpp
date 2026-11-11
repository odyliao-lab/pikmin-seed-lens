// NO target arguments, game calls or external paths. Only disposable children.
#include "x64_call_context.h"
#include <atomic>
#include <cstdio>
#include <new>
#include <thread>
#include <sys/syscall.h>
using namespace seed_research;
using Clock=std::chrono::steady_clock;
struct State{std::atomic<unsigned> heartbeat{0};std::atomic<bool> done{false};};
__attribute__((noinline)) static uintptr_t changeFp(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t e,uintptr_t f){
    volatile unsigned char local[8192];for(unsigned i=0;i<sizeof(local);i++)local[i]=static_cast<unsigned char>(i);
    __asm__ volatile("pcmpeqd %%xmm0, %%xmm0\npxor %%xmm7, %%xmm7\nfninit":::"xmm0","xmm7","memory");
    return a+2*b+3*c+4*d+5*e+6*f+local[255];
}
__attribute__((naked,noinline)) static void spin(){__asm__ volatile("pcmpeqd %xmm0,%xmm0\n1: pause\njmp 1b");}
__attribute__((naked,noinline)) static void fault(){__asm__ volatile("ud2\nret");}
__attribute__((naked,noinline)) static void exitChild(){__asm__ volatile("mov $60,%rax\nxor %rdi,%rdi\nsyscall\nud2");}
static bool reap(pid_t p,int ms){int st=0;auto until=Clock::now()+std::chrono::milliseconds(ms);
    do{auto r=waitpid(p,&st,WNOHANG|__WALL);if(r==p)return WIFEXITED(st)&&!WEXITSTATUS(st);if(r<0&&errno!=EINTR)return false;usleep(1000);}while(Clock::now()<until);return false;}
struct Child{pid_t pid=-1;int fd=-1;~Child(){if(pid>0&&fd>=0){syscall(SYS_pidfd_send_signal,fd,SIGKILL,nullptr,0);reap(pid,500);}if(fd>=0)close(fd);}};
static bool test(int mode){
    size_t page=static_cast<size_t>(sysconf(_SC_PAGESIZE)),size=65536;
    void* mapping=mmap(nullptr,size+2*page,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(mapping==MAP_FAILED)return false;
    auto base=reinterpret_cast<uintptr_t>(mapping)+page;
    if(mprotect(reinterpret_cast<void*>(base),size,PROT_READ|PROT_WRITE))return false;
    auto* state=static_cast<State*>(mmap(nullptr,sizeof(State),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(state==MAP_FAILED)return false;new(state)State;
    struct Cleanup{void* p;size_t n;State* s;~Cleanup(){munmap(p,n);munmap(s,sizeof(State));}} cleanup{mapping,size+2*page,state};
    Child child;child.pid=fork();if(child.pid<0)return false;
    if(!child.pid){alarm(5);signal(SIGUSR1,SIG_IGN);while(!state->done.load())state->heartbeat.fetch_add(1);_exit(0);}
    child.fd=int(syscall(SYS_pidfd_open,child.pid,0));if(child.fd<0)return false;
    auto until=Clock::now()+std::chrono::milliseconds(500);
    while(!state->heartbeat.load()&&Clock::now()<until)usleep(1000);
    if(!state->heartbeat.load())return false;
    KittyTraceMgr trace(child.pid,0,true,100);
    if(!trace.seize(PTRACE_O_EXITKILL)||!trace.stop())return false;
    X64CallContext context(trace);std::array<uintptr_t,6> args{1,2,3,4,5,6};
    if(context.call(reinterpret_cast<uintptr_t>(&changeFp),args,base,size,0).state!=CallState::Refused || context.poisoned())return false;
    std::thread signaler;
    if(mode==3)signaler=std::thread([fd=child.fd](){usleep(20000);syscall(SYS_pidfd_send_signal,fd,SIGUSR1,nullptr,0);});
    auto start=Clock::now();
    uintptr_t function=mode==0?reinterpret_cast<uintptr_t>(&changeFp):mode==2?reinterpret_cast<uintptr_t>(&fault):mode==4?reinterpret_cast<uintptr_t>(&exitChild):reinterpret_cast<uintptr_t>(&spin);
    auto result=context.call(function,args,base,size,100);
    if(signaler.joinable())signaler.join();
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-start).count();
    printf("X64_CONTEXT mode=%d state=%d restored=%d signal=%d value=%llu elapsed_ms=%lld\n",mode,int(result.state),result.cpuRestored,result.signal,static_cast<unsigned long long>(result.value),static_cast<long long>(elapsed));
    if(elapsed>1000)return false;
    if(mode==4){if(result.state!=CallState::Unknown || result.cpuRestored || !context.poisoned())return false;child.pid=-1;return true;}
    if(!result.cpuRestored)return false;
    if(mode==0){
        if(result.state!=CallState::Returned || result.value!=346 || context.poisoned())return false;
        auto second=context.call(function,args,base,size,100);
        if(second.state!=CallState::Returned || !second.cpuRestored || second.value!=346)return false;
    }else{
        if(result.state!=CallState::Unknown || !context.poisoned())return false;
        if(mode==2&&result.signal!=SIGILL)return false;
        if(mode==3&&result.signal!=SIGUSR1)return false;
        if(context.call(reinterpret_cast<uintptr_t>(&changeFp),args,base,size,100).state!=CallState::Refused)return false;
        // Dispose rather than asserting arbitrary side effects are reversible.
        return true;
    }
    auto count=state->heartbeat.load();if(!trace.detach())return false;
    until=Clock::now()+std::chrono::milliseconds(500);
    while(state->heartbeat.load()==count&&Clock::now()<until)usleep(1000);
    if(state->heartbeat.load()==count)return false;
    state->done.store(true);if(!reap(child.pid,500))return false;child.pid=-1;
    puts("X64_CONTEXT_RETURN_PASS gpr/xstate/redzone=equal resumed=yes");return true;
}
int main(int argc,char**){if(argc!=1)return 2;setbuf(stdout,nullptr);alarm(20);
    for(int round=0;round<3;round++)for(int mode=0;mode<5;mode++)if(!test(mode)){puts("X64_CONTEXT_FAILED");return 1;}
    puts("X64_CONTEXT_PASS 15 isolated cases; NOT TLS/heap/dlopen rollback or game acceptance");}
