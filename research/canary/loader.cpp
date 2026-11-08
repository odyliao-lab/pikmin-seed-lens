// Research-only containment harness. It CANNOT target the game or choose a library.
// EXITKILL and parent watchdog intentionally terminate ONLY our disposable probe
// on uncertainty. This is NOT a production timeout/recovery implementation.
#include "Injector/KittyInjector.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <chrono>
#include <poll.h>
#include <dirent.h>
#include "identity.h"

static std::string readFile(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
static std::string ticks(int p){
    auto s=readFile("/proc/"+std::to_string(p)+"/stat");auto n=s.rfind(')');if(n==s.npos)return {};
    std::istringstream stream(s.substr(n+2));std::string v;for(int i=3;i<=22;i++)if(!(stream>>v))return {};return v;
}
static bool identity(int p,const std::string& start){
    auto s=readFile("/proc/"+std::to_string(p)+"/cmdline");struct stat proc{},app{};
    return canary_command(s)&&!start.empty()&&ticks(p)==start
      &&!stat(("/proc/"+std::to_string(p)).c_str(),&proc)&&!stat("/data/user/0/dev.ody.seedlensprobe",&app)
      &&app.st_uid>=10000&&proc.st_uid==app.st_uid;
}
static int selfThread(int p){
    auto dir="/proc/"+std::to_string(p)+"/task";DIR* d=opendir(dir.c_str());if(!d)return -1;
    int found=-1;while(auto* e=readdir(d)){char* end=nullptr;long tid=strtol(e->d_name,&end,10);
        if(!end||*end||tid<=0||tid>INT_MAX)continue;
        if(readFile(dir+"/"+e->d_name+"/comm")=="SeedLensProbe\n"){
            if(found!=-1){found=-1;break;}found=int(tid);
        }
    }closedir(d);return found;
}
static bool threadIdentity(int p,int tid,const std::string& start){
    if(tid<=0 || ticks(tid)!=start || selfThread(p)!=tid)return false;
    auto status=readFile("/proc/"+std::to_string(tid)+"/status");
    return status.find("\nTgid:\t"+std::to_string(p)+"\n")!=status.npos;
}
static int run(int p,const std::string& start,bool timeoutTest){
    if(!identity(p,start))return 10;
    int tid=timeoutTest?p:selfThread(p);auto threadStart=ticks(tid);
    if(!timeoutTest&&!threadIdentity(p,tid,threadStart))return 16;
    KittyMemoryMgr mgr{};
    if(!mgr.initialize(p,EK_MEM_OP_SYSCALL,true))return 12;
    mgr.trace=KittyTraceMgr(tid,0,true,2000);
    if(!mgr.trace.seize(PTRACE_O_TRACESYSGOOD|PTRACE_O_EXITKILL)||!mgr.trace.stop())return 11;
    if(timeoutTest){puts("SELF_PROBE_TIMEOUT_TEST_ARMED");for(;;)pause();}
    // Do not detach on failure: EXITKILL contains a possibly modified probe.
    if(!identity(p,start)||!threadIdentity(p,tid,threadStart))_exit(12);
    printf("SELF_THREAD_SELECTED tid=%d tgid=%d\n",tid,p);
    inject_elf_config_t cfg{};cfg.sdk=35;cfg.timeout=2000;cfg.seize=true;cfg.package="dev.ody.seedlensprobe";
    cfg.selinux_state=Utils::selinux_state();
    KittyInjector inj;
    if(!inj.init(&mgr,cfg))_exit(13);
    auto result=inj.inject("/data/local/tmp/seed-lens-canary-host/libseedprobe_host.so");
    if(!result.is_valid())_exit(14);
    if(!mgr.trace.detach())_exit(15);
    puts("HOST_LOAD_RETURNED_CHECK_SELF_RECEIPT");return 0;
}
int main(int argc,char** argv){
    setbuf(stdout,nullptr);
    bool timeoutTest=argc==4&&!strcmp(argv[3],"--timeout-test");
    if(getuid()!=0||(argc!=3&&!timeoutTest))return 2;
    char* end=nullptr;long value=strtol(argv[1],&end,10);
    if(!end||*end||value<=1||value>INT_MAX)return 3;
    int p=int(value);std::string start=argv[2];
    if(!identity(p,start))return 4;
    struct stat st{};const char* lib="/data/local/tmp/seed-lens-canary-host/libseedprobe_host.so";
    if(lstat(lib,&st)||!S_ISREG(st.st_mode)||st.st_uid||st.st_mode&0022)return 5;
    // Pin the exact probe process for containment even if numeric PID is reused.
    int pidfd=int(syscall(SYS_pidfd_open,p,0));if(pidfd<0||!identity(p,start))return 6;
    pid_t worker=fork();if(worker<0)return 7;
    if(!worker){close(pidfd);_exit(run(p,start,timeoutTest));}
    auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(timeoutTest?3:15);int status=0;
    while(std::chrono::steady_clock::now()<deadline){
        pid_t found=waitpid(worker,&status,WNOHANG);
        if(found==worker){
            bool ok=WIFEXITED(status)&&WEXITSTATUS(status)==0;
            if(!ok)syscall(SYS_pidfd_send_signal,pidfd,SIGKILL,nullptr,0);
            close(pidfd);return ok?0:20;
        }
        if(found<0)break;
        usleep(10000);
    }
    syscall(SYS_pidfd_send_signal,pidfd,SIGKILL,nullptr,0);
    kill(worker,SIGKILL);
    bool reaped=false;
    for(int i=0;i<300;i++){if(waitpid(worker,&status,WNOHANG)==worker){reaped=true;break;}usleep(10000);}
    pollfd exited{pidfd,POLLIN,0};bool dead=poll(&exited,1,1000)>0&&(exited.revents&POLLIN);
    close(pidfd);
    if(timeoutTest&&reaped&&dead){puts("SELF_PROBE_CONTAINMENT_PASS");return 0;}
    puts("UNKNOWN_PROBE_TERMINATED_NOT_GAME");return 21;
}
