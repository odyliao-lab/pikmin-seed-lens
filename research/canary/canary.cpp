#include <jni.h>
#include <atomic>
#include <thread>
#if !defined(__aarch64__)
#error canary must be ARM64
#endif
// No constructors, IO, game references, hooks or network. JNI_OnLoad alone
// starts one bounded worker to exercise the real renderer's thread lifecycle.
namespace {
std::atomic<bool> entered{};
std::atomic<unsigned> state{};
std::atomic<unsigned> workers{};
}
extern "C" JNIEXPORT jlong JNICALL SeedLensCanary(JNIEnv*, jclass, jlong input) {
    return input ^ static_cast<jlong>(0x43414E4152593634ULL);
}
extern "C" JNIEXPORT jint JNICALL SeedLensLifecycleState(JNIEnv*,jclass) {
    return static_cast<jint>(state.load()|(workers.load()<<16));
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* key) {
    if(!vm||key!=reinterpret_cast<void*>(1337))return JNI_ERR;
    JNIEnv* env=nullptr;
    if(vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6)!=JNI_OK||!env
       ||env->GetVersion()<JNI_VERSION_1_6)return JNI_ERR;
    if(entered.exchange(true))return JNI_VERSION_1_6;
    state.store(3); // entered + host thread's translated JNI environment works
    try {
        std::thread([vm]{
            workers.fetch_add(1);
            JNIEnv* attached=nullptr;
            if(vm->AttachCurrentThread(&attached,nullptr)!=JNI_OK||!attached){state.fetch_or(0x100);return;}
            if(attached->GetVersion()>=JNI_VERSION_1_6)state.fetch_or(4);
            if(vm->DetachCurrentThread()==JNI_OK)state.fetch_or(8);
        }).detach();
    } catch(...) { state.fetch_or(0x200);return JNI_ERR; }
    return JNI_VERSION_1_6;
}
