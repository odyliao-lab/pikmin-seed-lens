// Candidate backend bundled by the build; not yet enabled by the UI/loader.
// This x86_64 host uses ART's real application ClassLoader namespace to enter
// the exact ARM64 renderer contained in the same PM-installed APK. No module,
// namespace fabrication, hiding, arbitrary library arguments or game RPCs.
#include "bridge_dispatch.h"
#include "apk_payload.h"
#include "payload_policy.h"
#include "identity_policy.h"
#include <pthread.h>
#include <dlfcn.h>
#include <limits.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#if !defined(__x86_64__) || !defined(SEED_BRIDGE_RENDERER_SHA)
#error Explicit x86_64 build and immutable ARM64 renderer digest required
#endif
namespace {
constexpr const char* game="com.nianticlabs.pikmin";
constexpr const char* files="/data/user/0/com.nianticlabs.pikmin/files";
constexpr const char* hostSuffix="/lib/x86_64/libseed_bridge.so";
std::string boundedRead(const char* path,size_t limit){
    int fd=open(path,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);if(fd<0)return {};
    std::string value(limit+1,'\0');auto n=read(fd,value.data(),value.size());close(fd);
    if(n<=0||static_cast<size_t>(n)>limit)return {};
    value.resize(static_cast<size_t>(n));return value;
}
std::string startTicks(){
    auto value=boundedRead("/proc/self/stat",4096);auto end=value.rfind(')');if(end==value.npos)return {};
    std::istringstream input(value.substr(end+2));std::string v;
    for(int field=3;field<=22;++field)if(!(input>>v))return {};
    return v.empty()||v.find_first_not_of("0123456789")!=v.npos?std::string{}:v;
}
bool self(){
    struct stat app{};auto uid=getuid();
    return uid>=10000&&uid<100000&&geteuid()==uid
        &&seed_root::game_command(boundedRead("/proc/self/cmdline",4096))
        &&!stat("/data/user/0/com.nianticlabs.pikmin",&app)&&S_ISDIR(app.st_mode)&&app.st_uid==uid;
}
bool pmFile(const std::string& path){
    char canonical[PATH_MAX]{};if(!realpath(path.c_str(),canonical)||path!=canonical)return false;
    for(size_t i=1;i<=path.size();++i)if(i==path.size()||path[i]=='/'){
        struct stat st{};auto part=path.substr(0,i);
        if(lstat(part.c_str(),&st)||(st.st_uid!=0&&st.st_uid!=1000)||(st.st_mode&0002)
           ||((st.st_mode&0020)&&st.st_gid!=0&&st.st_gid!=1000)
           ||(i==path.size()?!S_ISREG(st.st_mode):!S_ISDIR(st.st_mode)))return false;
    }
    return true;
}
struct Receipt{
    int fd=-1;
    explicit Receipt(const std::string& ticks){
        int dir=open(files,O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);if(dir<0)return;
        struct stat st{},app{};
        if(fstat(dir,&st)||!seed_root::owned_app_directory(st,getuid())
           ||lstat("/data/user/0/com.nianticlabs.pikmin",&app)||!seed_root::owned_app_directory(app,getuid())
           ||ticks.empty()){close(dir);return;}
        char name[128];snprintf(name,sizeof(name),"seed_bridge_%d_%s.tsv",getpid(),ticks.c_str());
        fd=openat(dir,name,O_WRONLY|O_APPEND|O_CREAT|O_EXCL|O_CLOEXEC|O_NOFOLLOW,0600);
        if(fd>=0&&(fstat(fd,&st)||st.st_uid!=getuid()||!S_ISREG(st.st_mode)||st.st_nlink!=1||fsync(dir))){close(fd);fd=-1;}
        close(dir);
    }
    ~Receipt(){if(fd>=0)close(fd);}
    bool add(const char* stage){return fd>=0&&dprintf(fd,"%s\n",stage)>0&&fsync(fd)==0;}
};
void anchor(){}
std::string ownApk(){
    Dl_info info{};if(!dladdr(reinterpret_cast<void*>(&anchor),&info)||!info.dli_fname)return {};
    std::string host=info.dli_fname;
    if(!seed_root::installed_native_path(host,hostSuffix)||!pmFile(host))return {};
    auto apk=host.substr(0,host.size()-strlen(hostSuffix))+"/base.apk";
    return pmFile(apk)?apk:std::string{};
}
std::string mappedGame(){
    std::ifstream input("/proc/self/maps");std::string line,path;size_t bytes=0;
    while(std::getline(input,line)){
        if(line.size()>4096||(bytes+=line.size()+1)>8*1024*1024)return {};
        if(!line.ends_with("/libil2cpp.so"))continue;
        auto at=line.find('/');if(at==line.npos)return {};
        auto found=line.substr(at);
        if(!seed_root::installed_package_path(found,"/lib/arm64/libil2cpp.so",game)||!pmFile(found))return {};
        if(!path.empty()&&path!=found)return {};
        path=found;
    }return path;
}
void* worker(void*){
    if(!self())return nullptr;
    auto start=startTicks();Receipt receipt(start);if(!receipt.add("HOST_ENTER_NOT_READY"))return nullptr;
    seed_bridge::PlatformForRuntime bridge;
    if(!bridge.open(game)){receipt.add(bridge.error());return nullptr;}
    auto gameFile=mappedGame();
    const auto* profile=gameFile.empty()?nullptr:seed_preview::verified_game_profile(gameFile.c_str());
    if(!profile){receipt.add("GAME_DIGEST_REJECTED");return nullptr;}
    if(!bridge.version(profile->version,profile->versionCode)){receipt.add(bridge.error());return nullptr;}
    if(!receipt.add("EXACT_GAME_VERIFIED"))return nullptr;
    auto apk=ownApk();int fd=apk.empty()?-1:open(apk.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    struct Close{int fd;~Close(){if(fd>=0)close(fd);}} closeApk{fd};
    seed_root::ApkEntry payload;
    if(fd<0||!seed_root::verified_apk_payload(fd,"assets/libseed_display.so",SEED_BRIDGE_RENDERER_SHA,payload)){
        receipt.add("RENDERER_APK_REJECTED");return nullptr;}
    struct stat pinned{},current{};
    if(!self()||startTicks()!=start||!bridge.version(profile->version,profile->versionCode)||!pmFile(apk)
       ||fstat(fd,&pinned)||stat(apk.c_str(),&current)||pinned.st_dev!=current.st_dev
       ||pinned.st_ino!=current.st_ino||pinned.st_size!=current.st_size){receipt.add("IDENTITY_OR_APK_CHANGED");return nullptr;}
    if(!receipt.add("CALLING_ARM64_LOAD_UNKNOWN_UNTIL_RETURN"))return nullptr;
    auto handle=bridge.load((apk+"!/assets/libseed_display.so").c_str());
    if(!handle){receipt.add(bridge.error());return nullptr;}
    auto entry=reinterpret_cast<jint(*)(JavaVM*,void*)>(bridge.entry(handle,"JNI_OnLoad",nullptr,0));
    if(!entry){receipt.add("NO_TRANSLATED_ENTRY");return nullptr;}
    if(!receipt.add("CALLING_ARM64_ENTRY_UNKNOWN_UNTIL_RETURN"))return nullptr;
    if(entry(bridge.vm(),reinterpret_cast<void*>(1337))!=JNI_VERSION_1_6){receipt.add("ARM64_ENTRY_FAILED");return nullptr;}
    // This is not READY. The app must independently observe renderer status,
    // PID/epoch/freshness and actual bound text. Never unload live hooks.
    receipt.add("ARM64_ENTRY_RETURNED_NOT_READY");return nullptr;
}
__attribute__((constructor)) void start(){
    if(!self())return;
    pthread_t thread{};if(!pthread_create(&thread,nullptr,worker,nullptr))pthread_detach(thread);
}
}
