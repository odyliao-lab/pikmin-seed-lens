#pragma once
// Research only. The caller must own a stopped tracee and a dedicated writable
// stack with guard pages. This is NOT rollback of TLS, heap, locks or dlopen.
// UNKNOWN latches this object: no retry, implicit detach, cleanup or signal loss.
#include "KittyTrace.hpp"
#include "api32_user_context.h"
#include <array>
#include <chrono>
#include <vector>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <elf.h>
#if !defined(__x86_64__)
#error x86_64 research backend only
#endif
namespace seed_research {
enum class CallState { Refused, Returned, Unknown };
struct CallResult { CallState state=CallState::Refused; uintptr_t value=0; bool cpuRestored=false; int signal=0; };
class X64CallContext {
    KittyTraceMgr& trace_;
    bool poisoned_=false;
    user_regs_struct original_{};
    std::vector<unsigned char> xstate_;
    std::array<unsigned char,128> redzone_{};
    static bool xread(pid_t pid,std::vector<unsigned char>& out) {
        out.assign(65536,0); iovec io{out.data(),out.size()};
        if(ptrace(PTRACE_GETREGSET,pid,NT_X86_XSTATE,&io)!=0 || io.iov_len<576 || io.iov_len>=out.size()) return false;
        out.resize(io.iov_len); return true;
    }
    bool capture() {
        bool regs=trace_.getRegs(&original_);
        bool user=regs&&user_context_registers(original_,trace_.pid(),true);
        bool stack=user&&original_.rsp>=redzone_.size();
        bool state=stack&&xread(trace_.pid(),xstate_);
        bool red=state&&trace_.peekMem(original_.rsp-redzone_.size(),redzone_.data(),redzone_.size())==redzone_.size();
#ifdef SEED_API32_NATIVE_CONTEXT
        if(!red)printf("API32_CONTEXT_CAPTURE_REFUSED regs=%d orig=%lld rax=%lld stack=%d xstate=%d redzone=%d\n",regs,(long long)original_.orig_rax,(long long)original_.rax,stack,state,red);
#endif
        return red;
    }
    bool restore() {
        if(!trace_.stop())return false;
        iovec io{xstate_.data(),xstate_.size()};
        bool xok=ptrace(PTRACE_SETREGSET,trace_.pid(),NT_X86_XSTATE,&io)==0;
        bool gok=trace_.setRegs(&original_);
        user_regs_struct actual{};std::vector<unsigned char> x;
        std::array<unsigned char,128> redzone{};
        return xok && gok && trace_.getRegs(&actual) && !memcmp(&original_,&actual,sizeof(actual)) &&
            xread(trace_.pid(),x) && x==xstate_ &&
            trace_.peekMem(original_.rsp-redzone.size(),redzone.data(),redzone.size())==redzone.size() && redzone==redzone_;
    }
public:
    explicit X64CallContext(KittyTraceMgr& trace):trace_(trace){}
    X64CallContext(const X64CallContext&)=delete;
    X64CallContext& operator=(const X64CallContext&)=delete;
    bool poisoned()const{return poisoned_;}
    // At most six integer/pointer arguments; no varargs/FP ABI inference. A zero
    // return sentinel is recognized only at the expected synchronous fault. A
    // nonzero sentinel requires caller-verified readable, non-executable mapping.
    // Other signals are left stopped and reported, never swallowed or replayed.
    CallResult call(uintptr_t function,const std::array<uintptr_t,6>& args,
                    uintptr_t stackBase,size_t stackSize,int timeoutMs,uintptr_t returnAddress=0) {
        if(poisoned_ || !function || timeoutMs<1 || timeoutMs>2000 ||
            stackSize<16384 || stackSize>1048576 || stackBase>UINTPTR_MAX-stackSize ||
            !trace_.autoRestoreRegs() || !capture())return {};
        uintptr_t end=stackBase+stackSize;
        if(stackBase<=original_.rsp && end>=original_.rsp-128)return {};
        auto regs=original_;regs.rsp=(end&~uintptr_t(15))-8;
        uintptr_t sentinel=returnAddress;
        if(trace_.pokeMem(regs.rsp,&sentinel,sizeof(sentinel))!=sizeof(sentinel))return {};
        regs.rip=function;regs.orig_rax=static_cast<unsigned long long>(-1);
        regs.rax=0;regs.rdi=args[0];regs.rsi=args[1];regs.rdx=args[2];
        regs.rcx=args[3];regs.r8=args[4];regs.r9=args[5];
        // From the first attempted register change onward, uncertainty consumes
        // the context. Even a restored CPU does not make side effects retryable.
        poisoned_=true;
        CallResult out{CallState::Unknown,0,false,0};
        if(!trace_.setRegs(&regs)){out.cpuRestored=restore();return out;}
        const auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(timeoutMs);
        if(!trace_.cont()){out.cpuRestored=restore();return out;}
        int status=0;
        auto left=std::chrono::duration_cast<std::chrono::milliseconds>(deadline-std::chrono::steady_clock::now()).count();
        auto got=left>0?trace_.wait(&status,__WALL,int(left)):0;
        if(got==trace_.pid() && WIFSTOPPED(status)) {
            out.signal=WSTOPSIG(status);siginfo_t info{};user_regs_struct returned{};
            if(out.signal==SIGSEGV && trace_.getSignalInfo(&info) &&
               info.si_code==(returnAddress?SEGV_ACCERR:SEGV_MAPERR) &&
               reinterpret_cast<uintptr_t>(info.si_addr)==returnAddress && trace_.getRegs(&returned) &&
               returned.rip==returnAddress && returned.rsp==(end&~uintptr_t(15))) {
                out.value=returned.rax;out.state=CallState::Returned;
            }
        }
        // Exited/unowned targets cannot be restored; stop() is bounded and all
        // ptrace writes plus read-back checks must pass before reporting restore.
        out.cpuRestored=restore();
        if(!out.cpuRestored)out.state=CallState::Unknown;
        if(out.state==CallState::Returned)poisoned_=false;
        return out;
    }
};
}
