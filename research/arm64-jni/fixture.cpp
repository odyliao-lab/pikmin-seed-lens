#include <jni.h>
#include <chrono>
#include <android/api-level.h>
#if !defined(__aarch64__)
#error This fixture requires the native ARM64 research backend.
#endif
extern "C" JNIEXPORT jint JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_nativeApi(JNIEnv*,jclass){return android_get_device_api_level();}
extern "C" JNIEXPORT jlong JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_fixture(JNIEnv*,jclass,jlong value){return value^0x53454544LL;}
extern "C" JNIEXPORT void JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_spin(JNIEnv*,jclass){
    auto end=std::chrono::steady_clock::now()+std::chrono::seconds(35);
    while(std::chrono::steady_clock::now()<end)__asm__ volatile("yield":::"memory");
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){return JNI_VERSION_1_6;}
