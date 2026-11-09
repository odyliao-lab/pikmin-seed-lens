#include "control_transaction.h"
#include <sys/wait.h>
#include <csignal>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#undef NDEBUG
#include <cassert>
static unsigned checks=0;
static void check(bool ok){++checks;if(!ok){printf("FAIL %u\n",checks);abort();}}
static std::string read(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
int main(int argc,char**){
    if(argc!=1||getuid()!=0)return 2;alarm(15);setbuf(stdout,nullptr);
    char path[]="/data/local/tmp/seed-control-test-XXXXXX";check(mkdtemp(path));
    std::string directory=path;int parent=open(path,O_RDONLY|O_DIRECTORY|O_CLOEXEC);check(parent>=0);
    using namespace seed_root;
    {LoadLease lease(path);check(lease.held());
     check(commit_control(lease,parent,"display",false,100,"123"));check(read(directory+"/display")=="off\n");
     check(commit_control(lease,parent,"display",true,100,"123"));check(read(directory+"/display")=="on\n");
     check(!commit_control(lease,parent,"../escape",false,100,"123"));
     check(!commit_control(lease,parent,"display",false,100,"0"));
     LoadLease contender(path);check(!contender.held());
     check(!commit_control(contender,parent,"display",false,100,"123"));check(read(directory+"/display")=="on\n");
     check(symlink("display",(directory+"/link").c_str())==0);
     check(!commit_control(lease,parent,"link",false,100,"123"));check(read(directory+"/display")=="on\n");
     check(link((directory+"/display").c_str(),(directory+"/hard").c_str())==0);
     check(!commit_control(lease,parent,"display",false,100,"123"));
     check(mkfifo((directory+"/fifo").c_str(),0600)==0);
     check(!commit_control(lease,parent,"fifo",false,100,"123"));
     check(chmod((directory+"/display").c_str(),0666)==0);
     check(!commit_control(lease,parent,"display",false,100,"123"));}
    // Separate processes compete for the same lock inode. A completed control
    // remains readable after writer exit; the next explicit OFF is independent.
    {LoadLease lease(path);check(lease.held());pid_t child=fork();check(child>=0);
     if(!child){LoadLease other(path);_exit(other.held()?1:0);}
     int status=0;check(waitpid(child,&status,0)==child&&WIFEXITED(status)&&WEXITSTATUS(status)==0);}
    pid_t child=fork();check(child>=0);
    if(!child){LoadLease lease(path);_exit(commit_control(lease,parent,"after-exit",true,100,"123")?0:1);}
    int status=0;check(waitpid(child,&status,0)==child&&WIFEXITED(status)&&WEXITSTATUS(status)==0);
    check(read(directory+"/after-exit")=="on\n");
    {LoadLease lease(path);check(lease.held());check(commit_control(lease,parent,"after-exit",false,100,"123"));}
    check(read(directory+"/after-exit")=="off\n");
    const std::string good="v1\t100000\t100\t156.1\tready\t1\t5\t1\t4\t2\t123\n";
    check(fresh_renderer_status(good,100,"123",100000));
    check(!fresh_renderer_status(good,101,"123",100000));check(!fresh_renderer_status(good,100,"124",100000));
    check(!fresh_renderer_status(good,100,"123",105001));check(!fresh_renderer_status(good,100,"123",97999));
    check(!fresh_renderer_status(good+"\n",100,"123",100000));
    for(auto bad:{"v2","wrong",""}){auto text=good;text.replace(0,2,bad);check(!fresh_renderer_status(text,100,"123",100000));}
    check(!fresh_renderer_status(good.substr(0,good.size()-5)+"\n",100,"123",100000));
    auto text=good;text.replace(text.find("ready"),5,"incompatible");check(!fresh_renderer_status(text,100,"123",100000));
    text=good;text.replace(text.find("100000"),6,"18446744073709551616");check(!fresh_renderer_status(text,100,"123",100000));
    text=good;text.replace(text.find("156.1"),5,"156.2");check(!fresh_renderer_status(text,100,"123",100000));
    text=good;text.replace(text.find("ready"),5,"waiting-cards");check(fresh_renderer_status(text,100,"123",100000));
    text=good;text.replace(text.find("156.1"),5,"156.0");
    check(fresh_renderer_status(text,100,"123",100000,"156.0"));
    check(!fresh_renderer_status(text,100,"123",100000,"156.1"));
    check(!fresh_renderer_status(good,100,"123",100000,"156.0"));
    check(!fresh_renderer_status(text,100,"123",100000,"156.2"));
    text=good;text.replace(text.find("156.1"),5,"157.0");
    check(fresh_renderer_status(text,100,"123",100000,"157.0"));
    check(!fresh_renderer_status(text,100,"123",100000,"156.1"));
    check(!fresh_renderer_status(good,100,"123",100000,"157.0"));
    using namespace seed_preview;
    for(const auto& profile:game_profiles){
        check(profile_for_version(profile.version,profile.versionCode)==&profile);
        check(profile_for_digest(profile.elfSize,profile.elfSha)==&profile);
        check(!profile_for_version(profile.version,profile.versionCode+1));
        check(!profile_for_digest(profile.elfSize+1,profile.elfSha));
        check(!profile_for_digest(profile.elfSize,std::string(64,'0')));
    }
    check(!profile_for_digest(game_profiles[0].elfSize,game_profiles[1].elfSha));
    const std::string boot="12345678-abcd-1234-abcd-123456789abc";
    const std::string record="v2\tBEGIN\t"+boot+"\t100\t123\t10001\t200\t456\tarm64\n"
      "v2\t0\tPREPARED\nv2\t1\tATTACHING\nv2\t2\tATTACHED\nv2\t3\tCALLING\nv2\t4\tRETURNED\nv2\t5\tDETACHED\nv2\t6\tRETURNED_UNVERIFIED\n";
    check(completed_load_record(record,boot,100,"123",10001));
    check(!completed_load_record(record,boot,100,"123",10002));
    check(!completed_load_record(record,boot,101,"123",10001));
    check(!completed_load_record(record,boot,100,"124",10001));
    check(!completed_load_record(record+"v2\tEND\tUNKNOWN\n",boot,100,"123",10001));
    check(!completed_load_record(record.substr(0,record.size()-1),boot,100,"123",10001));
    text=record;text.replace(text.find("\t200\t"),5,"\t1\t");check(!completed_load_record(text,boot,100,"123",10001));
    text=record;text.replace(text.find("arm64"),5,"unknown");check(!completed_load_record(text,boot,100,"123",10001));
    text=record;text.replace(text.find("DETACHED"),8,"UNKNOWN");check(!completed_load_record(text,boot,100,"123",10001));
    close(parent);printf("CONTROL_TRANSACTION_PASS %u; fixtures retained; no game operation\n",checks);
}
