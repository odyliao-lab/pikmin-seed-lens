#pragma once
// Read-only acquisition of a native user-space stop. Never edits registers,
// skips a syscall, swallows an external signal, or opts the game into EXITKILL.
// Eligibility is only a CPU boundary, NOT proof that dlopen cannot hold locks.
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <elf.h>
#include <unistd.h>
#include <cerrno>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
namespace seed_root {
inline std::string proc_start(int tid) {
    std::ifstream f("/proc/"+std::to_string(tid)+"/stat");std::string text;
    std::getline(f,text);auto end=text.rfind(')');if(end==text.npos)return {};
    std::istringstream in(text.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(in>>value))return {};return value;
}
inline bool thread_owner(int tid,int tgid,const std::string& start) {
    if(tid<=1||tgid<=1||start.empty()||proc_start(tid)!=start)return false;
    std::ifstream f("/proc/"+std::to_string(tid)+"/status");std::string line;
    while(std::getline(f,line))if(line.starts_with("Tgid:")){
        std::istringstream in(line.substr(5));int actual=0;return (in>>actual)&&actual==tgid;
    }
    return false;
}
class StoppedEntry {
    int tid_=-1,signal_=0;bool attached_=false,stopped_=false;
public:
    StoppedEntry()=default;
    StoppedEntry(const StoppedEntry&)=delete;
    ~StoppedEntry(){release();}
    bool acquire(int tid,int tgid,const std::string& start,int timeoutMs=250) {
        if(attached_||timeoutMs<1||timeoutMs>500||!thread_owner(tid,tgid,start))return false;
        if(ptrace(PTRACE_SEIZE,tid,nullptr,nullptr))return false;
        tid_=tid;attached_=true;
        if(ptrace(PTRACE_INTERRUPT,tid,nullptr,nullptr))return false;
        auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(timeoutMs);
        while(std::chrono::steady_clock::now()<deadline){
            int status=0;int got=waitpid(tid,&status,__WALL|WNOHANG);
            if(got<0&&errno==EINTR)continue;
            if(got<0)return false;
            if(!got){usleep(1000);continue;}
            if(WIFEXITED(status)||WIFSIGNALED(status)){attached_=false;return false;}
            if(!WIFSTOPPED(status))return false;
            stopped_=true;
            // Only our interrupt event can be consumed. Deliver any unrelated
            // signal back on detach, including group-stop signals.
            int event=status>>16;int sig=WSTOPSIG(status);
            if(event!=PTRACE_EVENT_STOP||sig!=SIGTRAP){signal_=sig;return false;}
            return thread_owner(tid,tgid,start);
        }return false;
    }
    bool user_context(user_regs_struct& regs)const {
        if(!attached_||!stopped_)return false;
        iovec io{&regs,sizeof(regs)};
        return !ptrace(PTRACE_GETREGSET,tid_,NT_PRSTATUS,&io)&&io.iov_len==sizeof(regs)
            &&static_cast<int64_t>(regs.orig_rax)==-1;
    }
    bool release() {
        if(!attached_)return true;
        if(!stopped_) {
            // No remote state was written. If the interrupt hasn't arrived,
            // collect it with one bounded wait before attempting detach.
            for(int i=0;i<250;++i){int status=0;int got=waitpid(tid_,&status,__WALL|WNOHANG);
                if(got==tid_){
                    if(WIFEXITED(status)||WIFSIGNALED(status)){attached_=false;return true;}
                    if(WIFSTOPPED(status)){stopped_=true;signal_=((status>>16)==PTRACE_EVENT_STOP&&WSTOPSIG(status)==SIGTRAP)?0:WSTOPSIG(status);break;}
                }else if(got<0&&errno!=EINTR)break;
                usleep(1000);
            }
        }
        if(!stopped_||ptrace(PTRACE_DETACH,tid_,nullptr,reinterpret_cast<void*>(static_cast<intptr_t>(signal_))))return false;
        attached_=false;stopped_=false;return true;
    }
};
}
