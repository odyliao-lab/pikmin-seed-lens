#include <jni.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <string>
#include "xdl.h"
#include "apk_payload.h"
#ifdef SEED_API34_LAB_CANARY
#include "api34_platform.h"
using CanaryPlatform=seed_probe_api34::Platform;
#elif defined(SEED_API31_CANARY)
#include "api31_platform.h"
using CanaryPlatform=seed_probe_api31::Platform;
#elif defined(SEED_API35_LAB_CANARY)
#include "bridge_dispatch.h"
using CanaryPlatform=seed_bridge::PlatformForRuntime;
#else
#include "bridge_platform.h"
using CanaryPlatform=seed_bridge::Platform;
#endif
#if !defined(__x86_64__)
#error host probe requires x86_64
#endif
namespace {
#if defined(SEED_API31_CANARY) || defined(SEED_API35_LAB_CANARY) || defined(SEED_API34_LAB_CANARY)
constexpr const char* probeVersion=SEED_API31_PROBE_VERSION;
constexpr jlong probeCode=SEED_API31_PROBE_CODE;
#else
constexpr const char* probeVersion="0.0.5";
constexpr jlong probeCode=5;
#endif
bool self() {
    char value[128]{}; int fd=open("/proc/self/cmdline",O_RDONLY|O_CLOEXEC);
    if(fd<0)return false;
    ssize_t n=read(fd,value,sizeof(value)-1);close(fd);
    return n>0&&!strcmp(value,"dev.ody.seedlensprobe");
}
struct Receipt {
    int fd=-1;
    Receipt() {
        char path[160];snprintf(path,sizeof(path),"/data/user/0/dev.ody.seedlensprobe/files/namespace-%d.tsv",getpid());
        fd=open(path,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
    }
    ~Receipt(){if(fd>=0)close(fd);}
    bool add(const char* stage){return fd>=0&&dprintf(fd,"%s\n",stage)>0&&fsync(fd)==0;}
};
void* worker(void*) {
    if(!self())return nullptr;
    Receipt r;if(!r.add("HOST_ENTER"))return nullptr;
    // Use exactly the same adapter as the production candidate, including its
    // package/version refusals; canary success is not game renderer acceptance.
    {CanaryPlatform wrong;
     if(wrong.open("dev.ody.seedlensprobe.wrong")){r.add("WRONG_PACKAGE_ACCEPTED");return nullptr;}
     if(strcmp(wrong.error(),"WRONG_PACKAGE")){r.add(wrong.error());return nullptr;}}
    r.add("WRONG_PACKAGE_REJECTED");
    CanaryPlatform bridge;
    if(!bridge.open("dev.ody.seedlensprobe")){r.add(bridge.error());return nullptr;}
    if(bridge.version("0.0.wrong",probeCode)||strcmp(bridge.error(),"VERSION_NAME_MISMATCH")){
        r.add("WRONG_VERSION_NAME_ACCEPTED");return nullptr;}
    if(bridge.version(probeVersion,probeCode-1)||strcmp(bridge.error(),"VERSION_CODE_MISMATCH")){
        r.add("WRONG_VERSION_CODE_ACCEPTED");return nullptr;}
    if(!bridge.version(probeVersion,probeCode)){r.add(bridge.error());return nullptr;}
    r.add("EXACT_VERSION_AND_NEGATIVE_CONTROLS_PASS");
    auto vm=bridge.vm();auto env=bridge.env();auto app=bridge.application();
#ifdef SEED_FOREIGN_APK
    // Cross-APK namespace control: load the immutable installed Lens renderer
    // in our disposable probe, but NEVER provide the initialization key. This
    // checks ELF/dependencies/namespace/JNI reachability, not gameplay hooks.
    {
        const char* foreign=SEED_FOREIGN_APK;
        int fd=open(foreign,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
        struct Close{int fd;~Close(){if(fd>=0)close(fd);}} closeForeign{fd};
        seed_root::ApkEntry entry;
        if(fd<0||!seed_root::verified_apk_payload(fd,"lib/arm64-v8a/libseed_display.so",
            "2de0117c0802483a65cc761ce7bc7c65f8d0b27865f376954fa68043b126c61a",entry)){
            r.add("FOREIGN_APK_REJECTED");return nullptr;}
        struct stat pinned{},current{};
        if(fstat(fd,&pinned)||stat(foreign,&current)||pinned.st_dev!=current.st_dev||pinned.st_ino!=current.st_ino
            ||pinned.st_size!=current.st_size){r.add("FOREIGN_APK_CHANGED");return nullptr;}
        auto handle=bridge.load((std::string(foreign)+"!/lib/arm64-v8a/libseed_display.so").c_str());
        if(!handle){r.add("FOREIGN_ARM64_LOAD_FAILED");return nullptr;}
        auto init=reinterpret_cast<jint(*)(JavaVM*,void*)>(bridge.entry(handle,"JNI_OnLoad",nullptr,0));
        if(!init||init(vm,nullptr)!=JNI_ERR){r.add("FOREIGN_BAD_KEY_NOT_REJECTED");return nullptr;}
        r.add("FOREIGN_APK_ARM64_ENTRY_REACHABLE_NOT_INITIALIZED");
    }
#endif
    auto failed=[&](){if(env->ExceptionCheck()){env->ExceptionClear();r.add("JNI_EXCEPTION");return true;}return false;};
    auto context=env->FindClass("android/content/Context");if(failed()||!context)return nullptr;
    auto getInfo=env->GetMethodID(context,"getApplicationInfo","()Landroid/content/pm/ApplicationInfo;");
    if(failed()||!getInfo)return nullptr;
    auto info=env->CallObjectMethod(app,getInfo);if(failed()||!info)return nullptr;
    auto infoClass=env->GetObjectClass(info);
    auto libDirField=env->GetFieldID(infoClass,"nativeLibraryDir","Ljava/lang/String;");
    if(failed()||!libDirField)return nullptr;
    auto libDir=static_cast<jstring>(env->GetObjectField(info,libDirField));
    if(failed()||!libDir)return nullptr;
    auto chars=env->GetStringUTFChars(libDir,nullptr);if(!chars)return nullptr;
    std::string path(chars);env->ReleaseStringUTFChars(libDir,chars);
    if(!path.starts_with("/data/app/")||path.find("/dev.ody.seedlensprobe-")==std::string::npos||!path.ends_with("/lib/arm64")){r.add("BAD_PM_PATH");return nullptr;}
    auto sourceField=env->GetFieldID(infoClass,"sourceDir","Ljava/lang/String;");
    if(failed()||!sourceField)return nullptr;
    auto source=static_cast<jstring>(env->GetObjectField(info,sourceField));
    if(failed()||!source)return nullptr;
    chars=env->GetStringUTFChars(source,nullptr);if(!chars)return nullptr;
    std::string apk(chars);env->ReleaseStringUTFChars(source,chars);
    if(apk!=path.substr(0,path.size()-10)+"/base.apk"){r.add("BAD_PM_APK_PATH");return nullptr;}
    int apkFd=open(apk.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    struct Close{int fd;~Close(){if(fd>=0)close(fd);}} closeApk{apkFd};
    seed_root::ApkEntry payload;
    if(apkFd<0||!seed_root::verified_apk_payload(apkFd,"assets/bridge/libcanary.so",SEED_CANARY_SHA,payload)){
        r.add("APK_PAYLOAD_REJECTED");return nullptr;
    }
    r.add("APK_PAYLOAD_SHA_VERIFIED");
    path=apk+"!/assets/bridge/libcanary.so";
    r.add("APK_ZIP_CANARY");
#ifdef SEED_API31_CANARY
    r.add("API31_NAMESPACE_PAIR_NOT_YET_LOADED");
#elif defined(SEED_API34_LAB_CANARY)
    r.add("API34_NAMESPACE_PAIR_NOT_YET_LOADED");
#else
    r.add("REAL_BRIDGED_NAMESPACE");
#endif
    // A real namespace must report a missing self-owned library as a failure;
    // do not substitute a fallback namespace after this negative control.
    std::string absent=path+".does-not-exist";
    if(bridge.load(absent.c_str())){r.add("MISSING_LIBRARY_NOT_REJECTED");return nullptr;}
    r.add("MISSING_LIBRARY_REJECTED");
    struct stat pinned{},currentApk{};
    if(fstat(apkFd,&pinned)||stat(apk.c_str(),&currentApk)||pinned.st_ino!=currentApk.st_ino
       ||pinned.st_dev!=currentApk.st_dev||pinned.st_size!=currentApk.st_size){r.add("APK_REPLACED");return nullptr;}
    void* handle=bridge.load(path.c_str());
    if(!handle){r.add(bridge.error());return nullptr;}
#ifdef SEED_API31_CANARY
    r.add("API31_BRIDGED_NAMESPACE_CONFIRMED_BY_LOAD");
#elif defined(SEED_API34_LAB_CANARY)
    r.add("API34_BRIDGED_NAMESPACE_CONFIRMED_BY_LOAD");
#endif
    r.add("ARM64_HANDLE");
    if(bridge.entry(handle,"SeedLensMissingSymbol","JJ",2)){r.add("MISSING_SYMBOL_NOT_REJECTED");return nullptr;}
    r.add("MISSING_SYMBOL_REJECTED");
    // kJNICallTypeRegular=1, shorty long(long). JNIEnv/jclass are implicit.
    auto fn=reinterpret_cast<jlong(*)(JNIEnv*,jclass,jlong)>(bridge.entry(handle,"SeedLensCanary","JJ",2));
    if(!fn){r.add("TRAMPOLINE_FAILED");return nullptr;}
    for(jlong input:{0LL,1LL,0x123456789LL,-1LL}){
        if(fn(env,context,input)!=(input^static_cast<jlong>(0x43414E4152593634ULL))||failed()){r.add("RETURN_MISMATCH");return nullptr;}
    }
    r.add("CANARY_PASS_4");
    auto onLoad=reinterpret_cast<jint(*)(JavaVM*,void*)>(bridge.entry(handle,"JNI_OnLoad",nullptr,0));
    auto lifecycle=reinterpret_cast<jint(*)(JNIEnv*,jclass)>(bridge.entry(handle,"SeedLensLifecycleState","I",1));
    if(!onLoad||!lifecycle){r.add("LIFECYCLE_SYMBOL_MISSING");return nullptr;}
    r.add("CALLING_TRANSLATED_JNI_ONLOAD");
    if(onLoad(vm,nullptr)!=JNI_ERR||lifecycle(env,context)!=0){r.add("BAD_KEY_NOT_REJECTED");return nullptr;}
    r.add("BAD_KEY_REJECTED");
    if(onLoad(vm,reinterpret_cast<void*>(1337))!=JNI_VERSION_1_6){r.add("JNI_ONLOAD_FAILED");return nullptr;}
    if(onLoad(vm,reinterpret_cast<void*>(1337))!=JNI_VERSION_1_6){r.add("DUPLICATE_ENTRY_FAILED");return nullptr;}
    for(int i=0;i<500;i++){
        auto value=lifecycle(env,context);
        if(value==0x1000f){r.add("JNI_ONLOAD_THREAD_SINGLETON_PASS");return nullptr;}
        if((value&0xff00)||(value>>16)>1){r.add("THREAD_LIFECYCLE_FAILED");return nullptr;}
        usleep(10000);
    }
    r.add("THREAD_LIFECYCLE_TIMED_OUT");
    return nullptr;
}
__attribute__((constructor)) void start(){
    if(!self())return;
    pthread_t thread{};
    if(!pthread_create(&thread,nullptr,worker,nullptr))pthread_detach(thread);
}
}
