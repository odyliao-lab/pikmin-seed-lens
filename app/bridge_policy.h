#pragma once
#include <string_view>
namespace seed_bridge {
enum class Backend { Unsupported, Api35Libnb, Api31NdkTranslationCandidate, Api34NdkTranslationCandidate };
constexpr Backend backend_for(int api,std::string_view bridge,bool api31Candidate,bool api34Candidate=false){
    if(api==35&&bridge=="libnb.so")return Backend::Api35Libnb;
    if(api31Candidate&&api==31&&bridge=="libndk_translation.so")return Backend::Api31NdkTranslationCandidate;
    if(api34Candidate&&api==34&&bridge=="libndk_translation.so")return Backend::Api34NdkTranslationCandidate;
    return Backend::Unsupported;
}
#ifdef SEED_API31_CANDIDATE
inline constexpr bool api31Candidate=true;
#else
inline constexpr bool api31Candidate=false;
#endif
#ifdef SEED_API34_CANDIDATE
inline constexpr bool api34Candidate=true;
#else
inline constexpr bool api34Candidate=false;
#endif
}
