#include "../../native/renderer/gps_clipboard.h"
#include <thread>
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm,void*){seed_lens::clipboard_vm.store(vm);return JNI_VERSION_1_6;}
extern "C" JNIEXPORT jint JNICALL Java_dev_ody_seedlensclipprobe_ProbeActivity_run(JNIEnv*,jclass,jint mode){
 if(mode==0)return 0; // OFF never touches the clipboard.
 if(mode==2)return seed_lens::clipboard_copy(nullptr)?-1:2;
 if(mode==3){bool ok=false;std::thread worker([&]{ok=seed_lens::clipboard_copy("seed-lens-isolated-worker-probe");});worker.join();return ok?3:-3;}
 return seed_lens::clipboard_copy("seed-lens-isolated-clipboard-probe")?1:-1;
}
