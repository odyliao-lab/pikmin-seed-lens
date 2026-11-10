#include "../../app/arm64_backend.h"
#include "../../app/payload_policy.h"
#include "../../native/renderer/seed_preview_sha256.h"
#include <fstream>
#include <sstream>
#include <climits>
#include <sys/stat.h>
#include <android/api-level.h>
#include "../probe_platform.h"
static std::string readFile(const std::string& path){std::ifstream f(path);return {std::istreambuf_iterator<char>(f),{}};}
static std::string ticks(int pid){
    auto text=readFile("/proc/"+std::to_string(pid)+"/stat");auto end=text.rfind(')');if(end==text.npos)return {};
    std::istringstream in(text.substr(end+2));std::string word;for(int i=3;i<=22;++i)if(!(in>>word))return {};return word;
}
int main(int argc,char** argv){
    setbuf(stdout,nullptr);alarm(25);
    if(argc!=5||getuid()!=0)return 2;
    const int api=android_get_device_api_level();
    if(!seed_probe::admit(argv[4],api,seed_probe::Abi::Arm64)){
        puts("PROBE_API_MISMATCH_OR_UNREVIEWED_NO_ATTACH");return 11;
    }
    printf("RESEARCH_ONLY_ACTUAL_API=%d ABI=arm64-v8a NOT_PRODUCT_QUALIFICATION\n",api);
    std::string raw=argv[1];if(!seed_root::start_ticks(raw)||raw.size()>10)return 3;
    auto value=strtoll(raw.c_str(),nullptr,10);if(value<=1||value>INT_MAX)return 3;int pid=int(value);
    std::string start=argv[2],payload=argv[3],name="dev.ody.seedlensarmprobe";
    auto cmd=readFile("/proc/"+raw+"/cmdline");struct stat process{},app{};
    if(!seed_root::start_ticks(start)||ticks(pid)!=start||cmd.size()<=name.size()||!cmd.starts_with(name)||
       cmd.find_first_not_of('\0',name.size())!=cmd.npos||stat(("/proc/"+raw).c_str(),&process)||
       stat(("/data/user/0/"+name).c_str(),&app)||app.st_uid<10000||app.st_uid>=100000||app.st_uid!=process.st_uid)return 4;
    if(!seed_root::installed_package_path(payload,"/lib/arm64/libcanary.so",name))return 5;
    // Match production PM policy: Android system/root ownership is trusted,
    // foreign write permissions and symlinks are not. Exact canary digest pinned.
    char resolved[PATH_MAX]{};
    if(!realpath(payload.c_str(),resolved)||payload!=resolved)return 6;
    for(size_t at=1;at<=payload.size();++at)if(at==payload.size()||payload[at]=='/'){
        struct stat st{};auto part=payload.substr(0,at);
        if(lstat(part.c_str(),&st)||(st.st_uid!=0&&st.st_uid!=1000)||(st.st_mode&0002)||
           ((st.st_mode&0020)&&st.st_gid!=0&&st.st_gid!=1000)||
           (at==payload.size()?!S_ISREG(st.st_mode):!S_ISDIR(st.st_mode)))return 6;
    }
    int file=open(payload.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW);struct stat st{};
    if(file<0||fstat(file,&st)||!S_ISREG(st.st_mode)||st.st_size<4096||st.st_size>33554432)return 7;
    seed_preview::Digest digest;std::array<unsigned char,65536> bytes{};
    for(off_t offset=0;offset<st.st_size;){auto n=pread(file,bytes.data(),bytes.size(),offset);if(n<=0)return 8;
        digest.update(bytes.data(),size_t(n));offset+=n;}
    if(digest.finish()!=SEED_ARM64_PROBE_SHA)return 9;
    auto boot=readFile("/proc/sys/kernel/random/boot_id");if(!boot.empty()&&boot.back()=='\n')boot.pop_back();
    char directory[]="/data/local/tmp/seed-arm64-jni-attempt-XXXXXX";if(!mkdtemp(directory))return 10;
    seed_root::LoadLease lease(directory,0);
    seed_root::LoadAttempt attempt(lease,boot,pid,start,app.st_uid,getpid(),ticks(getpid()),"arm64");
    printf("PROBE_ATTEMPT=%s\n",directory);
    auto result=seed_arm64_load(pid,start,payload,api,attempt);close(file);
    if(!result)puts("ARM64_JNI_PROBE_RETURNED_REQUIRES_APP_RECEIPT");return result;
}
