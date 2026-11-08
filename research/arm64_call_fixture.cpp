// Research executable: no target arguments, files or game symbols. Only its
// own fork children may be traced. NOT linked into the product loader.
// Returned CPU state is not a rollback of locks, TLS contents, heap or dlopen.
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include <thread>
#include <asm/hwcap.h>
#include <asm/ptrace.h>
#include <elf.h>
#include <signal.h>
#include <sys/auxv.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#if !defined(__aarch64__)
#error ARM64 isolated research fixture only
#endif
using Clock=std::chrono::steady_clock;
using Milliseconds=std::chrono::milliseconds;

static pid_t waitBounded(pid_t p,int& status,int ms) {
    const auto deadline=Clock::now()+Milliseconds(ms);
    do {
        auto got=waitpid(p,&status,WNOHANG|__WALL);
        if(got>0 || (got<0 && errno!=EINTR))return got;
        if(Clock::now()>=deadline)return 0;
        usleep(1000);
    } while(true);
}
template<class T> static bool get(pid_t p,int note,T& value) {
    value={};iovec io{&value,sizeof(value)};
    return ptrace(PTRACE_GETREGSET,p,note,&io)==0 && io.iov_len==sizeof(value);
}
template<class T> static bool set(pid_t p,int note,T& value) {
    iovec io{&value,sizeof(value)};
    return ptrace(PTRACE_SETREGSET,p,note,&io)==0 && io.iov_len==sizeof(value);
}
static bool absentRegset(pid_t p,int note) {
    std::array<unsigned char,32> bytes{};iovec io{bytes.data(),bytes.size()};
    errno=0;
    if(ptrace(PTRACE_GETREGSET,p,note,&io)==0)return false;
    return errno==EINVAL || errno==EIO;
}
struct Child {
    pid_t pid=-1;int fd=-1;bool reaped=false;
    bool collect(int ms,bool normal=false) {
        if(reaped)return true;
        int status=0;auto got=waitBounded(pid,status,ms);
        if(got!=pid || (!WIFEXITED(status)&&!WIFSIGNALED(status)))return false;
        reaped=true;return !normal || (WIFEXITED(status)&&WEXITSTATUS(status)==0);
    }
    bool dispose() {
        if(pid<=0||reaped)return true;
        // An unreaped, direct child cannot have its PID reused. pidfd is used
        // whenever available; fallback is only for failing pidfd_open itself.
        if(fd>=0)syscall(SYS_pidfd_send_signal,fd,SIGKILL,nullptr,0);
        else kill(pid,SIGKILL);
        return collect(500);
    }
    ~Child(){dispose();if(fd>=0)close(fd);}
};
struct Snapshot {
    user_pt_regs gpr{};user_fpsimd_state fp{};uint64_t tls=0;int syscall=-2;
    std::array<unsigned char,128> stack{};
    bool capture(pid_t p) {
        if(!get(p,NT_PRSTATUS,gpr)||!get(p,NT_PRFPREG,fp)||
           !get(p,NT_ARM_TLS,tls)||!get(p,NT_ARM_SYSTEM_CALL,syscall)||syscall!=-1 ||
           (gpr.pstate&(PSR_MODE_MASK|PSR_MODE32_BIT)) || (gpr.sp&15) || !gpr.sp)return false;
        iovec local{stack.data(),stack.size()},remote{reinterpret_cast<void*>(gpr.sp),stack.size()};
        return process_vm_readv(p,&local,1,&remote,1,0)==static_cast<ssize_t>(stack.size());
    }
    bool same(const Snapshot& other)const {
        return !memcmp(&gpr,&other.gpr,sizeof(gpr)) && !memcmp(&fp,&other.fp,sizeof(fp)) &&
            tls==other.tls && syscall==other.syscall && stack==other.stack;
    }
    bool restore(pid_t p) {
        // All writes are checked. No implicit resume on partial failure.
        if(!set(p,NT_PRFPREG,fp)||!set(p,NT_ARM_TLS,tls)||
           !set(p,NT_ARM_SYSTEM_CALL,syscall)||!set(p,NT_PRSTATUS,gpr))return false;
        Snapshot actual;return actual.capture(p)&&same(actual);
    }
};
enum class State{Refused,Returned,Unknown};
struct Result{State state=State::Refused;uint64_t value=0;bool restored=false,fpChanged=false;int signal=0;};
class FixtureCall {
    Child& child_;bool poisoned_=false;
public:
    explicit FixtureCall(Child& child):child_(child){}
    bool poisoned()const{return poisoned_;}
    Result call(uintptr_t function,const std::array<uint64_t,8>& args,uintptr_t base,size_t size,int timeout) {
        if(poisoned_||!function||(function&3)||!base||(base&15)||size<16384||size>1048576||
           base>UINTPTR_MAX-size||timeout<1||timeout>500)return {};
        // Minimal FPSIMD fixture, not SVE/SME/MTE/PAC support. Refuse rather than
        // silently claiming that saving V0..V31 preserves extended CPU state.
        auto hw=getauxval(AT_HWCAP),hw2=getauxval(AT_HWCAP2);
        if((hw&(HWCAP_SVE|HWCAP_PACA|HWCAP_PACG)) || (hw2&(HWCAP2_SME|HWCAP2_MTE)) ||
           !absentRegset(child_.pid,NT_ARM_SVE)||!absentRegset(child_.pid,NT_ARM_SSVE))return {};
        Snapshot original;if(!original.capture(child_.pid))return {};
        auto end=(base+size)&~uintptr_t(15);
        if(original.gpr.sp<=UINTPTR_MAX-original.stack.size() &&
           base<original.gpr.sp+original.stack.size() && end>original.gpr.sp)return {};
        auto regs=original.gpr;
        for(size_t i=0;i<args.size();++i)regs.regs[i]=args[i];
        regs.sp=end;regs.pc=function;regs.regs[30]=0;
        // Fixture functions have no branch protection; production must qualify
        // BTI/PAC and mapped executable entry independently.
        regs.pstate&=~static_cast<uint64_t>(PSR_BTYPE_MASK);
        poisoned_=true;Result out{State::Unknown};
        if(!set(child_.pid,NT_PRSTATUS,regs)||ptrace(PTRACE_CONT,child_.pid,nullptr,nullptr))return out;
        int status=0;auto got=waitBounded(child_.pid,status,timeout);
        if(got==child_.pid&&(WIFEXITED(status)||WIFSIGNALED(status))){child_.reaped=true;return out;}
        if(got==child_.pid&&WIFSTOPPED(status)) {
            out.signal=WSTOPSIG(status);siginfo_t info{};user_pt_regs returned{};user_fpsimd_state changed{};
            if(out.signal==SIGSEGV && (status>>16)==0 &&
               ptrace(PTRACE_GETSIGINFO,child_.pid,nullptr,&info)==0 && info.si_code==SEGV_MAPERR &&
               info.si_addr==nullptr && get(child_.pid,NT_PRSTATUS,returned)&&returned.pc==0&&returned.sp==end) {
                out.value=returned.regs[0];
                out.fpChanged=get(child_.pid,NT_PRFPREG,changed)&&memcmp(&changed,&original.fp,sizeof(changed));
                out.restored=original.restore(child_.pid);
                if(out.restored){out.state=State::Returned;poisoned_=false;}
            }
        }
        // Any other event remains UNKNOWN. Do not resume, retry, call remote
        // cleanup or claim CPU restoration undoes effects. Owner kills/reaps
        // only this disposable child. This is not a production containment path.
        return out;
    }
};
struct Shared{std::atomic<unsigned> heartbeat{0};std::atomic<bool> done{false};};
__attribute__((noinline)) static uint64_t changeFp(uint64_t a,uint64_t b,uint64_t c,uint64_t d,
                                                uint64_t e,uint64_t f,uint64_t g,uint64_t h) {
    volatile unsigned char local[8192];for(unsigned i=0;i<sizeof(local);++i)local[i]=static_cast<unsigned char>(i);
    __asm__ volatile("movi v0.16b,#0x5a\nmovi v31.16b,#0xa5\nmov x9,#0x400000\nmsr fpcr,x9\nmov x9,#1\nmsr fpsr,x9"
        :::"v0","v31","x9","memory");
    return a+2*b+3*c+4*d+5*e+6*f+7*g+8*h+local[255];
}
__attribute__((naked,noinline)) static void spin(){__asm__ volatile("movi v0.16b,#0x3c\n1: yield\nb 1b");}
__attribute__((naked,noinline)) static void fault(){__asm__ volatile(".inst 0x00000000\nret");}
__attribute__((naked,noinline)) static void exitChild(){__asm__ volatile("mov x0,#0\nmov x8,#93\nsvc #0\n.inst 0x00000000");}
static bool test(int mode) {
    const size_t page=static_cast<size_t>(sysconf(_SC_PAGESIZE)),size=65536;
    auto* state=static_cast<Shared*>(mmap(nullptr,sizeof(Shared),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(state==MAP_FAILED)return false;new(state)Shared;
    void* mapping=mmap(nullptr,size+2*page,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    struct Cleanup{void* p;size_t n;Shared* s;~Cleanup(){if(p!=MAP_FAILED)munmap(p,n);munmap(s,sizeof(Shared));}} cleanup{mapping,size+2*page,state};
    if(mapping==MAP_FAILED)return false;
    auto base=reinterpret_cast<uintptr_t>(mapping)+page;
    if(mprotect(reinterpret_cast<void*>(base),size,PROT_READ|PROT_WRITE))return false;
    const auto parent=getpid();Child child;child.pid=fork();if(child.pid<0)return false;
    if(!child.pid){
        if(prctl(PR_SET_PDEATHSIG,SIGKILL)||getppid()!=parent)_exit(3);
        alarm(5);signal(SIGUSR1,SIG_IGN);
        while(!state->done.load())state->heartbeat.fetch_add(1);
        _exit(0);
    }
    child.fd=int(syscall(SYS_pidfd_open,child.pid,0));if(child.fd<0)return false;
    auto until=Clock::now()+Milliseconds(500);
    while(!state->heartbeat.load()&&Clock::now()<until)usleep(1000);
    if(!state->heartbeat.load()||ptrace(PTRACE_SEIZE,child.pid,nullptr,PTRACE_O_EXITKILL)||
       ptrace(PTRACE_INTERRUPT,child.pid,nullptr,nullptr))return false;
    int status=0;
    if(waitBounded(child.pid,status,500)!=child.pid||!WIFSTOPPED(status)||
       (status>>16)!=PTRACE_EVENT_STOP)return false;
    FixtureCall context(child);std::array<uint64_t,8> args{1,2,3,4,5,6,7,8};
    Snapshot before;if(!before.capture(child.pid))return false;
    auto fn=reinterpret_cast<uintptr_t>(&changeFp);
    for(auto result:{context.call(fn,args,base,size,0),context.call(fn,args,base,size,501),
                    context.call(fn+1,args,base,size,100),context.call(fn,args,base,15,100),
                    context.call(fn,args,UINTPTR_MAX-15,size,100),
                    context.call(fn,args,before.gpr.sp,size,100)})
        if(result.state!=State::Refused||context.poisoned())return false;
    Snapshot after;if(!after.capture(child.pid)||!before.same(after))return false;
    std::thread signaler;
    if(mode==3)signaler=std::thread([fd=child.fd](){usleep(20000);syscall(SYS_pidfd_send_signal,fd,SIGUSR1,nullptr,0);});
    uintptr_t function=mode==0?fn:mode==2?reinterpret_cast<uintptr_t>(&fault):
        mode==4?reinterpret_cast<uintptr_t>(&exitChild):reinterpret_cast<uintptr_t>(&spin);
    auto began=Clock::now();auto result=context.call(function,args,base,size,100);
    if(signaler.joinable())signaler.join();
    auto elapsed=std::chrono::duration_cast<Milliseconds>(Clock::now()-began).count();
    printf("ARM64_FIXTURE mode=%d state=%d restored=%d fp_changed=%d signal=%d value=%llu elapsed_ms=%lld\n",
        mode,int(result.state),result.restored,result.fpChanged,result.signal,
        static_cast<unsigned long long>(result.value),static_cast<long long>(elapsed));
    if(elapsed>1000)return false;
    if(mode==0){
        if(result.state!=State::Returned||!result.restored||!result.fpChanged||result.value!=459||context.poisoned())return false;
        auto second=context.call(fn,args,base,size,100);
        if(second.state!=State::Returned||!second.restored||second.value!=459)return false;
        auto count=state->heartbeat.load();
        if(ptrace(PTRACE_DETACH,child.pid,nullptr,nullptr))return false;
        until=Clock::now()+Milliseconds(500);
        while(state->heartbeat.load()==count&&Clock::now()<until)usleep(1000);
        if(state->heartbeat.load()==count)return false;
        state->done.store(true);
        return child.collect(500,true);
    }
    if(result.state!=State::Unknown||result.restored||!context.poisoned()||
       context.call(fn,args,base,size,100).state!=State::Refused)return false;
    if(mode==2&&result.signal!=SIGILL)return false;
    if(mode==3&&result.signal!=SIGUSR1)return false;
    return child.dispose();
}
int main(int argc,char**) {
    if(argc!=1)return 2;
    setbuf(stdout,nullptr);alarm(25);
    auto hw=getauxval(AT_HWCAP),hw2=getauxval(AT_HWCAP2);
    if((hw&(HWCAP_SVE|HWCAP_PACA|HWCAP_PACG))||(hw2&(HWCAP2_SME|HWCAP2_MTE))){
        puts("ARM64_FIXTURE_UNQUALIFIED_EXTENDED_CPU_STATE; NOT RUN");return 77;
    }
    for(int round=0;round<3;++round)for(int mode=0;mode<5;++mode)
        if(!test(mode)){puts("ARM64_FIXTURE_FAILED; NO GAME TOUCHED");return 1;}
    puts("ARM64_FIXTURE_PASS 15 own-child cases, 90 refusal checks; NOT product/game/extended-state acceptance");
}
