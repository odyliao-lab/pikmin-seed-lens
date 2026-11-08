#include "supervised_load.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <sys/mman.h>
static std::string startTicks(pid_t pid){
    std::ifstream file("/proc/"+std::to_string(pid)+"/stat");std::string text;
    std::getline(file,text);auto end=text.rfind(')');if(end==text.npos)return {};
    std::istringstream fields(text.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(fields>>value))return {};return value;
}
int main(int argc,char**){
    if(argc!=1)return 2;
    alarm(45);int checks=0;
    for(int scenario=0;scenario<7;++scenario){
        char directory[]="/data/local/tmp/seed-supervise-test-XXXXXX";if(!mkdtemp(directory))return 2;
        seed_root::LoadLease lease(directory,getuid());if(!lease.held())return 3;
        pid_t parent=getpid(),target=fork();if(target<0)return 4;
        if(!target){if(prctl(PR_SET_PDEATHSIG,SIGKILL)||getppid()!=parent)_exit(3);for(;;)pause();}
        // Only our direct, unreaped child. No external PID can be supplied.
        struct Cleanup{pid_t p;~Cleanup(){kill(p,SIGKILL);
            auto until=std::chrono::steady_clock::now()+std::chrono::seconds(2);
            while(std::chrono::steady_clock::now()<until){auto got=waitpid(p,nullptr,WNOHANG);
                if(got==p||(got<0&&errno!=EINTR))break;usleep(1000);}}} cleanup{target};
        int fd=int(syscall(SYS_pidfd_open,target,0));if(fd<0)return 5;
        auto start=startTicks(target);
#if defined(__aarch64__)
        constexpr auto backend="arm64";
#else
        constexpr auto backend="nativebridge";
#endif
        seed_root::LoadAttempt attempt(lease,"12345678-1234-1234-1234-123456789abc",target,start,10001,getpid(),startTicks(getpid()),backend);
        auto result=seed_root::supervised_load(fd,attempt,[&](int socket){
            if(scenario==1)return 7; // fails before any tracee modification
            if(scenario==4){seed_root::worker_phase(socket,'X');return 8;}
            if(!seed_root::worker_phase(socket,'A')||!seed_root::worker_phase(socket,'C'))return 9;
            if(scenario==2)return 10;
            if(scenario==3){for(;;)pause();}
            if(!seed_root::worker_phase(socket,'R'))return 11;
            if(scenario==6)return 12;
            if(!seed_root::worker_phase(socket,'D'))return 13;
            return scenario==5?14:0;
        },scenario==3?100:2000);
        bool shouldDie=scenario==2||scenario==3||scenario==6;
        pollfd dead{fd,POLLIN,0};bool exited=poll(&dead,1,0)>0&&(dead.revents&POLLIN);close(fd);
        auto expected=scenario==0?seed_root::SupervisedResult::Returned:shouldDie?seed_root::SupervisedResult::UnknownContained:seed_root::SupervisedResult::Refused;
        if(result!=expected||exited!=shouldDie){printf("FAIL scenario=%d result=%d dead=%d\n",scenario,int(result),exited);return 15;}
        ++checks;printf("SUPERVISOR_CASE_PASS %d target_stopped=%d\n",scenario,exited);
    }
    printf("SUPERVISOR_PASS %d OWN_CHILDREN_ONLY\n",checks);return 0;
}
