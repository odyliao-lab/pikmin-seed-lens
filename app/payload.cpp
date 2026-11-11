#include <jni.h>
#include <atomic>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include "seed_preview.h"
#include "display_contract.h"
#include "gps_clipboard.h"
// No constructors, game RPC, JNI object writes or module installation.
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *key) {
    static std::atomic<bool> entered{};
    if (!vm || key != reinterpret_cast<void*>(1337)) return JNI_ERR;
    if (entered.exchange(true)) return JNI_VERSION_1_6;
    // Held until game exits; never dlclose a payload with live hooks.
    int fd=open(seed_lens::payload_lock,O_CREAT|O_RDWR|O_CLOEXEC|O_NOFOLLOW,0600);
    if(fd<0 || flock(fd,LOCK_EX|LOCK_NB)) { if(fd>=0)close(fd);return JNI_ERR; }
    seed_lens::clipboard_vm.store(vm);
    seed_preview::start("/data/user/0/com.nianticlabs.pikmin");
    return JNI_VERSION_1_6;
}
