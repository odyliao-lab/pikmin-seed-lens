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

#if !defined(__aarch64__)
#error This fixture requires AArch64.
#endif

static int probe(int mode) {
    alarm(5);
    const rlimit no_core{0,0};
    if (setrlimit(RLIMIT_CORE,&no_core)) return 10;
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
    std::printf("SEED_CPU_INIT_V2 kernel=%s machine=%s pid=1\n",kernel.release,kernel.machine);
    constexpr const char *names[]{"sevl","plain-stp-q","tagged-strb","tagged-str-q","tagged-stp-q","plain-sdk-sequence","tagged-sdk-sequence"};
    unsigned passed=0,failed=0,skipped=0;
    for (int mode=0;mode<7;++mode) {
        std::printf("SEED_CPU_BEGIN %s\n",names[mode]);
        const pid_t child=fork();
        if (child==0) _exit(probe(mode));
        if (child<0) {++failed;std::printf("SEED_CPU_FAIL fork errno=%d\n",errno);continue;}
        int status=0;pid_t got;
        do {got=waitpid(child,&status,0);} while(got<0&&errno==EINTR);
        if (got!=child) {++failed;std::printf("SEED_CPU_FAIL wait\n");}
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
