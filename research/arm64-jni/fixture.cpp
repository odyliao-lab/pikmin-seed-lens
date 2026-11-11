#include <jni.h>
#include <chrono>
#include <android/api-level.h>
#if !defined(__aarch64__)
#error This fixture requires the native ARM64 research backend.
#endif
extern "C" JNIEXPORT jint JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_nativeApi(JNIEnv*,jclass){return android_get_device_api_level();}
extern "C" JNIEXPORT jlong JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_fixture(JNIEnv*,jclass,jlong value){return value^0x53454544LL;}
extern "C" JNIEXPORT void JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_spin(JNIEnv*,jclass){
    // Research target lifetime only, never a loader/remote-call timeout.
    // Keep a runnable userspace interval while slow lab coordination occurs.
    // Batch clock reads; do not fill the sampling window with clock syscalls
    // or a QEMU scheduling hint. The original JNI return is still required.
    auto end=std::chrono::steady_clock::now()+std::chrono::seconds(120);
    do{
        for(unsigned int i=0;i<65536;++i)__asm__ volatile("nop":::"memory");
    }while(std::chrono::steady_clock::now()<end);
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){return JNI_VERSION_1_6;}
