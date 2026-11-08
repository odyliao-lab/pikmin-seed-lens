#pragma once
// Explicitly consented fail-stop, NOT rollback of an interrupted dlopen.
// pidfd pins the exact target; only the worker is forked by this component.
// No target signal is sent before the durable CALLING acknowledgement.
#include "load_journal.h"
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <chrono>
namespace seed_root {
inline bool worker_phase(int socket,char phase){
    if(send(socket,&phase,1,MSG_NOSIGNAL)!=1)return false;
    char ack=0;ssize_t n;do{n=recv(socket,&ack,1,0);}while(n<0&&errno==EINTR);
    return n==1&&ack==phase;
}
enum class SupervisedResult { Returned, Refused, UnknownContained, ContainmentUnconfirmed };
template<class Worker> SupervisedResult supervised_load(int targetFd,LoadAttempt& attempt,Worker worker,int timeoutMs=15000){
    if(targetFd<0||!attempt.ready()||timeoutMs<10||timeoutMs>20000)return SupervisedResult::Refused;
    pollfd alive{targetFd,POLLIN,0};if(poll(&alive,1,0)!=0)return SupervisedResult::Refused;
    int sockets[2];if(socketpair(AF_UNIX,SOCK_SEQPACKET|SOCK_CLOEXEC,0,sockets))return SupervisedResult::Refused;
    if(!attempt.record(LoadPhase::Attaching)){close(sockets[0]);close(sockets[1]);return SupervisedResult::Refused;}
    pid_t parent=getpid(),child=fork();
    if(child<0){close(sockets[0]);close(sockets[1]);return SupervisedResult::Refused;}
    if(!child){
        close(sockets[0]);close(targetFd);
        if(prctl(PR_SET_PDEATHSIG,SIGKILL)||getppid()!=parent)_exit(70);
        _exit(worker(sockets[1]));
    }
    close(sockets[1]);bool uncertain=false,reaped=false,protocol=true;int step=0,status=0;
    const char phases[]={'A','C','R','D'};
    const LoadPhase records[]={LoadPhase::Attached,LoadPhase::Calling,LoadPhase::Returned,LoadPhase::Detached};
    auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(timeoutMs);
    while(protocol&&std::chrono::steady_clock::now()<deadline){
        pollfd message{sockets[0],POLLIN,0};int ready=poll(&message,1,10);
        if(ready<0&&errno!=EINTR){protocol=false;break;}
        if(ready>0&&(message.revents&POLLIN)){
            char bytes[2]{};auto n=recv(sockets[0],bytes,sizeof(bytes),0);
            if(n==0&&step==4){/* expected EOF after final acknowledgement */}
            else{
                if(n!=1||step>=4||bytes[0]!=phases[step]||!attempt.record(records[step])){protocol=false;break;}
                if(step==1)uncertain=true; // before allowing any tracee writes/calls
                if(step==3)uncertain=false; // worker has already detached successfully
                if(send(sockets[0],bytes,1,MSG_NOSIGNAL)!=1){protocol=false;break;}
                ++step;
            }
        }
        auto done=waitpid(child,&status,WNOHANG);
        if(done==child){reaped=true;break;}
        if(done<0&&errno!=EINTR){protocol=false;break;}
    }
    close(sockets[0]);
    bool success=protocol&&reaped&&WIFEXITED(status)&&WEXITSTATUS(status)==0&&step==4;
    if(success)return attempt.record(LoadPhase::ReturnedUnverified)?SupervisedResult::Returned:SupervisedResult::Refused;
    printf("SUPERVISED_RESULT phases=%d reaped=%d exit=%d signal=%d uncertain=%d\n",step,reaped,
        reaped&&WIFEXITED(status)?WEXITSTATUS(status):-1,reaped&&WIFSIGNALED(status)?WTERMSIG(status):0,uncertain);
    // No kill-by-numeric-target-PID fallback. Any unconfirmed containment is
    // reported and never used to authorize another load or to fake a receipt.
    bool dead=true;
    if(uncertain){
        int sent=int(syscall(SYS_pidfd_send_signal,targetFd,SIGKILL,nullptr,0));
        pollfd exited{targetFd,POLLIN,0};dead=(sent==0||errno==ESRCH)&&poll(&exited,1,2000)>0&&(exited.revents&POLLIN);
    }
    if(!reaped){kill(child,SIGKILL);for(int i=0;i<200;++i){if(waitpid(child,&status,WNOHANG)==child){reaped=true;break;}usleep(10000);}}
    if(!reaped||!dead)return SupervisedResult::ContainmentUnconfirmed;
    return uncertain?SupervisedResult::UnknownContained:SupervisedResult::Refused;
}
}
