#include "Injector/KittyInjector.hpp"
#include "seed_preview_sha256.h"
#include "identity_policy.h"
#include <fstream>
#include <sstream>
#include <sys/file.h>
#include <sys/system_properties.h>

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
int main(int argc,char **argv) {
    if(getuid()!=0||argc!=3)return fail(2);
#if defined(__x86_64__)
    // Virgo 2026-11-08: no exported namespace; remote syscall path also waited
    // without its configured timeout. Keep this backend OFF until isolated fixes
    // and a verified real namespace are available. Never guess namespace pointer 1.
    puts("NATIVEBRIDGE_BACKEND_NOT_QUALIFIED");return fail(22);
#endif
    char *end=nullptr;long n=strtol(argv[1],&end,10);
    if(!end||*end||n<=1||n>INT_MAX)return 2;int pid=static_cast<int>(n);
    std::string start=argv[2],dir=base,payload=dir+"/libseed_display.so";
    if(!secure(dir,true)||!secure(payload)||!identity(pid,start))return fail(3);
    int lock=open((dir+"/load.lock").c_str(),O_CREAT|O_RDWR|O_CLOEXEC|O_NOFOLLOW,0600);
    if(lock<0||flock(lock,LOCK_EX|LOCK_NB))return 4;
    auto maps=read("/proc/"+std::to_string(pid)+"/maps");
    for(auto bad:{"libseed_display", "pikmin-control", "pcc_seed_preview", "libseed_preview", "libpikmin_automation"})
        if(maps.find(bad)!=std::string::npos)return 5;
    // Reject incompatible game BEFORE ptrace, then renderer repeats SHA/prologue/metadata checks.
    std::istringstream lines(maps);std::string line,game;
    while(std::getline(lines,line))if(line.ends_with("/libil2cpp.so")) {
        auto at=line.find('/');if(at!=std::string::npos)game=line.substr(at);
    }
    if(game.empty()||!seed_preview::verified_game_digest(game.c_str()))return fail(6);
    char sdk[PROP_VALUE_MAX]{};__system_property_get("ro.build.version.sdk",sdk);
    int api=atoi(sdk);if(api<28||api>35)return 7;
    // Persistent intent prevents automatic retries after ambiguous injection or app death.
    // Boot ID + start ticks also protects against PID reuse across reboots.
    auto boot=read("/proc/sys/kernel/random/boot_id");
    if(boot.size()<36)return 8;boot.resize(36);
    auto intent=dir+"/attempt-"+boot+"-"+std::to_string(pid)+"-"+start;
    int attempt=open(intent.c_str(),O_CREAT|O_EXCL|O_WRONLY|O_CLOEXEC|O_NOFOLLOW,0600);
    if(attempt<0)return fail(9);
    if(write(attempt,"attempted\n",10)!=10||fsync(attempt)){close(attempt);return 10;}close(attempt);
    if(!identity(pid,start))return 11;
    inject_elf_config_t cfg{};cfg.sdk=api;cfg.timeout=5000;
    cfg.selinux_state=Utils::selinux_state();cfg.package=package;
    // No memfd/hiding, namespace/SELinux policy changes, launch, watch or unload flags.
    KittyMemoryMgr mgr{};mgr.trace=KittyTraceMgr(pid,0,true,5000);
    cfg.seize=mgr.trace.seize(PTRACE_O_TRACESYSGOOD);
    if(!cfg.seize||!mgr.trace.stop()){mgr.trace.detach();return 12;}
    struct Detach {KittyTraceMgr &trace;~Detach(){trace.detach();}} detach{mgr.trace};
    if(!identity(pid,start)||!mgr.initialize(pid,EK_MEM_OP_SYSCALL,true))return 13;
    mgr.trace.setRemoteCallTimeout(5000);
    KittyInjector injector;
    if(!injector.init(&mgr,cfg))return 14;
#if defined(__x86_64__)
    // Upstream falls back to a guessed namespace pointer when exports are absent.
    // Never use that fallback in the game: require a real exported namespace.
    mgr.nbScanner.init();
    auto nb=mgr.nbScanner.nbItfData();
    if(nb.version<3||!nb.loadLibraryExt||!nb.getExportedNamespace)return 17;
    auto scratch=injector._rsyscall.rmmap(0,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(!scratch)return fail(18);
    struct Scratch {KittyRemoteSys &sys;uintptr_t p;~Scratch(){if(p)sys.rmunmap(p,4096);}} scratch_owner{injector._rsyscall,scratch};
    bool namespace_found=false;
    for(auto name:{"default","classloader-namespace","classloader-namespace-shared"}) {
        if(!mgr.writeMemStr(scratch,name))return fail(18);
        auto ns=mgr.trace.callFunction(reinterpret_cast<uintptr_t>(nb.getExportedNamespace),scratch);
        if(ns.status!=KT_RP_CALL_SUCCESS)return 19;
        if(ns.result.ptr){namespace_found=true;break;}
    }
    if(!namespace_found){puts("NO_EXPORTED_NATIVEBRIDGE_NAMESPACE");return 20;}
    if(!injector._rsyscall.rmunmap(scratch,4096))return 21;
    scratch_owner.p=0;
#endif
    auto result=injector.inject(payload);
    // dlopen != renderer ready. UI must read new same-PID status after detach.
    if(!result.is_valid()||!result.pJNI_OnLoad||!result.pJvm)return 15;
    if(!mgr.trace.detach())return 16;
    puts("LOAD_RETURNED_REQUIRES_STATUS");return 0;
}
