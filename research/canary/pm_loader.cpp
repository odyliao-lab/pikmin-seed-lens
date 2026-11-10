// Self-test only. PM carrier path and exact host SHA are both required before
// any ptrace. Shared durable lease/supervisor preserves UNKNOWN across runs.
#include "pm_backend.h"
#include "identity.h"
#include "../../app/entry_context.h"
#include "../../app/payload_policy.h"
#include "../../native/renderer/seed_preview_sha256.h"
#include <android/api-level.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#ifdef SEED_API34_PM_RESEARCH
#include <sys/system_properties.h>
#include "api34_tuple.h"
#endif
#ifndef SEED_PROBE_HOST_NAME
#define SEED_PROBE_HOST_NAME "libseedprobe_api31_host.so"
#endif
static std::string readFile(const std::string& path){
    std::ifstream file(path);return {std::istreambuf_iterator<char>(file),{}};
}
static bool pmOwned(const std::string& path){
    char resolved[PATH_MAX]{};
    if(!realpath(path.c_str(),resolved)||path!=resolved)return false;
    for(size_t at=1;at<=path.size();++at)if(at==path.size()||path[at]=='/'){
        struct stat st{};auto part=path.substr(0,at);
        if(lstat(part.c_str(),&st)||(st.st_uid!=0&&st.st_uid!=1000)||(st.st_mode&0002)||
           ((st.st_mode&0020)&&st.st_gid!=0&&st.st_gid!=1000)||
           (at==path.size()?!S_ISREG(st.st_mode):!S_ISDIR(st.st_mode)))return false;
    }
    return true;
}
static bool pmFile(const std::string& path){
    return seed_root::installed_package_path(path,"/lib/x86_64/" SEED_PROBE_HOST_NAME,
        "dev.ody.seedlenshostcarrier")&&pmOwned(path);
}
int main(int argc,char** argv){
    setbuf(stdout,nullptr);alarm(30);
    bool timeoutTest=argc==6&&std::string_view(argv[5])=="--timeout-test";
    if(getuid()!=0||(argc!=5&&!timeoutTest))return 2;
#ifdef SEED_PROBE_PM_CLIENT
    char exe[PATH_MAX]{};auto length=readlink("/proc/self/exe",exe,sizeof(exe)-1);
    if(length<=0||length>=static_cast<ssize_t>(sizeof(exe)-1)||
       !seed_root::installed_package_path(exe,"/lib/x86_64/libseed_probe_runner.so",
           "dev.ody.seedlensrootprobe")||!pmOwned(exe))return 14;
#endif
    if(android_get_device_api_level()!=SEED_PROBE_API||std::string_view(argv[3])!=std::to_string(SEED_PROBE_API))return 3;
#ifdef SEED_API34_PM_RESEARCH
    char sdk[PROP_VALUE_MAX]{},bridge[PROP_VALUE_MAX]{};
    __system_property_get("ro.build.version.sdk",sdk);
    __system_property_get("ro.dalvik.vm.native.bridge",bridge);
    if(!seed_probe_api34::platform(android_get_device_api_level(),sdk,bridge))return 3;
#endif
    std::string raw=argv[1],start=argv[2],host=argv[4];
    if(!seed_root::start_ticks(raw)||raw.size()>10||!seed_root::start_ticks(start))return 4;
    auto number=strtoll(raw.c_str(),nullptr,10);if(number<=1||number>INT_MAX)return 4;
    int pid=int(number);struct stat process{},app{};
    if(!canary_command(readFile("/proc/"+raw+"/cmdline"))||seed_root::proc_start(pid)!=start||
       stat(("/proc/"+raw).c_str(),&process)||stat("/data/user/0/dev.ody.seedlensprobe",&app)||
       app.st_uid<10000||app.st_uid>=100000||process.st_uid!=app.st_uid)return 5;
    if(!pmFile(host))return 6;
    int fd=open(host.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    struct Close{int fd;~Close(){if(fd>=0)close(fd);}} close{fd};
    struct stat before{},after{};
    if(fd<0||fstat(fd,&before)||!S_ISREG(before.st_mode)||before.st_size<4096||before.st_size>33554432)return 7;
    seed_preview::Digest digest;std::array<unsigned char,65536> bytes{};
    auto until=std::chrono::steady_clock::now()+std::chrono::seconds(3);
    for(off_t at=0;at<before.st_size;){
        if(std::chrono::steady_clock::now()>until)return 8;
        auto n=pread(fd,bytes.data(),bytes.size(),at);if(n<=0)return 8;
        digest.update(bytes.data(),size_t(n));at+=n;
    }
    if(fstat(fd,&after)||before.st_dev!=after.st_dev||before.st_ino!=after.st_ino||
       before.st_size!=after.st_size||digest.finish()!=SEED_API31_HOST_SHA)return 9;
    auto boot=readFile("/proc/sys/kernel/random/boot_id");if(!boot.empty()&&boot.back()=='\n')boot.pop_back();
    if(!seed_root::boot_id(boot))return 10;
    constexpr auto directory="/data/local/tmp/seed-lens-probe-pm-v1";
    if(mkdir(directory,0700)&&errno!=EEXIST)return 11;
    seed_root::LoadLease lease(directory);
    seed_root::LoadAttempt attempt(lease,boot,pid,start,app.st_uid,getpid(),seed_root::proc_start(getpid()),"nativebridge");
    if(!attempt.ready()){puts("PROBE_PM_ATTEMPT_REFUSED_PRESERVE_PRIOR_RECORD");return 12;}
    struct stat named{};
    if(!pmFile(host)||stat(host.c_str(),&named)||named.st_dev!=before.st_dev||named.st_ino!=before.st_ino||
       named.st_size!=before.st_size)return 13;
    printf("PROBE_PM_ATTEMPT pid=%d start=%s api=%d\n",pid,start.c_str(),SEED_PROBE_API);
    return seed_probe_pm_load(pid,start,host,attempt,timeoutTest);
}
