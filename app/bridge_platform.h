#pragma once
// Platform adapter shared by the production candidate and isolated canary.
// Opaque exported handles only. No private ART layout, guessed namespace or
// force-loading of platform libraries. Missing ABI/API returns a refusal.
#include <jni.h>
#include <cstring>
#include <cstdint>
#include <initializer_list>
#include "xdl.h"
namespace seed_bridge {
class Platform {
    void *art_=nullptr,*loaderLib_=nullptr,*bridgeLib_=nullptr,*ns_=nullptr;
    JavaVM* vm_=nullptr;JNIEnv* env_=nullptr;jobject app_=nullptr;
    bool attached_=false,frame_=false;
    using Load=void*(*)(void*,const char*,bool*,char**);
    using FreeError=void(*)(char*);
    using Trampoline=void*(*)(void*,const char*,const char*,uint32_t,int);
    Load load_=nullptr;FreeError freeError_=nullptr;Trampoline trampoline_=nullptr;
    const char* error_="NOT_OPEN";
    template<class T> static T symbol(void* h,const char* name){return reinterpret_cast<T>(xdl_sym(h,name,nullptr));}
    static void* mapped(const char* apex,const char* runtime,const char* system){
        for(auto path:{apex,runtime,system})if(auto h=xdl_open(path,XDL_DEFAULT))return h;
        return nullptr;
    }
    bool fail(const char* reason){error_=reason;return false;}
    bool exception(){if(!env_||!env_->ExceptionCheck())return false;env_->ExceptionClear();error_="JNI_EXCEPTION";return true;}
    bool stringEquals(jstring s,const char* expected){
        if(exception()||!s||!expected)return false;
        auto chars=env_->GetStringUTFChars(s,nullptr);
        if(exception()||!chars){if(chars)env_->ReleaseStringUTFChars(s,chars);return false;}
        bool same=!strcmp(chars,expected);env_->ReleaseStringUTFChars(s,chars);return same;
    }
public:
    Platform()=default;Platform(const Platform&)=delete;Platform& operator=(const Platform&)=delete;
    ~Platform(){
        if(env_&&frame_)env_->PopLocalFrame(nullptr);
        if(vm_&&attached_)vm_->DetachCurrentThread();
        // XDL_DEFAULT handles only mapped ELF metadata; never dlclose payloads.
        if(bridgeLib_)xdl_close(bridgeLib_);if(loaderLib_)xdl_close(loaderLib_);if(art_)xdl_close(art_);
    }
    const char* error()const{return error_;}
    JavaVM* vm()const{return vm_;}
    JNIEnv* env()const{return env_;}
    jobject application()const{return app_;} // local reference; valid only during this adapter's lifetime
    bool open(const char* expectedPackage){
        if(art_||!expectedPackage)return fail("ALREADY_OPEN_OR_NO_PACKAGE");
        art_=mapped("/apex/com.android.art/lib64/libart.so","/apex/com.android.runtime/lib64/libart.so","/system/lib64/libart.so");
        auto getVm=art_?symbol<jint(*)(JavaVM**,jsize,jsize*)>(art_,"JNI_GetCreatedJavaVMs"):nullptr;jsize count=0;
        if(!getVm||getVm(&vm_,1,&count)!=JNI_OK||count!=1||!vm_)return fail("NO_VM");
        auto status=vm_->GetEnv(reinterpret_cast<void**>(&env_),JNI_VERSION_1_6);
        if(status==JNI_EDETACHED){if(vm_->AttachCurrentThread(&env_,nullptr)!=JNI_OK||!env_)return fail("ATTACH_FAILED");attached_=true;}
        else if(status!=JNI_OK||!env_)return fail("ENV_FAILED");
        if(env_->PushLocalFrame(48)!=JNI_OK){exception();return fail("LOCAL_FRAME_FAILED");}frame_=true;
        auto at=env_->FindClass("android/app/ActivityThread");if(exception()||!at)return fail("NO_ACTIVITY_THREAD");
        auto current=env_->GetStaticMethodID(at,"currentApplication","()Landroid/app/Application;");
        if(exception()||!current)return fail("NO_CURRENT_APPLICATION_METHOD");
        app_=env_->CallStaticObjectMethod(at,current);if(exception()||!app_)return fail("NO_APPLICATION");
        auto context=env_->FindClass("android/content/Context");if(exception()||!context)return fail("NO_CONTEXT");
        auto packageName=env_->GetMethodID(context,"getPackageName","()Ljava/lang/String;");
        if(exception()||!packageName)return fail("NO_PACKAGE_METHOD");
        auto name=static_cast<jstring>(env_->CallObjectMethod(app_,packageName));
        if(exception()||!stringEquals(name,expectedPackage))return fail("WRONG_PACKAGE");
        auto getLoader=env_->GetMethodID(context,"getClassLoader","()Ljava/lang/ClassLoader;");
        if(exception()||!getLoader)return fail("NO_CLASSLOADER_METHOD");
        auto classLoader=env_->CallObjectMethod(app_,getLoader);if(exception()||!classLoader)return fail("NO_CLASSLOADER");
        loaderLib_=mapped("/apex/com.android.art/lib64/libnativeloader.so","/apex/com.android.runtime/lib64/libnativeloader.so","/system/lib64/libnativeloader.so");
        bridgeLib_=mapped("/apex/com.android.art/lib64/libnativebridge.so","/apex/com.android.runtime/lib64/libnativebridge.so","/system/lib64/libnativebridge.so");
        if(!loaderLib_||!bridgeLib_)return fail("PLATFORM_HANDLES_MISSING");
        auto find=symbol<void*(*)(JNIEnv*,jobject)>(loaderLib_,"FindNativeLoaderNamespaceByClassLoader");
        auto bridged=symbol<bool(*)(const void*)>(loaderLib_,"IsNamespaceNativeBridged");
        auto initialized=symbol<bool(*)()>(bridgeLib_,"NativeBridgeInitialized");
        load_=symbol<Load>(loaderLib_,"OpenNativeLibraryInNamespace");
        freeError_=symbol<FreeError>(loaderLib_,"NativeLoaderFreeErrorMessage");
        trampoline_=symbol<Trampoline>(bridgeLib_,"NativeBridgeGetTrampoline2");
        if(!find||!bridged||!initialized||!load_||!freeError_||!trampoline_)return fail("SYMBOL_MISSING");
        if(!initialized())return fail("BRIDGE_NOT_INITIALIZED");
        ns_=find(env_,classLoader);
        if(exception()||!ns_||!bridged(ns_)){ns_=nullptr;return fail("NO_REAL_BRIDGED_NAMESPACE");}
        error_="OPEN";return true;
    }
    bool version(const char* expectedName,jlong expectedCode){
        if(!env_||!app_||!ns_)return fail("NOT_OPEN");
        auto context=env_->FindClass("android/content/Context");if(exception()||!context)return fail("NO_CONTEXT");
        auto getManager=env_->GetMethodID(context,"getPackageManager","()Landroid/content/pm/PackageManager;");
        if(exception()||!getManager)return fail("NO_PACKAGE_MANAGER_METHOD");
        auto getName=env_->GetMethodID(context,"getPackageName","()Ljava/lang/String;");
        if(exception()||!getName)return fail("NO_PACKAGE_NAME_METHOD");
        auto manager=env_->CallObjectMethod(app_,getManager);if(exception()||!manager)return fail("NO_PACKAGE_MANAGER");
        auto name=env_->CallObjectMethod(app_,getName);if(exception()||!name)return fail("NO_PACKAGE_NAME");
        auto cls=env_->GetObjectClass(manager);if(exception()||!cls)return fail("NO_PACKAGE_MANAGER_CLASS");
        auto getInfo=env_->GetMethodID(cls,"getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        if(exception()||!getInfo)return fail("NO_PACKAGE_INFO_METHOD");
        auto info=env_->CallObjectMethod(manager,getInfo,name,0);if(exception()||!info)return fail("NO_PACKAGE_INFO");
        auto infoClass=env_->GetObjectClass(info);if(exception()||!infoClass)return fail("NO_PACKAGE_INFO_CLASS");
        auto versionName=env_->GetFieldID(infoClass,"versionName","Ljava/lang/String;");
        if(exception()||!versionName)return fail("NO_VERSION_NAME_FIELD");
        auto getCode=env_->GetMethodID(infoClass,"getLongVersionCode","()J");
        if(exception()||!getCode)return fail("NO_VERSION_CODE_METHOD");
        auto v=static_cast<jstring>(env_->GetObjectField(info,versionName));
        if(exception()||!stringEquals(v,expectedName))return fail("VERSION_NAME_MISMATCH");
        auto code=env_->CallLongMethod(info,getCode);
        if(exception()||code!=expectedCode)return fail("VERSION_CODE_MISMATCH");
        error_="VERSION_MATCH";return true;
    }
    void* load(const char* path){
        if(!ns_||!path)return nullptr;bool needs=false;char* error=nullptr;
        auto handle=load_(ns_,path,&needs,&error);if(error)freeError_(error);
        if(exception()||!handle||!needs){fail("LOAD_FAILED");return nullptr;}
        error_="LOADED_NOT_INITIALIZED";return handle;
    }
    void* entry(void* handle,const char* name,const char* shorty,uint32_t length){
        if(!ns_||!handle||!name)return nullptr;
        return trampoline_(handle,name,shorty,length,1); // kJNICallTypeRegular
    }
};
}
