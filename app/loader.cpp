#include "Injector/KittyInjector.hpp"
#include "seed_preview_sha256.h"
#include "identity_policy.h"
#include "payload_policy.h"
#include "load_journal.h"
#include "control_transaction.h"
#include <fstream>
#include <sstream>
#include <sys/file.h>
#include <sys/system_properties.h>
#if defined(__x86_64__)
#include "x64_backend.h"
#else
#include "arm64_backend.h"
#endif

static constexpr const char *package="com.nianticlabs.pikmin";
static constexpr const char *base="/data/local/tmp/pcc-seed-root-v1";
static int fail(int stage){printf("BLOCKED_STAGE_%d\n",stage);return stage;}
static std::string read(const std::string &p) {std::ifstream f(p);return {std::istreambuf_iterator<char>(f),{}};}
static std::string ticks(int pid) {
    auto s=read("/proc/"+std::to_string(pid)+"/stat");auto end=s.rfind(')');
    if(end==std::string::npos)return {};
    std::istringstream in(s.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(in>>value))return {};return value;
}
static bool identity(int pid,const std::string &start) {
    auto cmd=read("/proc/"+std::to_string(pid)+"/cmdline");
    struct stat st{},app{};
    return seed_root::game_command(cmd) && !start.empty() && ticks(pid)==start
        && !stat(("/proc/"+std::to_string(pid)).c_str(),&st)
        && !stat("/data/user/0/com.nianticlabs.pikmin",&app)
        && st.st_uid==app.st_uid && app.st_uid>=10000 && app.st_uid<100000;
}
static bool secure(const std::string &path,bool dir=false) {
    struct stat st{};return !lstat(path.c_str(),&st)&&st.st_uid==0&&!(st.st_mode&0022)
        && (dir?S_ISDIR(st.st_mode):S_ISREG(st.st_mode));
}
static bool installed_file(const std::string &path,bool executable=false) {
    bool payloadPath=seed_root::installed_payload_path(path);
#if defined(__x86_64__)
    payloadPath=seed_root::installed_native_path(path,"/lib/x86_64/libseed_bridge.so");
#endif
    if(executable?!seed_root::installed_loader_path(path):!payloadPath)return false;
    char resolved[PATH_MAX]{};
    if(!realpath(path.c_str(),resolved)||path!=resolved)return false;
    // Package-manager files, not shell_data_file. Never relabel or disable SELinux.
    for(size_t i=1;i<=path.size();++i)if(i==path.size()||path[i]=='/'){
        struct stat st{};auto part=path.substr(0,i);
        if(lstat(part.c_str(),&st)||(st.st_uid!=0&&st.st_uid!=1000)||(st.st_mode&0002)
           ||((st.st_mode&0020)&&st.st_gid!=0&&st.st_gid!=1000)
           ||(i==path.size()?!S_ISREG(st.st_mode):!S_ISDIR(st.st_mode)))return false;
    }
    return !executable||access(path.c_str(),X_OK)==0;
}
static const seed_preview::GameProfile* game_profile(int pid,const std::string& start){
    if(!identity(pid,start))return nullptr;
    std::istringstream lines(read("/proc/"+std::to_string(pid)+"/maps"));std::string line,game;
    while(std::getline(lines,line))if(line.ends_with("/libil2cpp.so")){
        auto at=line.find('/');if(at==std::string::npos)return nullptr;
        auto found=line.substr(at);if(!game.empty()&&game!=found)return nullptr;game=found;
    }
    auto profile=game.empty()?nullptr:seed_preview::verified_game_profile(game.c_str());
    return identity(pid,start)?profile:nullptr;
}
static int inject_game(int pid,const std::string& start,const std::string& payload,const seed_root::LoadLease& lease,const seed_preview::GameProfile* expected) {
    if(!lease.held()||!installed_file(payload)||!identity(pid,start))return fail(3);
    auto maps=read("/proc/"+std::to_string(pid)+"/maps");
    for(auto bad:{"libseed_display", "libseed_bridge", "pikmin-control", "pcc_seed_preview", "libseed_preview", "libpikmin_automation"})
        if(maps.find(bad)!=std::string::npos)return 5;
    // Reject incompatible game BEFORE ptrace, then renderer repeats SHA/prologue/metadata checks.
    if(!expected||game_profile(pid,start)!=expected)return fail(6);
    char sdk[PROP_VALUE_MAX]{};__system_property_get("ro.build.version.sdk",sdk);
    int api=atoi(sdk);if(api<28||api>35)return 7;
#if defined(__x86_64__)
    char bridge[PROP_VALUE_MAX]{};__system_property_get("ro.dalvik.vm.native.bridge",bridge);
    if(api!=35||strcmp(bridge,"libnb.so"))return fail(22);
#endif
    // Persistent intent prevents automatic retries after ambiguous injection or app death.
    // Boot ID + start ticks also protects against PID reuse across reboots.
    auto boot=read("/proc/sys/kernel/random/boot_id");
    if(boot.size()==37&&boot.back()=='\n')boot.pop_back();
    struct stat app{};if(!seed_root::boot_id(boot)||stat("/data/user/0/com.nianticlabs.pikmin",&app))return 8;
#if defined(__x86_64__)
    constexpr const char* backend="nativebridge";
#else
    constexpr const char* backend="arm64";
#endif
    seed_root::LoadAttempt attempt(lease,boot,pid,start,app.st_uid,getpid(),ticks(getpid()),backend);
    if(!attempt.ready())return fail(9);
    if(!identity(pid,start))return 11;
#if defined(__x86_64__)
    return seed_x64_load(pid,start,payload,attempt);
#else
    return seed_arm64_load(pid,start,payload,api,attempt);
#endif
}

static bool renderer_status(int pid,const std::string& start,std::string& status,const seed_preview::GameProfile* profile){
    if(!profile)return false;
    status.clear();int fd=open("/data/user/0/com.nianticlabs.pikmin/files/seed_preview_status.tsv",O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
    if(fd<0)return false;struct stat st{},app{};char bytes[513]{};ssize_t n=-1;
    if(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_size>0&&st.st_size<=512&&st.st_nlink==1
       &&!(st.st_mode&0022)&&!stat("/data/user/0/com.nianticlabs.pikmin",&app)&&st.st_uid==app.st_uid)n=::read(fd,bytes,sizeof(bytes));
    close(fd);if(n<=0||n>512)return false;status.assign(bytes,static_cast<size_t>(n));
    auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return identity(pid,start)&&seed_root::fresh_renderer_status(status,pid,start,now,profile->version);
}
static bool completed_attempt(const seed_root::LoadLease& lease,int pid,const std::string& start){
    auto boot=read("/proc/sys/kernel/random/boot_id");if(boot.size()==37&&boot.back()=='\n')boot.pop_back();
    if(!seed_root::boot_id(boot))return false;
    auto name="attempt-"+boot+"-"+std::to_string(pid)+"-"+start;
    int fd=openat(lease.directory(),name.c_str(),O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
    if(fd<0)return false;char bytes[2049]{};ssize_t n=seed_root::owned_file(fd,0)?::read(fd,bytes,sizeof(bytes)):-1;close(fd);
    if(n<=0||n>2048)return false;std::string text(bytes,n);
    // Only our full, durable sequence can authorize re-enabling an existing
    // renderer. Unknown/partial/legacy attempts never become successful here.
    struct stat app{};if(stat("/data/user/0/com.nianticlabs.pikmin",&app))return false;
    return seed_root::completed_load_record(text,boot,pid,start,app.st_uid);
}
int main(int argc,char**argv){
    if(getuid()!=0)return fail(2);
    char exe[PATH_MAX]{};auto length=readlink("/proc/self/exe",exe,sizeof(exe)-1);
    if(length<=0||length>=static_cast<ssize_t>(sizeof(exe)-1)||!installed_file(exe,true))return fail(23);
    if(argc==2&&!strcmp(argv[1],"--self-check")){
#if defined(__aarch64__)
        puts("INSTALLED_LOADER_SELF_CHECK arm64-v8a; EXPLICIT_CONTAINMENT_REQUIRED; NOT GAME_ACCEPTANCE");
#else
        puts("INSTALLED_LOADER_SELF_CHECK x86_64; EXPLICIT_CONTAINMENT_REQUIRED; NOT GAME_ACCEPTANCE");
#endif
        return 0;
    }
    bool contained=argc==5&&!strcmp(argv[1],"--enable-contained");
    bool on=(argc==5&&!strcmp(argv[1],"--enable"))||contained,off=argc==4&&!strcmp(argv[1],"--off"),statusOnly=argc==4&&!strcmp(argv[1],"--status");
    if(!on&&!off&&!statusOnly)return fail(2);
    char* end=nullptr;long n=strtol(argv[2],&end,10);
    if(!end||*end||n<=1||n>INT_MAX||!seed_root::start_ticks(argv[3]))return fail(2);
    int pid=static_cast<int>(n);std::string start=argv[3],status;
    if(!identity(pid,start))return fail(3);
    const auto* profile=(on||statusOnly)?game_profile(pid,start):nullptr;
    if(statusOnly){if(renderer_status(pid,start,status,profile))printf("%s",status.c_str());else puts("NO_FRESH_SAME_PID_START_STATUS");return 0;}
    if(on&&!profile)return fail(6);
    if(on&&!contained){puts("EXPLICIT_FAIL_STOP_CONSENT_REQUIRED");return fail(22);}
    if(mkdir(base,0755)&&errno!=EEXIST)return fail(24);
    if(!secure(base,true))return fail(24);
    seed_root::LoadLease lease(base);if(!lease.held())return fail(4);
    int parent=open("/data/local/tmp",O_RDONLY|O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW);
    struct Close{int fd;~Close(){if(fd>=0)close(fd);}} parentOwner{parent};
    if(parent<0||!identity(pid,start))return fail(3);
    if(!seed_root::commit_control(lease,parent,"pikmin-seed-preview.txt",false,pid,start))return fail(25);
    if(off){puts("OFF_COMMITTED_REQUIRES_UI_RESTORE");return 0;}
    std::string payload=argv[4];if(!installed_file(payload))return fail(3);
    auto maps=read("/proc/"+std::to_string(pid)+"/maps");
    const char* marker=
#if defined(__x86_64__)
        "libseed_bridge.so";
#else
        "libseed_display.so";
#endif
    if(maps.find(marker)==maps.npos){int result=inject_game(pid,start,payload,lease,profile);if(result)return result;}
    else if(maps.find(payload)==maps.npos){puts("OTHER_RENDERER_REQUIRES_GAME_RESTART");return fail(26);}
    if(!completed_attempt(lease,pid,start)){puts("UNKNOWN_ATTEMPT_PRESERVED");return fail(27);}
    bool ready=false;for(int i=0;i<160&&identity(pid,start);++i){if(renderer_status(pid,start,status,profile)){ready=true;break;}usleep(250000);}
    if(!ready){puts("NO_FRESH_RENDERER_LEFT_OFF");return fail(28);}
    if(!identity(pid,start)||!seed_root::commit_control(lease,parent,"pikmin-seed-preview.txt",true,pid,start))return fail(25);
    puts("ON_COMMITTED_REQUIRES_UI_ACCEPTANCE");printf("%s",status.c_str());return 0;
}
