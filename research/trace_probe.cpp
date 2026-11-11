// Research only. Accepts NO external PID, path, syscall or payload arguments.
// Traces only our own newly forked child; never opens a game or loads a library.
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <new>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error This isolated probe is for the unqualified x86_64 backend only.
#endif
using Clock=std::chrono::steady_clock;
struct Shared { std::atomic<unsigned> heartbeat; std::atomic<bool> finish; };
static_assert(std::atomic<unsigned>::is_always_lock_free);
__attribute__((naked,noinline)) static void trap() {
    __asm__ volatile("syscall\nint3\nret");
}
static int event(pid_t child,int &status,int milliseconds) {
    const auto deadline=Clock::now()+std::chrono::milliseconds(milliseconds);
    do {
        const auto got=waitpid(child,&status,__WALL|WNOHANG);
        if(got==child)return 1;
        if(got<0&&errno!=EINTR)return -1;
        usleep(1000);
    } while(Clock::now()<deadline);
    return 0;
}
struct Child {
    pid_t pid=-1;
    ~Child(){
        // Only a child created in this object, never a caller-supplied PID.
        if(pid>0){kill(pid,SIGKILL);int status=0;event(pid,status,1000);}
    }
};
static bool probe(bool timeout_case) {
    auto shared=static_cast<Shared*>(mmap(nullptr,sizeof(Shared),PROT_READ|PROT_WRITE,
                                         MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(shared==MAP_FAILED)return false;
    new(shared) Shared{};
    struct Unmap{Shared *s;~Unmap(){munmap(s,sizeof(Shared));}} unmap{shared};
    Child child;
    child.pid=fork();
    if(child.pid==0){
        alarm(8);
        while(!shared->finish.load(std::memory_order_relaxed))
            shared->heartbeat.fetch_add(1,std::memory_order_relaxed);
        _exit(0);
    }
    if(child.pid<0)return false;
    const auto started=Clock::now();
    while(!shared->heartbeat.load()&&Clock::now()-started<std::chrono::seconds(1))usleep(1000);
    if(!shared->heartbeat.load())return false;
    if(ptrace(PTRACE_SEIZE,child.pid,nullptr,nullptr)<0)return false;
    if(ptrace(PTRACE_INTERRUPT,child.pid,nullptr,nullptr)<0)return false;
    int status=0;
    if(event(child.pid,status,500)!=1||!WIFSTOPPED(status))return false;
    user_regs_struct saved{},changed{},observed{};
    if(ptrace(PTRACE_GETREGS,child.pid,nullptr,&saved)<0)return false;
    // Child spins in userspace; no application syscall is being interrupted.
    if(static_cast<long long>(saved.orig_rax)!=-1)return false;
    changed=saved;
    changed.rip=reinterpret_cast<unsigned long long>(&trap);
    changed.orig_rax=static_cast<unsigned long long>(-1);
    changed.rax=timeout_case?SYS_pause:SYS_getpid;
    const auto call_start=Clock::now();
    if(ptrace(PTRACE_SETREGS,child.pid,nullptr,&changed)<0)return false;
    if(ptrace(PTRACE_CONT,child.pid,nullptr,nullptr)<0)return false;
    const auto result=event(child.pid,status,100);
    if(timeout_case){
        if(result!=0)return false;
        // A timeout is NOT permission to SETREGS while the tracee is still running.
        if(ptrace(PTRACE_INTERRUPT,child.pid,nullptr,nullptr)<0)return false;
        if(event(child.pid,status,500)!=1||!WIFSTOPPED(status))return false;
    }else{
        if(result!=1||!WIFSTOPPED(status)||WSTOPSIG(status)!=SIGTRAP)return false;
        if(ptrace(PTRACE_GETREGS,child.pid,nullptr,&observed)<0||observed.rax!=static_cast<unsigned long>(child.pid))return false;
    }
    if(ptrace(PTRACE_SETREGS,child.pid,nullptr,&saved)<0)return false;
    if(ptrace(PTRACE_GETREGS,child.pid,nullptr,&observed)<0||memcmp(&saved,&observed,sizeof(saved)))return false;
    const auto before=shared->heartbeat.load();
    if(ptrace(PTRACE_DETACH,child.pid,nullptr,nullptr)<0)return false;
    const auto resumed=Clock::now();
    while(shared->heartbeat.load()==before&&Clock::now()-resumed<std::chrono::milliseconds(500))usleep(1000);
    if(shared->heartbeat.load()==before)return false;
    shared->finish.store(true);
    if(event(child.pid,status,500)!=1||!WIFEXITED(status)||WEXITSTATUS(status)!=0)return false;
    child.pid=-1;
    const auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-call_start).count();
    if(elapsed>1500)return false;
    printf("%s PASS elapsed_ms=%lld registers=equal resumed=yes reaped=yes\n",
           timeout_case?"deadline_interrupt_restore":"normal_call_restore",static_cast<long long>(elapsed));
    return true;
}
int main(int argc,char**) {
    if(argc!=1){puts("external arguments refused");return 2;}
    for(int i=0;i<5;++i)if(!probe(false)||!probe(true)){puts("ISOLATED_PROBE_FAILED");return 1;}
    puts("ISOLATED_PROBE_PASS 10 cases; NOT production-loader or NativeBridge acceptance");
}
