#include "load_journal.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <sys/wait.h>
#include <fstream>
#undef NDEBUG
#include <cassert>
using namespace seed_root;
static unsigned checks=0;
static void check(bool ok){++checks;if(!ok){fprintf(stderr,"CHECK_FAILED %u\n",checks);abort();}}
static constexpr const char* boot="12345678-abcd-1234-abcd-123456789abc";
static std::string dir(){char p[]="/data/local/tmp/seed-lens-journal-test-XXXXXX";auto x=mkdtemp(p);assert(x);return x;}
static std::string path(const std::string& d,int pid){return d+"/attempt-"+boot+"-"+std::to_string(pid)+"-12345";}
static std::string read(const std::string& p){std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
static void file(const std::string& p,const char* s){int f=open(p.c_str(),O_CREAT|O_EXCL|O_WRONLY,0600);assert(f>=0);assert(write(f,s,strlen(s))==ssize_t(strlen(s)));close(f);}
static void wait_child(pid_t p){int status=0;check(waitpid(p,&status,0)==p&&WIFEXITED(status)&&WEXITSTATUS(status)==0);}
int main(int argc,char**){
    if(argc!=1||getuid()!=0)return 2;
    alarm(20);setbuf(stdout,nullptr);
    check(boot_id(boot));check(!boot_id("../bad"));check(!boot_id("12345678-abcd-1234-abcd-123456789abg"));
    check(start_ticks("1"));check(start_ticks("18446744073709551615"));
    for(auto s:{"","0","01","1/2","1\n","18446744073709551616","123456789012345678901"})check(!start_ticks(s));
    auto d=dir();
    {
        LoadLease lease(d.c_str());check(lease.held());LoadLease contender(d.c_str());check(!contender.held());
        {
            LoadAttempt a(lease,boot,100,"12345",10001,getpid(),"99","arm64");check(a.ready());
            check(!a.record(LoadPhase::Calling));
            check(a.record(LoadPhase::Attaching));check(a.record(LoadPhase::Attached));check(a.record(LoadPhase::Calling));
        }
        check(read(path(d,100)).find("END\tUNKNOWN")!=std::string::npos);
        LoadAttempt duplicate(lease,boot,100,"12345",10001,getpid(),"99","arm64");check(!duplicate.ready());
        file(path(d,101),"attempted\n");
        LoadAttempt legacy(lease,boot,101,"12345",10001,getpid(),"99","arm64");check(!legacy.ready());
        check(read(path(d,101))=="attempted\n");
        LoadAttempt a(lease,boot,102,"12345",10001,getpid(),"99","arm64");check(a.ready());
        for(auto ph:{LoadPhase::Attaching,LoadPhase::Attached,LoadPhase::Calling,LoadPhase::Returned,LoadPhase::Detached,LoadPhase::ReturnedUnverified})check(a.record(ph));
        check(!a.ready());check(!a.record(LoadPhase::ReturnedUnverified));
        check(read(path(d,102)).find("RETURNED_UNVERIFIED")!=std::string::npos);
        check(read(path(d,102)).find("SUCCESS")==std::string::npos);
        LoadAttempt invalid(lease,"../bad",103,"12345",10001,getpid(),"99","arm64");check(!invalid.ready());
    }
    // Every pre-completion crash preserves a non-retryable record; no destructor.
    for(int stop=0;stop<=5;stop++){
        pid_t p=fork();check(p>=0);
        if(!p){LoadLease lease(d.c_str());LoadAttempt a(lease,boot,200+stop,"12345",10001,getpid(),"99","nativebridge");
            if(!a.ready())_exit(1);for(int i=1;i<=stop;i++)if(!a.record(static_cast<LoadPhase>(i)))_exit(2);_exit(0);}
        wait_child(p);LoadLease lease(d.c_str());check(lease.held());
        LoadAttempt again(lease,boot,200+stop,"12345",10001,getpid(),"99","nativebridge");check(!again.ready());
        check(!read(path(d,200+stop)).empty());
    }
    // A live foreign process holds the same inode; after death its attempt remains.
    int pipes[2];check(pipe(pipes)==0);pid_t p=fork();check(p>=0);
    if(!p){alarm(8);close(pipes[0]);LoadLease lease(d.c_str());LoadAttempt a(lease,boot,300,"12345",10001,getpid(),"99","arm64");
        char c=a.ready()?'Y':'N';write(pipes[1],&c,1);for(;;)pause();}
    close(pipes[1]);char c=0;check(::read(pipes[0],&c,1)==1&&c=='Y');close(pipes[0]);
    {LoadLease locked(d.c_str());check(!locked.held());}
    check(kill(p,SIGKILL)==0);int status=0;check(waitpid(p,&status,0)==p&&WIFSIGNALED(status));
    {LoadLease lease(d.c_str());check(lease.held());LoadAttempt a(lease,boot,300,"12345",10001,getpid(),"99","arm64");check(!a.ready());}
    auto weak=dir();chmod(weak.c_str(),0777);{LoadLease lease(weak.c_str());check(!lease.held());}
    chmod(weak.c_str(),0700);
    auto links=dir();file(links+"/victim","ORIGINAL");check(symlink("victim",(links+"/load.lock").c_str())==0);
    {LoadLease lease(links.c_str());check(!lease.held());}check(read(links+"/victim")=="ORIGINAL");
    auto hard=dir();file(hard+"/victim","ORIGINAL");check(link((hard+"/victim").c_str(),(hard+"/load.lock").c_str())==0);
    {LoadLease lease(hard.c_str());check(!lease.held());}check(read(hard+"/victim")=="ORIGINAL");
    auto loose=dir();file(loose+"/load.lock","");chmod((loose+"/load.lock").c_str(),0666);
    {LoadLease lease(loose.c_str());check(!lease.held());}
    chmod((loose+"/load.lock").c_str(),0600);
    auto fifo=dir();check(mkfifo((fifo+"/load.lock").c_str(),0600)==0);
    {LoadLease lease(fifo.c_str());check(!lease.held());}
    auto linkdir=dir();check(symlink(d.c_str(),(linkdir+"/link").c_str())==0);
    {LoadLease lease((linkdir+"/link").c_str());check(!lease.held());}
    printf("LOAD_JOURNAL_PASS checks=%u crash_points=6; fixtures retained, no game access\n",checks);
}
