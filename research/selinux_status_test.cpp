#include "selinux_status.h"
#include <cstdio>
#include <cstdlib>
#include <utility>
int main(int argc,char**){
    if(argc!=1)return 2;
    using namespace seed_selinux_research;unsigned checks=0;
    for(auto pair:{std::pair{"1",State::Enforcing},std::pair{"1\n",State::Enforcing},
                   std::pair{"0",State::Permissive},std::pair{"0\n",State::Permissive}}){
        if(parse(pair.first)!=pair.second)return 3;++checks;
    }
    for(auto value:{""," ","01","10","-1","2","true","1\n\n","1 ","Enforcing"}){
        if(parse(value)!=State::Unknown)return 4;++checks;
    }
    if(parse(std::string_view("1\0",2))!=State::Unknown||readVerifiedFd(-1)!=State::Unknown)return 5;
    checks+=2;
    char path[]="/data/local/tmp/seed-selinux-status-XXXXXX";
    int fd=mkstemp(path);if(fd<0)return 6;
    bool reject=write(fd,"1",1)==1&&lseek(fd,0,SEEK_SET)==0&&readVerifiedFd(fd)==State::Unknown;
    close(fd); // Keep the own fixture for audit; never touch SELinux configuration.
    if(!reject)return 7;++checks;
    auto actual=seed_selinux_research::read();if(actual!=State::Enforcing)return 8;
    printf("SELINUX_READ_ONLY_RESEARCH_PASS %u negative/parser checks; actual=Enforcing; NOT App/game\n",checks);
    return 0;
}
