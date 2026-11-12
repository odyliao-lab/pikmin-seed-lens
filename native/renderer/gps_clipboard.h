#pragma once
#include <jni.h>
#include <atomic>
namespace seed_lens {
// The only external effect is an explicitly enabled local clipboard write.
// No logs, files, Android location API, service or third-party package.
inline std::atomic<JavaVM*> clipboard_vm{};
inline bool clipboard_copy(const char *text) {
    auto vm=clipboard_vm.load();if(!vm||!text||!*text)return false;
    JNIEnv *env=nullptr;bool attached=false;
    auto state=vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6);
    if(state==JNI_EDETACHED){if(vm->AttachCurrentThread(&env,nullptr)!=JNI_OK)return false;attached=true;}
    else if(state!=JNI_OK||!env)return false;
    struct Scope{JavaVM* vm;JNIEnv* env;bool attached,frame{};~Scope(){if(env->ExceptionCheck())env->ExceptionClear();if(frame)env->PopLocalFrame(nullptr);if(attached)vm->DetachCurrentThread();}} scope{vm,env,attached};
    if(env->PushLocalFrame(40)!=JNI_OK)return false;scope.frame=true;
    auto clean=[&]{if(!env->ExceptionCheck())return true;env->ExceptionClear();return false;};
    auto thread=env->FindClass("android/app/ActivityThread");if(!clean()||!thread)return false;
    auto current=env->GetStaticMethodID(thread,"currentApplication","()Landroid/app/Application;");if(!clean()||!current)return false;
    auto app=env->CallStaticObjectMethod(thread,current);if(!clean()||!app)return false;
    auto context=env->FindClass("android/content/Context");if(!clean()||!context)return false;
    auto service=env->GetMethodID(context,"getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");if(!clean()||!service)return false;
    auto name=env->NewStringUTF("clipboard");auto manager=env->CallObjectMethod(app,service,name);if(!clean()||!manager)return false;
    auto clipClass=env->FindClass("android/content/ClipData");if(!clean()||!clipClass)return false;
    auto plain=env->GetStaticMethodID(clipClass,"newPlainText","(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;");if(!clean()||!plain)return false;
    auto value=env->NewStringUTF(text),label=env->NewStringUTF("Seed Lens GPS");if(!clean()||!value||!label)return false;
    auto clip=env->CallStaticObjectMethod(clipClass,plain,label,value);if(!clean()||!clip)return false;
    auto managerClass=env->GetObjectClass(manager);
    auto set=managerClass?env->GetMethodID(managerClass,"setPrimaryClip","(Landroid/content/ClipData;)V"):nullptr;if(!clean()||!set)return false;
    env->CallVoidMethod(manager,set,clip);if(!clean())return false;
    auto get=env->GetMethodID(managerClass,"getPrimaryClip","()Landroid/content/ClipData;");if(!clean()||!get)return false;
    auto actual=env->CallObjectMethod(manager,get);if(!clean()||!actual)return false;
    auto count=env->GetMethodID(clipClass,"getItemCount","()I"),item=env->GetMethodID(clipClass,"getItemAt","(I)Landroid/content/ClipData$Item;");if(!clean()||!count||!item)return false;
    if(env->CallIntMethod(actual,count)!=1||!clean())return false;
    auto first=env->CallObjectMethod(actual,item,0);if(!clean()||!first)return false;
    auto itemClass=env->GetObjectClass(first);auto getText=itemClass?env->GetMethodID(itemClass,"getText","()Ljava/lang/CharSequence;"):nullptr;if(!clean()||!getText)return false;
    auto read=env->CallObjectMethod(first,getText);if(!clean()||!read)return false;
    auto strings=env->FindClass("java/lang/String");auto equals=strings?env->GetMethodID(strings,"equals","(Ljava/lang/Object;)Z"):nullptr;if(!clean()||!equals)return false;
    bool copied=env->CallBooleanMethod(value,equals,read);return clean()&&copied;
}
}
