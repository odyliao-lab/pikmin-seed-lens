#include <jni.h>
#include <android/api-level.h>
#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ucontext.h>
#include <unistd.h>

#if !defined(__aarch64__)
#error ARM64-only own-App signal observation
#endif

namespace {
struct Record {
    uint32_t magic=0x534c5331, version=1, bytes=112, phase=0;
    uint32_t mode=0, api=0, signal=0;
    int32_t code=0;
    uint64_t pid=0, uid=0, expectedPc=0, savedPc=0, faultAddress=0;
    uint64_t savedSp=0, savedPstate=0, x19=0, x20=0, flags=0;
};
static_assert(sizeof(Record)==112);
static_assert(std::atomic<uint64_t>::is_always_lock_free);
static_assert(std::atomic<int>::is_always_lock_free);
static_assert(std::atomic<bool>::is_always_lock_free);
std::atomic<int> recordFd{-1}, selectedMode{-1}, nativeApi{0};
std::atomic<uint64_t> expectedPc{0}, ownPid{0}, ownUid{0};
std::atomic<bool> used{false}, resumeHandlerSeen{false};
alignas(16) unsigned char alternateStack[65536];

extern "C" void own_udf(),own_sevl(),own_pair(),own_wfe();
extern "C" const char own_udf_fault[],own_sevl_fault[],own_pair_fault[],own_wfe_fault[];
extern "C" uint64_t own_resume_udf(),own_resume_bad();
extern "C" const char own_resume_udf_fault[],own_resume_bad_fault[];

Record baseRecord(uint32_t phase) {
    Record r;
    r.phase=phase; r.mode=selectedMode.load(std::memory_order_relaxed);
    r.api=nativeApi.load(std::memory_order_relaxed);
    r.pid=ownPid.load(std::memory_order_relaxed);
    r.uid=ownUid.load(std::memory_order_relaxed);
    r.expectedPc=expectedPc.load(std::memory_order_relaxed);
    return r;
}

bool append(const Record& r) {
    const int fd=recordFd.load(std::memory_order_relaxed);
    return write(fd,&r,sizeof(r))==sizeof(r) && fsync(fd)==0;
}

void observeAndExit(int signal,siginfo_t* info,void* context) {
    // Public NDK fields are observations, not presumed translator conformance.
    // Original modes remain fail-stop. The two explicit resume modes below
    // concern only this module's known UDF and never a game/event instruction.
    Record r=baseRecord(3);
    r.signal=signal;
    if(info) { r.flags|=1; r.code=info->si_code;
        r.faultAddress=reinterpret_cast<uintptr_t>(info->si_addr); }
    if(context) {
        r.flags|=2;
        const auto* uc=static_cast<const ucontext_t*>(context);
        r.savedPc=uc->uc_mcontext.pc; r.savedSp=uc->uc_mcontext.sp;
        r.savedPstate=uc->uc_mcontext.pstate;
        r.x19=uc->uc_mcontext.regs[19]; r.x20=uc->uc_mcontext.regs[20];
    }
    if(r.mode==7||r.mode==8) {
        // A translator ignoring the PC write must not cause a signal loop.
        if(resumeHandlerSeen.exchange(true))_exit(88);
        const bool knownCode=r.code==1||r.code==2||r.code==-6||r.code==0;
        const bool allowed=signal==SIGILL&&r.flags==3&&knownCode&&r.expectedPc&&
                r.savedPc==r.expectedPc&&r.savedSp&&!(r.savedSp&15)&&
                r.x19==0x534c&&r.x20==0x1357;
        if(!allowed)_exit(append(r)?89:87);
        // Flag value4 records intent BEFORE the write, not its successful effect.
        // Only the later native canary + phase4 + Java return can prove a round
        // trip. Synthetic bridge siginfo is not promoted to hardware siginfo.
        r.flags|=4;
        if(!append(r))_exit(87);
        static_cast<ucontext_t*>(context)->uc_mcontext.pc=r.expectedPc+4;
        return;
    }
    // Successful original observation deliberately exits the own process86.
    // It does not turn an illegal instruction into successful execution.
    _exit(append(r)?86:87);
}
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*) { return JNI_VERSION_1_6; }

extern "C" JNIEXPORT jint JNICALL
Java_dev_ody_seedlensarmsignalprobe_SignalInstrumentation_runCase(
        JNIEnv*,jclass,jint mode,jint expectedApi,jint fd) {
    const int api=android_get_device_api_level();
    struct stat st{};
    const int flags=fcntl(fd,F_GETFL);
    if(mode<0||mode>8||api!=expectedApi||(api!=31&&api!=34&&api!=35)||
       getuid()<10000||getuid()!=geteuid()||fstat(fd,&st)||!S_ISREG(st.st_mode)||
       st.st_uid!=getuid()||st.st_nlink!=1||(st.st_mode&0777)!=0600||st.st_size!=0||
       flags<0||(flags&O_ACCMODE)!=O_WRONLY||!(flags&O_APPEND)||used.exchange(true))return -1;
    recordFd.store(fd); selectedMode.store(mode); nativeApi.store(api);
    ownPid.store(getpid()); ownUid.store(getuid());
    expectedPc.store(mode==2?reinterpret_cast<uintptr_t>(own_udf_fault):
                     mode==3?reinterpret_cast<uintptr_t>(own_sevl_fault):
                     mode==4?reinterpret_cast<uintptr_t>(own_pair_fault):
                     mode==5?reinterpret_cast<uintptr_t>(own_wfe_fault):
                     mode==7?reinterpret_cast<uintptr_t>(own_resume_udf_fault):
                     mode==8?reinterpret_cast<uintptr_t>(own_resume_bad_fault):0);
    if(!append(baseRecord(1)))return -2;
    struct sigaction action{},previous{};
    action.sa_sigaction=observeAndExit; action.sa_flags=SA_SIGINFO|SA_ONSTACK;
    stack_t stack{},oldStack{};
    stack.ss_sp=alternateStack; stack.ss_size=sizeof(alternateStack);
    if(sigemptyset(&action.sa_mask)||sigaltstack(&stack,&oldStack)||
       sigaction(SIGILL,&action,&previous))return -3;
    // Common setup for all modes. Default SIGALRM bounds only this own process.
    struct sigaction alarmAction{};
    alarmAction.sa_handler=SIG_DFL;
    if(sigemptyset(&alarmAction.sa_mask)||sigaction(SIGALRM,&alarmAction,nullptr))return -4;
    alarm(3);
    if(!append(baseRecord(2)))return -5;
    if(mode==1)raise(SIGILL);
    else if(mode==2)own_udf();
    else if(mode==3)own_sevl();
    else if(mode==4)own_pair();
    else if(mode==5)own_wfe();
    // Intentional timeout control in this own disposable process only. The
    // default SIGALRM action must terminate it; never report a native return.
    else if(mode==6)for(;;)pause();
    else if(mode==7||mode==8) {
        const uint64_t canary=mode==7?own_resume_udf():own_resume_bad();
        // The bad-marker case must never reach here. A normal UDF return with
        // no handler, or an incorrect landing, is not a successful round trip.
        if(mode!=7||canary!=0x5eed||!resumeHandlerSeen.load())_exit(90);
    }
    alarm(0);
    if(sigaction(SIGILL,&previous,nullptr)||sigaltstack(&oldStack,nullptr))return -6;
    if(!append(baseRecord(4)))return -7;
    return api;
}
