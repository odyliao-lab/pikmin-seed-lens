#pragma once
// Separate self-only API34 experiment; never included by product targets.
// Same four-argument opaque namespace interface as the API31 adapter, with
// independent exact API34/SDK/bridge admission before ART lookup. No fallback.
// Android14 source contract: platform/art android-14.0.0_r1,
// libnativebridge/include/nativebridge/native_bridge.h and
// libnativeloader/include/nativeloader/native_loader.h.
// NativeBridgeGetTrampoline has FOUR arguments; no five-argument cast.
#include <android/api-level.h>
#if !defined(__x86_64__)
#error API34 NativeBridge experiment requires x86_64
#endif
#include <jni.h>
#include <cstring>
#include <cstdint>
#include <initializer_list>
#include "xdl.h"
#include <sys/system_properties.h>
#include "api34_tuple.h"
namespace seed_probe_api34 {
class Platform {
    void *art_=nullptr,*loaderLib_=nullptr,*bridgeLib_=nullptr,*ns_=nullptr;
    JavaVM* vm_=nullptr;JNIEnv* env_=nullptr;jobject app_=nullptr;
    bool attached_=false,frame_=false;void* translatedHandle_=nullptr;
    using Load=void*(*)(void*,const char*,bool*,char**);
    using FreeError=void(*)(char*);
    using Trampoline=void*(*)(void*,const char*,const char*,uint32_t);
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
        char sdk[PROP_VALUE_MAX]{},bridge[PROP_VALUE_MAX]{};
        __system_property_get("ro.build.version.sdk",sdk);
        __system_property_get("ro.dalvik.vm.native.bridge",bridge);
        if(!platform(android_get_device_api_level(),sdk,bridge))return fail("API34_EXACT_RESEARCH_TUPLE_REQUIRED");
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
        auto rawNative=symbol<void*(*)(JNIEnv*,jobject)>(loaderLib_,"FindNamespaceByClassLoader");
        auto version=symbol<uint32_t(*)()>(bridgeLib_,"NativeBridgeGetVersion");
        auto initialized=symbol<bool(*)()>(bridgeLib_,"NativeBridgeInitialized");
        load_=symbol<Load>(loaderLib_,"OpenNativeLibraryInNamespace");
        freeError_=symbol<FreeError>(loaderLib_,"NativeLoaderFreeErrorMessage");
        trampoline_=symbol<Trampoline>(bridgeLib_,"NativeBridgeGetTrampoline");
        if(!find||!rawNative||!version||!initialized||!load_||!freeError_||!trampoline_)return fail("API34_SYMBOL_MISSING");
        if(!initialized()||version()<3)return fail("API34_NAMESPACE_BRIDGE_NOT_INITIALIZED");
        ns_=find(env_,classLoader);
        if(exception()||!ns_){ns_=nullptr;return fail("NO_CLASSLOADER_NAMESPACE");}
        auto raw=rawNative(env_,classLoader);
        if(exception()||raw){ns_=nullptr;return fail("RAW_NATIVE_NAMESPACE_REJECTED");}
        // The pair is only a candidate. load() must confirm the bridged flag;
        // no ready/success claim follows merely from these two lookups.
        error_="API34_NAMESPACE_PAIR_NOT_LOADED";return true;
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
        translatedHandle_=handle;error_="API34_BRIDGED_LOAD_NOT_INITIALIZED";return handle;
    }
    void* entry(void* handle,const char* name,const char* shorty,uint32_t length){
        if(!ns_||!handle||!name||handle!=translatedHandle_)return nullptr;
        return trampoline_(handle,name,shorty,length); // API34 four-argument ABI only
    }
};
}
