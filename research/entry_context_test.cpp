#include "../app/entry_context.h"
#include <sys/mman.h>
#include <cstdio>
#include <csignal>
int main(){
    alarm(15);auto* counter=static_cast<volatile unsigned long*>(mmap(nullptr,4096,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0));
    if(counter==MAP_FAILED)return 2;*counter=0;
    pid_t child=fork();if(child<0)return 3;
    if(!child){for(;;)*counter=*counter+1;}
    struct Cleanup{pid_t p;~Cleanup(){kill(p,SIGKILL);waitpid(p,nullptr,0);}} cleanup{child};
    usleep(20000);auto start=seed_root::proc_start(child);
    seed_root::StoppedEntry wrong;
    if(wrong.acquire(child,getpid(),start)||wrong.acquire(child,child,start+"0"))return 4;
    for(int i=0;i<12;++i){
        seed_root::StoppedEntry entry;
        if(!entry.acquire(child,child,start))return 5;
        user_regs_struct regs{};if(!entry.user_context(regs))return 6;
        auto before=*counter;usleep(1000);if(*counter!=before)return 7;
        if(!entry.release())return 8;
        usleep(1000);if(*counter==before)return 9;
    }
    puts("ENTRY_CONTEXT_PASS wrong_owner wrong_start 12_user_stops_resume; OWN_CHILD_ONLY");
    return 0;
}
