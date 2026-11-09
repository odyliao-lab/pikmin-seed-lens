#include "bridge_policy.h"
#include <cstdio>
int main(){
    using namespace seed_bridge;int checks=0;
    constexpr auto no=Backend::Unsupported,yes35=Backend::Api35Libnb,yes31=Backend::Api31NdkTranslationCandidate;
    struct Case{int api;std::string_view bridge;Backend normal,candidate;};
    // Explicit qualification boundaries, independently specified for both builds.
    const Case cases[]={
        {35,"libnb.so",yes35,yes35},{31,"libndk_translation.so",no,yes31},
        {31,"libnb.so",no,no},{35,"libndk_translation.so",no,no},
        {32,"libnb.so",no,no},{32,"libndk_translation.so",no,no},
        {34,"libnb.so",no,no},{30,"libndk_translation.so",no,no},
        {36,"libnb.so",no,no},{0,"libndk_translation.so",no,no},
        {31,"",no,no},{35,"",no,no},{31,"libnativebridge.so",no,no},
        {35,"libnb.so.wrong",no,no},{31,"libndk_translation.so.wrong",no,no},
        {31,"Libndk_translation.so",no,no},{35," libnb.so",no,no}
    };
    for(const auto& c:cases){
        if(backend_for(c.api,c.bridge,false)!=c.normal||backend_for(c.api,c.bridge,true)!=c.candidate)return 1;
        checks+=2;
        if(backend_for(c.api,c.bridge,api31Candidate)!=(api31Candidate?c.candidate:c.normal))return 2;++checks;
    }
    printf("BRIDGE_POLICY_PASS %d; candidate=%d; NOT_DEVICE_QUALIFICATION\n",checks,api31Candidate);return 0;
}
