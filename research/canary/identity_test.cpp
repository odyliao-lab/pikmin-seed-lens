#include "identity.h"
#include <cstdio>
int main(){
    const std::string p="dev.ody.seedlensprobe";
    if(!canary_command(p+'\0')||!canary_command(p+std::string(78,'\0'))
       ||canary_command(p)||canary_command(p+"2\0")||canary_command("com.nianticlabs.pikmin\0")
       ||canary_command(p+'\0'+"argument")||canary_command(p+":other\0"))return 1;
    puts("CANARY_IDENTITY_7_PASS");return 0;
}
