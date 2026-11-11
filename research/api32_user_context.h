#pragma once
#include <sys/user.h>
#include <cstdint>
#ifdef SEED_API32_CONTEXT_RESEARCH
#include <android/api-level.h>
#include <sys/system_properties.h>
#include <cstring>
#include <sys/ptrace.h>
#endif
namespace seed_research {
// API32 MuMu reports multiple negative no-syscall markers on native busy loops.
// Corroborate using the stopped task syscall-info arch/IP/SP. Positive syscalls and
// Linux restart results still refuse. Research flag is absent in product.
inline bool user_context_registers(const user_regs_struct& regs,int tid,bool completedRemoteSyscall=false){
#ifndef SEED_API32_CONTEXT_RESEARCH
    (void)tid;(void)completedRemoteSyscall;
#endif
    if(static_cast<int64_t>(regs.orig_rax)==-1)return true;
#ifdef SEED_API32_CONTEXT_RESEARCH
    char sdk[PROP_VALUE_MAX]{},bridge[PROP_VALUE_MAX]{};
    __system_property_get("ro.build.version.sdk",sdk);
    __system_property_get("ro.dalvik.vm.native.bridge",bridge);
    const auto result=static_cast<int64_t>(regs.rax);
    if(android_get_device_api_level()!=32||strcmp(sdk,"32")||strcmp(bridge,"libnb.so")||
       static_cast<int64_t>(regs.orig_rax)>=0||static_cast<int64_t>(regs.orig_rax)<-511||
       (result<=-512&&result>=-516))return false;
    unsigned char syscallInfo[128]{};
    auto bytes=ptrace(0x420e,tid,sizeof(syscallInfo),syscallInfo);
    uint32_t arch=0;uint64_t ip=0,sp=0;
    memcpy(&arch,syscallInfo+4,sizeof(arch));memcpy(&ip,syscallInfo+8,sizeof(ip));memcpy(&sp,syscallInfo+16,sizeof(sp));
    return bytes>=24&&(syscallInfo[0]==0||(completedRemoteSyscall&&syscallInfo[0]==2))&&arch==0xc000003e&&ip==regs.rip&&sp==regs.rsp;

#else
    return false;
#endif
}
}
