// Isolated initramfs CPU research, never an Android/product entry point.
// No mounted Android partitions, network, credentials, game or ptrace operations.
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/reboot.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
#include <signal.h>
#include <ucontext.h>
struct FaultRecord {
    volatile unsigned signal, code;
    volatile uintptr_t address, pc, sp, x0, x1, x22;
};
static FaultRecord *fault_record;
static void recordFault(int number,siginfo_t *info,void *context) {
    auto *saved=static_cast<ucontext_t*>(context);
    fault_record->signal=static_cast<unsigned>(number);
    fault_record->code=static_cast<unsigned>(info->si_code);
    fault_record->address=reinterpret_cast<uintptr_t>(info->si_addr);
    fault_record->pc=saved->uc_mcontext.pc;
    fault_record->sp=saved->uc_mcontext.sp;
    fault_record->x0=saved->uc_mcontext.regs[0];
    fault_record->x1=saved->uc_mcontext.regs[1];
    fault_record->x22=saved->uc_mcontext.regs[22];
    // A signal return can invalidate/retranslate the faulty TCG block and
    // accidentally make a retry succeed. Re-raise with the now-default
    // disposition instead; the original fatal result remains observable.
    raise(number);
}
#endif

#if !defined(__aarch64__)
#error This fixture requires AArch64.
#endif

static int probe(int mode) {
    alarm(5);
    const rlimit no_core{0,0};
    if (setrlimit(RLIMIT_CORE,&no_core)) return 10;
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
    struct sigaction action{};
    action.sa_sigaction=recordFault;
    action.sa_flags=SA_SIGINFO|SA_RESETHAND;
    if (sigemptyset(&action.sa_mask)||sigaction(SIGSEGV,&action,nullptr)||
        sigaction(SIGBUS,&action,nullptr)||sigaction(SIGILL,&action,nullptr)) return 17;
    if (mode==7) {
        asm volatile("mov x0, #0\nldr x1, [x0]" ::: "x0","x1","memory");
        return 18;
    }
#endif
    const long page=sysconf(_SC_PAGESIZE);
    if (page<4096) return 11;
    auto *area=static_cast<unsigned char*>(mmap(nullptr,3*page,PROT_NONE,
        MAP_PRIVATE|MAP_ANONYMOUS,-1,0));
    if (area==MAP_FAILED) return 12;
    auto *plain=area+page;
    if (mprotect(plain,page,PROT_READ|PROT_WRITE)) return 13;
    plain+=0x720; // Match the observed allocation's within-page alignment.
    std::memset(plain,0xa5,64);
    uintptr_t address=reinterpret_cast<uintptr_t>(plain);
    if (mode>=2&&mode!=5) {
        if (prctl(PR_SET_TAGGED_ADDR_CTRL,PR_TAGGED_ADDR_ENABLE,0,0,0)) return 77;
        address=(address&UINT64_C(0x00ffffffffffffff))|UINT64_C(0xb400000000000000);
    }
    if (mode==0) {
        asm volatile("sevl" ::: "memory");
        return 0;
    }
    // Fix X0 and Q0 to match the faulting SDK instruction, independently of C++.
    register uintptr_t target asm("x0")=address;
    if (mode==5||mode==6) {
        // Reproduce SDK c424..c43c, including vector operations before STP.
        // Reserve our own stack slots instead of touching the caller's frame.
        asm volatile("sub sp, sp, #0x100\nmov x22, #32\n"
                     "dup v1.2d, x0\ndup v2.2d, x22\nmovi v0.2d, #0\n"
                     "add v1.2d, v1.2d, v2.2d\nstr x0, [sp, #0x90]\n"
                     "stp q0, q0, [x0]\nstur q1, [sp, #0x98]\nadd sp, sp, #0x100"
                     : : "r"(target) : "x22","v0","v1","v2","memory");
        for (unsigned i=0;i<64;++i) if (plain[i]!=(i<32?0:0xa5)) return 16;
        return 0;
    }
    if (mode==2) {
        asm volatile("mov x1, #0x5a\nstrb w1, [x0]" : : "r"(target) : "x1","memory");
        return plain[0]==0x5a&&plain[1]==0xa5?0:14;
    }
    if (mode==3) {
        asm volatile("movi v0.16b, #0x5a\nstr q0, [x0]" : : "r"(target) : "v0","memory");
    } else {
        asm volatile("movi v0.16b, #0x5a\nstp q0, q0, [x0]" : : "r"(target) : "v0","memory");
    }
    const unsigned written=mode==3?16:32;
    for (unsigned i=0;i<64;++i) if (plain[i]!=(i<written?0x5a:0xa5)) return 15;
    return 0;
}

int main() {
    // Refuse execution on any running Android app/device or ordinary host shell.
    if (getpid()!=1) return 2;
    setvbuf(stdout,nullptr,_IONBF,0);
    utsname kernel{};
    if (uname(&kernel)) return 3;
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
    fault_record=static_cast<FaultRecord*>(mmap(nullptr,sizeof(FaultRecord),
        PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if (fault_record==MAP_FAILED) return 4;
    std::printf("SEED_CPU_INIT_V3_DIAGNOSTIC kernel=%s machine=%s pid=1\n",kernel.release,kernel.machine);
#else
    std::printf("SEED_CPU_INIT_V2 kernel=%s machine=%s pid=1\n",kernel.release,kernel.machine);
#endif
    constexpr const char *names[]{"sevl","plain-stp-q","tagged-strb","tagged-str-q","tagged-stp-q","plain-sdk-sequence","tagged-sdk-sequence"};
    unsigned passed=0,failed=0,skipped=0;
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
    constexpr int count=8;
#else
    constexpr int count=7;
#endif
    for (int mode=0;mode<count;++mode) {
        const char *name=mode<7?names[mode]:"intentional-null-read-control";
        std::printf("SEED_CPU_BEGIN %s\n",name);
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
        std::memset(fault_record,0,sizeof(*fault_record));
#endif
        const pid_t child=fork();
        if (child==0) _exit(probe(mode));
        if (child<0) {++failed;std::printf("SEED_CPU_FAIL fork errno=%d\n",errno);continue;}
        int status=0;pid_t got;
        do {got=waitpid(child,&status,0);} while(got<0&&errno==EINTR);
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
        if (fault_record->signal) std::printf(
            "SEED_CPU_FAULT %s signal=%u code=%u address=%llx pc=%llx sp=%llx x0=%llx x1=%llx x22=%llx\n",
            name,fault_record->signal,fault_record->code,
            static_cast<unsigned long long>(fault_record->address),
            static_cast<unsigned long long>(fault_record->pc),
            static_cast<unsigned long long>(fault_record->sp),
            static_cast<unsigned long long>(fault_record->x0),
            static_cast<unsigned long long>(fault_record->x1),
            static_cast<unsigned long long>(fault_record->x22));
        if (mode==7) {
            const bool expected=got==child&&WIFSIGNALED(status)&&WTERMSIG(status)==SIGSEGV&&
                fault_record->signal==SIGSEGV&&fault_record->address==0;
            std::printf("SEED_CPU_DIAGNOSTIC_CONTROL %s child_reaped=%u\n",expected?"PASS":"FAIL",got==child);
            if (!expected) ++failed;
            continue; // A deliberately fatal child is never counted as CPU success.
        }
#endif
        if (got!=child) {++failed;std::printf("SEED_CPU_FAIL wait\n");}
#ifdef SEED_TCG_FAULT_DIAGNOSTICS
        else if (fault_record->signal) {
            ++failed;std::printf("SEED_CPU_FAIL %s recorded_signal=%u terminal_signal=%d exit=%d\n",
                name,fault_record->signal,WIFSIGNALED(status)?WTERMSIG(status):0,
                WIFEXITED(status)?WEXITSTATUS(status):-1);
        }
#endif
        else if (WIFEXITED(status)&&WEXITSTATUS(status)==0) {++passed;std::printf("SEED_CPU_PASS %s\n",names[mode]);}
        else if (WIFEXITED(status)&&WEXITSTATUS(status)==77) {++skipped;std::printf("SEED_CPU_SKIP %s tagged-abi-unavailable\n",names[mode]);}
        else {++failed;std::printf("SEED_CPU_FAIL %s signal=%d exit=%d\n",names[mode],WIFSIGNALED(status)?WTERMSIG(status):0,WIFEXITED(status)?WEXITSTATUS(status):-1);}
    }
    std::printf("SEED_CPU_RESULT passed=%u failed=%u skipped=%u android_app_qualification=0\n",passed,failed,skipped);
    sync();
    reboot(RB_POWER_OFF);
    // Remain alive as PID 1 if the virtual board does not implement power-off.
    for (;;) pause();
}
