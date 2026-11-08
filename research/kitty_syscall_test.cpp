// Uses the actual generated KittyTrace overlay, but accepts NO external target.
// Only our newly forked children are traced; no game, files or library loads.
#include "KittyTrace.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <new>
#include <thread>
#include <sys/syscall.h>
#if !defined(__x86_64__)
#error Isolated x86_64 test only
#endif
using Clock=std::chrono::steady_clock;
struct State{std::atomic<unsigned> count{0};std::atomic<bool> done{false};};
__attribute__((naked,noinline)) static void gadget(){__asm__ volatile("syscall\nint3\nret");}
static bool reap(pid_t p,int ms){
    auto end=Clock::now()+std::chrono::milliseconds(ms);int status=0;
    while(Clock::now()<end){auto got=waitpid(p,&status,WNOHANG|__WALL);if(got==p)return WIFEXITED(status)&&!WEXITSTATUS(status);if(got<0&&errno!=EINTR)return false;usleep(1000);}
    return false;
}
struct Child{pid_t pid=-1;int fd=-1;~Child(){if(pid>0&&fd>=0){syscall(SYS_pidfd_send_signal,fd,SIGKILL,nullptr,0);reap(pid,1000);}if(fd>=0)close(fd);}};
static bool test(int mode){
    auto* shared=static_cast<State*>(mmap(nullptr,sizeof(State),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(shared==MAP_FAILED)return false;
    new(shared)State;
    struct Unmap{State* s;~Unmap(){munmap(s,sizeof(State));}} unmap{shared};
    Child child;child.pid=fork();
    if(child.pid<0)return false;
    if(!child.pid){alarm(5);signal(SIGUSR1,SIG_IGN);while(!shared->done.load())shared->count.fetch_add(1);_exit(0);}
    child.fd=int(syscall(SYS_pidfd_open,child.pid,0));if(child.fd<0)return false;
    auto until=Clock::now()+std::chrono::milliseconds(500);
    while(!shared->count.load()&&Clock::now()<until)usleep(1000);
    if(!shared->count.load())return false;
    KittyTraceMgr trace(child.pid,0,true,100,reinterpret_cast<uintptr_t>(&gadget));
    if(!trace.seize(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL)||!trace.stop())return false;
    user_regs_struct before{},after{};
    if(!trace.getRegs(&before)||static_cast<long long>(before.orig_rax)!=-1)return false;
    int ignored=0;auto bad=Clock::now();
    if(trace.wait(&ignored,__WALL,0)!=-1||Clock::now()-bad>std::chrono::milliseconds(100))return false;
    // Signals must not reset the absolute deadline.
    std::thread signals;
    if(mode==2)signals=std::thread([fd=child.fd](){for(int i=0;i<70;i++){syscall(SYS_pidfd_send_signal,fd,SIGUSR1,nullptr,0);usleep(3000);}});
    if(mode==4){signal(SIGUSR1,+[](int){});auto tracer=pthread_self();
        signals=std::thread([tracer](){for(int i=0;i<70;i++){pthread_kill(tracer,SIGUSR1);usleep(3000);}});}
    auto start=Clock::now();
    auto result=trace.callSyscall(mode==0?SYS_getpid:(mode==3?SYS_exit:SYS_pause),uintptr_t(0));
    if(signals.joinable())signals.join();
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-start).count();
    printf("KITTY_RESULT mode=%d status=%d value=%lld elapsed_ms=%lld\n",mode,int(result.status),static_cast<long long>(result.result.val),static_cast<long long>(elapsed));
    if(mode==3){
        if(result.status!=KT_RP_CALL_EXITED||elapsed>1000)return false;
        child.pid=-1;puts("KITTY_TARGET_EXIT_PASS");return true;
    }
    bool expected=mode==0?(result.status==KT_RP_CALL_SUCCESS&&result.result.val==child.pid)
        :mode==2?(result.status==KT_RP_CALL_FAILED||result.status==KT_RP_CALL_TIMEOUT)
        :result.status==KT_RP_CALL_TIMEOUT;
    if(elapsed>1500||!expected)return false;
    if(!trace.getRegs(&after)||memcmp(&before,&after,sizeof(before)))return false;
    auto count=shared->count.load();if(!trace.detach())return false;
    until=Clock::now()+std::chrono::milliseconds(500);
    while(shared->count.load()==count&&Clock::now()<until)usleep(1000);
    if(shared->count.load()==count)return false;
    shared->done.store(true);if(!reap(child.pid,500))return false;child.pid=-1;
    printf("KITTY_SYSCALL_PASS mode=%d elapsed_ms=%lld gpr=equal resumed=yes\n",mode,static_cast<long long>(elapsed));return true;
}
int main(int argc,char**){
    if(argc!=1)return 2;setbuf(stdout,nullptr);alarm(15);
    for(int i=0;i<3;i++)for(int mode=0;mode<5;mode++)if(!test(mode)){puts("KITTY_SYSCALL_FAILED");return 1;}
    puts("KITTY_ISOLATED_PASS 15 cases; NOT arbitrary dlopen/FP/stack/TLS recovery");
}
