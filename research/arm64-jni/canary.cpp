#include <jni.h>
#include <atomic>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
// Independent self-test only. Never hook or read game state.
static std::atomic<bool> entered{};
static bool self(){
    char value[128]{};int fd=open("/proc/self/cmdline",O_RDONLY|O_CLOEXEC);
    if(fd<0)return false;auto n=read(fd,value,sizeof(value)-1);close(fd);
    return n>0&&!strcmp(value,"dev.ody.seedlensarmprobe");
}
static bool add(int fd,const char* text){return dprintf(fd,"%s\n",text)>0&&!fsync(fd);}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* key){
    if(!self()||!vm||key!=reinterpret_cast<void*>(1337))return JNI_ERR;
    JNIEnv* env=nullptr;
    if(vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6)!=JNI_OK||!env||env->GetVersion()<JNI_VERSION_1_6)return JNI_ERR;
    if(entered.exchange(true))return JNI_ERR;
    char path[180];snprintf(path,sizeof(path),"/data/user/0/dev.ody.seedlensarmprobe/files/native-%d.tsv",getpid());
    int fd=open(path,O_CREAT|O_EXCL|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
    if(fd<0||!add(fd,"ONLOAD_REAL_VM_ENV_PASS")){if(fd>=0)close(fd);return JNI_ERR;}
    try{
        std::thread([vm,fd]{
            JNIEnv* worker=nullptr;
            if(vm->AttachCurrentThread(&worker,nullptr)!=JNI_OK||!worker){add(fd,"FAIL_ATTACH");close(fd);return;}
            bool good=worker->GetVersion()>=JNI_VERSION_1_6&&add(fd,"WORKER_ATTACH_ENV_PASS");
            auto detached=vm->DetachCurrentThread();
            if(good&&detached==JNI_OK)add(fd,"WORKER_DETACH_PASS");else add(fd,"FAIL_WORKER");
            close(fd);
        }).detach();
    }catch(...){add(fd,"FAIL_THREAD");close(fd);return JNI_ERR;}
    return JNI_VERSION_1_6;
}
