#include "api34_tuple.h"
#include <cstdio>
#include <initializer_list>
int main(){
    int n=0;
    for(int api:{31,32,33,34,35,36})
        for(auto sdk:{"31","32","34","034","34 ","+34","35",""})
            for(auto bridge:{"libndk_translation.so","libnb.so","libzygisk.solibndk_translation.so","","0"}){
                bool want=api==34&&std::string_view(sdk)=="34"&&std::string_view(bridge)=="libndk_translation.so";
                if(seed_probe_api34::platform(api,sdk,bridge)!=want)return 1;
                ++n;
            }
    std::printf("API34_RESEARCH_TUPLE_PASS %d host checks; not a device qualification\n",n);
}
