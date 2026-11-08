#pragma once
// Bounded ARM64 CPU transaction. UNKNOWN never resumes or retries; the owner
// must contain its pinned tracee. CPU restoration is NOT dlopen/heap rollback.
#include "KittyTrace.hpp"
#include <array>
#include <chrono>
#include <fstream>
#include <asm/hwcap.h>
#include <asm/ptrace.h>
#include <sys/auxv.h>
#include <sys/uio.h>
#include <elf.h>
#if !defined(__aarch64__)
#error ARM64 context only
#endif
namespace seed_arm64 {
template<class T> inline bool readReg(int tid,int note,T& value){
    value={};iovec io{&value,sizeof(value)};
    return ptrace(PTRACE_GETREGSET,tid,note,&io)==0&&io.iov_len==sizeof(value);
}
template<class T> inline bool writeReg(int tid,int note,T& value){
    iovec io{&value,sizeof(value)};
    return ptrace(PTRACE_SETREGSET,tid,note,&io)==0&&io.iov_len==sizeof(value);
}
inline bool hardwareQualified(){
    auto hw=getauxval(AT_HWCAP),hw2=getauxval(AT_HWCAP2);
    // This backend does not yet preserve scalable vector, matrix, tagged-memory
    // or PAC key state. Fail before CALLING instead of discarding those states.
    return !(hw&(HWCAP_SVE|HWCAP_PACA|HWCAP_PACG))&&!(hw2&(HWCAP2_SME|HWCAP2_MTE));
}
inline bool absentRegset(int tid,int note){
    std::array<unsigned char,32> data{};iovec io{data.data(),data.size()};errno=0;
    if(ptrace(PTRACE_GETREGSET,tid,note,&io)==0)return false;
    return errno==EINVAL||errno==EIO;
}
inline bool userContext(int tid){
    user_pt_regs regs{};int number=-2;
    return readReg(tid,NT_PRSTATUS,regs)&&readReg(tid,NT_ARM_SYSTEM_CALL,number)&&number==-1&&
        !(regs.pstate&(PSR_MODE_MASK|PSR_MODE32_BIT|KT_CPSR_SS_MASK|KT_CPSR_D_MASK))&&!(regs.sp&15);
}
struct Snapshot {
    user_pt_regs gpr{};user_fpsimd_state fp{};uint64_t tls=0;int syscall=-2;
    std::array<unsigned char,128> stack{};
    bool capture(KittyTraceMgr& trace){
        auto tid=trace.pid();
        return hardwareQualified()&&absentRegset(tid,NT_ARM_SVE)&&absentRegset(tid,NT_ARM_SSVE)&&
            readReg(tid,NT_PRSTATUS,gpr)&&readReg(tid,NT_PRFPREG,fp)&&readReg(tid,NT_ARM_TLS,tls)&&
            readReg(tid,NT_ARM_SYSTEM_CALL,syscall)&&syscall==-1&&gpr.sp&&!(gpr.sp&15)&&
            !(gpr.pstate&(PSR_MODE_MASK|PSR_MODE32_BIT|KT_CPSR_SS_MASK|KT_CPSR_D_MASK))&&
            gpr.sp<=UINTPTR_MAX-stack.size()&&trace.peekMem(gpr.sp,stack.data(),stack.size())==stack.size();
    }
    bool same(const Snapshot& other)const{
        return !memcmp(&gpr,&other.gpr,sizeof(gpr))&&!memcmp(&fp,&other.fp,sizeof(fp))&&
            tls==other.tls&&syscall==other.syscall&&stack==other.stack;
    }
    bool restore(KittyTraceMgr& trace){
        // Caller has consumed an exact synchronous completion, not a timeout.
        auto tid=trace.pid();
        if(!writeReg(tid,NT_PRFPREG,fp)||!writeReg(tid,NT_ARM_TLS,tls)||
           !writeReg(tid,NT_ARM_SYSTEM_CALL,syscall)||!writeReg(tid,NT_PRSTATUS,gpr))return false;
        Snapshot actual;return actual.capture(trace)&&same(actual);
    }
};
inline bool mapping(int tid,uintptr_t address,size_t size,bool executable){
    if(!address||!size||address>UINTPTR_MAX-size)return false;
    std::ifstream maps("/proc/"+std::to_string(tid)+"/maps");std::string line;
    while(std::getline(maps,line)){
        unsigned long long lo=0,hi=0;char perms[5]{};
        if(sscanf(line.c_str(),"%llx-%llx %4s",&lo,&hi,perms)==3&&address>=lo&&address+size<=hi)
            return perms[0]=='r'&&(executable?perms[2]=='x':perms[2]!='x');
    }return false;
}
enum class State{Refused,Returned,Unknown};
struct Result{State state=State::Refused;intptr_t value=0;bool restored=false;int signal=0;};
class Context {
    KittyTraceMgr& trace_;bool poisoned_=false;
    bool contract(int timeout)const{return !poisoned_&&trace_.autoRestoreRegs()&&timeout>=1&&timeout<=2000;}
public:
    explicit Context(KittyTraceMgr& trace):trace_(trace){}
    bool poisoned()const{return poisoned_;}
    Result syscall(long number,const std::array<uintptr_t,6>& args,int timeout){
        auto pc=trace_.syscallGadget();uint32_t instruction=0;Snapshot original;
        if(!contract(timeout)||number<0||number>INT_MAX||!pc||(pc&3)||
           !mapping(trace_.pid(),pc,4,true)||trace_.peekMem(pc,&instruction,4)!=4||instruction!=0xd4000001||
           !original.capture(trace_))return {};
        auto regs=original.gpr;regs.pc=pc;regs.regs[8]=number;
        for(size_t i=0;i<args.size();++i)regs.regs[i]=args[i];
        regs.pstate&=~static_cast<uint64_t>(PSR_BTYPE_MASK);
        poisoned_=true;Result out{State::Unknown};
        if(!writeReg(trace_.pid(),NT_PRSTATUS,regs))return out;
        auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(timeout);
        user_pt_regs returned{};
        for(int phase=0;phase<2;++phase){
            if(ptrace(PTRACE_SYSCALL,trace_.pid(),nullptr,nullptr))return out;
            auto left=std::chrono::duration_cast<std::chrono::milliseconds>(deadline-std::chrono::steady_clock::now()).count();
            int status=0;auto got=left>0?trace_.wait(&status,__WALL,int(left)):0;
            if(got!=trace_.pid()||!WIFSTOPPED(status))return out;
            out.signal=WSTOPSIG(status);int active=-2;
            if(out.signal!=(SIGTRAP|0x80)||(status>>16)!=0||
               !readReg(trace_.pid(),NT_ARM_SYSTEM_CALL,active)||active!=number||
               !readReg(trace_.pid(),NT_PRSTATUS,returned)||returned.pc!=pc+4)return out;
        }
        out.value=static_cast<intptr_t>(returned.regs[0]);
        if(out.value<=-512&&out.value>=-516)return out;
        // ARM64 syscall tracing temporarily owns x7. At a syscall-exit stop,
        // SETREGSET/read-back can agree yet the kernel overwrites x7 when the
        // stop is left. Finish that kernel transition at an exact user fault
        // BEFORE restoring the original user snapshot. No gadget+4 executes.
        returned.pc=0;
        if(!writeReg(trace_.pid(),NT_PRSTATUS,returned)||!trace_.cont())return out;
        auto left=std::chrono::duration_cast<std::chrono::milliseconds>(deadline-std::chrono::steady_clock::now()).count();
        int status=0;auto got=left>0?trace_.wait(&status,__WALL,int(left)):0;
        siginfo_t info{};user_pt_regs boundary{};
        if(got!=trace_.pid()||!WIFSTOPPED(status)||WSTOPSIG(status)!=SIGSEGV||(status>>16)!=0||
           !trace_.getSignalInfo(&info)||info.si_code!=SEGV_MAPERR||info.si_addr!=nullptr||
           !readReg(trace_.pid(),NT_PRSTATUS,boundary)||boundary.pc!=0||boundary.sp!=original.gpr.sp)return out;
        out.restored=original.restore(trace_);
        if(out.restored){out.state=State::Returned;poisoned_=false;}
        return out;
    }
    Result function(uintptr_t function,const std::array<uintptr_t,8>& args,uintptr_t base,size_t size,int timeout,uintptr_t sentinel=0){
        Snapshot original;
        if(!contract(timeout)||!function||(function&3)||!base||(base&15)||size<16384||size>1048576||
           base>UINTPTR_MAX-size||!mapping(trace_.pid(),function,4,true)||
           (sentinel&&!mapping(trace_.pid(),sentinel,1,false))||!original.capture(trace_))return {};
        auto end=(base+size)&~uintptr_t(15);
        if(base<original.gpr.sp+original.stack.size()&&end>original.gpr.sp)return {};
        auto regs=original.gpr;regs.pc=function;regs.sp=end;regs.regs[30]=sentinel;
        regs.pstate&=~static_cast<uint64_t>(PSR_BTYPE_MASK);
        for(size_t i=0;i<args.size();++i)regs.regs[i]=args[i];
        poisoned_=true;Result out{State::Unknown};
        if(!writeReg(trace_.pid(),NT_PRSTATUS,regs)||!trace_.cont())return out;
        int status=0;auto got=trace_.wait(&status,__WALL,timeout);
        if(got!=trace_.pid()||!WIFSTOPPED(status))return out;
        out.signal=WSTOPSIG(status);siginfo_t info{};user_pt_regs returned{};
        if(out.signal!=SIGSEGV||(status>>16)!=0||!trace_.getSignalInfo(&info)||
           info.si_code!=(sentinel?SEGV_ACCERR:SEGV_MAPERR)||reinterpret_cast<uintptr_t>(info.si_addr)!=sentinel||
           !readReg(trace_.pid(),NT_PRSTATUS,returned)||returned.pc!=sentinel||returned.sp!=end)return out;
        out.value=static_cast<intptr_t>(returned.regs[0]);out.restored=original.restore(trace_);
        if(out.restored){out.state=State::Returned;poisoned_=false;}
        return out;
    }
};
}
