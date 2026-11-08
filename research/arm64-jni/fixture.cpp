#include <jni.h>
#include <chrono>
extern "C" JNIEXPORT jlong JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_fixture(JNIEnv*,jclass,jlong value){return value^0x53454544LL;}
extern "C" JNIEXPORT void JNICALL Java_dev_ody_seedlensarmprobe_ProbeInstrumentation_spin(JNIEnv*,jclass){
    auto end=std::chrono::steady_clock::now()+std::chrono::seconds(35);
    while(std::chrono::steady_clock::now()<end)__asm__ volatile("yield":::"memory");
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){return JNI_VERSION_1_6;}
