#include <jni.h>
static_assert(sizeof(void*) == 8);
#if !defined(__aarch64__)
#error fixture must be ARM64, not the host ABI
#endif
extern "C" JNIEXPORT jlong JNICALL
Java_dev_ody_seedlensprobe_ProbeInstrumentation_fixture(JNIEnv*, jclass, jlong input) {
    return input ^ static_cast<jlong>(0x534545444C454E53ULL);
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) { return JNI_VERSION_1_6; }
