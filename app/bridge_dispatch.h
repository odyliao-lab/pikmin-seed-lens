#pragma once
#include "bridge_runtime.h"
#include "bridge_platform.h"
#ifdef SEED_API31_CANDIDATE
#include "bridge_platform_api31.h"
#endif
#ifdef SEED_API34_CANDIDATE
#include "bridge_platform_api34.h"
#endif
namespace seed_bridge {
// Runtime selection precedes any ART lookup. Never call a different platform
// ABI after failure, and never reinterpret the 4/5-argument trampoline types.
class PlatformForRuntime {
    Backend backend_=Backend::Unsupported;
    Platform api35_;
#ifdef SEED_API31_CANDIDATE
    seed_bridge_api31::Platform api31_;
#endif
#ifdef SEED_API34_CANDIDATE
    seed_bridge_api34::Platform api34_;
#endif
    bool attempted_=false;
    template<class Fn> auto selected(Fn action){
#ifdef SEED_API31_CANDIDATE
        if(backend_==Backend::Api31NdkTranslationCandidate)return action(api31_);
#endif
#ifdef SEED_API34_CANDIDATE
        if(backend_==Backend::Api34NdkTranslationCandidate)return action(api34_);
#endif
        return action(api35_);
    }
public:
    bool open(const char* package){
        if(attempted_)return false;
        attempted_=true;backend_=runtime_backend();
        return backend_!=Backend::Unsupported&&selected([&](auto& p){return p.open(package);});
    }
    const char* error(){return backend_==Backend::Unsupported?"UNQUALIFIED_PLATFORM_TUPLE":selected([](auto& p){return p.error();});}
    JavaVM* vm(){return selected([](auto& p){return p.vm();});}
    JNIEnv* env(){return selected([](auto& p){return p.env();});}
    jobject application(){return selected([](auto& p){return p.application();});}
    bool version(const char* name,jlong code){return backend_!=Backend::Unsupported&&selected([&](auto& p){return p.version(name,code);});}
    void* load(const char* path){return backend_==Backend::Unsupported?nullptr:selected([&](auto& p){return p.load(path);});}
    void* entry(void* handle,const char* name,const char* shorty,uint32_t length){
        return backend_==Backend::Unsupported?nullptr:selected([&](auto& p){return p.entry(handle,name,shorty,length);});
    }
};
}
