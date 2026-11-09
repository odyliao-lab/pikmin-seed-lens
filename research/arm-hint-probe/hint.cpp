#include <jni.h>
#include <android/api-level.h>
#include <signal.h>
#include <unistd.h>
#if !defined(__aarch64__)
#error This probe must execute the ARM64 instructions through the actual App loader.
#endif
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){return JNI_VERSION_1_6;}
extern "C" JNIEXPORT jint JNICALL
Java_dev_ody_seedlensarmhintprobe_HintInstrumentation_runHint(JNIEnv*,jclass,jint mode,jint expected){
    const int api=android_get_device_api_level();
    if(api!=expected||(api!=31&&api!=34&&api!=35)||mode<0||mode>2)return -1;
    // A hang or illegal instruction terminates only this disposable process.
    // Do not catch SIGILL, rewrite instructions or change any bridge/library.
    signal(SIGALRM,SIG_DFL);alarm(3);
    if(mode==1)asm volatile("sevl" ::: "memory");
    else if(mode==2)asm volatile("sevl\n\twfe" ::: "memory");
    alarm(0);return api;
}
