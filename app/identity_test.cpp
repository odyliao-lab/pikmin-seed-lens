#include "identity_policy.h"
#include "payload_policy.h"
#include <string>
#undef NDEBUG
#include <cassert>
int main(){
    struct stat directory{};directory.st_mode=S_IFDIR|0771;directory.st_uid=directory.st_gid=10061;
    assert(seed_root::owned_app_directory(directory,10061));
    directory.st_mode=S_IFDIR|0700;assert(seed_root::owned_app_directory(directory,10061));
    directory.st_mode=S_IFDIR|0777;assert(!seed_root::owned_app_directory(directory,10061));
    directory.st_mode=S_IFDIR|0771;directory.st_gid=10062;assert(!seed_root::owned_app_directory(directory,10061));
    directory.st_gid=10061;directory.st_uid=10062;assert(!seed_root::owned_app_directory(directory,10061));
    directory.st_uid=10061;directory.st_mode=S_IFREG|0700;assert(!seed_root::owned_app_directory(directory,10061));
    directory.st_mode=S_IFLNK|0700;assert(!seed_root::owned_app_directory(directory,10061));
    directory.st_mode=S_IFDIR|0700;directory.st_uid=directory.st_gid=0;assert(!seed_root::owned_app_directory(directory,0));
    using seed_root::game_command;
    std::string main="com.nianticlabs.pikmin";
    assert(!game_command(main));assert(!game_command(""));
    assert(game_command(main+std::string(1,0)));assert(game_command(main+std::string(78,0)));
    assert(!game_command(main+":service"+char(0)));assert(!game_command(main+".fake"+char(0)));
    assert(!game_command(main+char(0)+"other"));assert(!game_command(main+char(0)+" "));
    assert(!game_command("prefix"+main+char(0)));
    using seed_root::installed_payload_path;
    assert(installed_payload_path("/data/app/~~abc=/dev.ody.seedpreview-def==/lib/arm64/libseed_display.so"));
    assert(installed_payload_path("/data/app/dev.ody.seedpreview-123/lib/arm64/libseed_display.so"));
    for(auto path:{"/data/local/tmp/libseed_display.so", "/data/app/dev.ody.seedpreview-/lib/arm64/libseed_display.so",
        "/data/app/dev.ody.seedpreview.fake-x/lib/arm64/libseed_display.so", "/data/app/other-x/lib/arm64/libseed_display.so",
        "/data/app/../dev.ody.seedpreview-x/lib/arm64/libseed_display.so", "/data/app/./dev.ody.seedpreview-x/lib/arm64/libseed_display.so",
        "/data/app//dev.ody.seedpreview-x/lib/arm64/libseed_display.so", "/data/app/dev.ody.seedpreview-x/lib/arm64/libother.so",
        "/data/app/dev.ody.seedpreview-x/lib/arm/libseed_display.so", "/data/app/dev.ody.seedpreview-';id/lib/arm64/libseed_display.so"})
        assert(!installed_payload_path(path));
    assert(!installed_payload_path("/data/app/other/dev.ody.seedpreview-x/lib/arm64/libseed_display.so"));
    assert(!installed_payload_path("/data/app/~~/dev.ody.seedpreview-x/lib/arm64/libseed_display.so"));
    assert(!installed_payload_path("/data/app/~~abc.def/dev.ody.seedpreview-x/lib/arm64/libseed_display.so"));
    assert(!installed_payload_path("/data/app/dev.ody.seedpreview-x.y/lib/arm64/libseed_display.so"));
#if defined(__aarch64__)
    assert(seed_root::installed_loader_path("/data/app/~~abc=/dev.ody.seedpreview-def==/lib/arm64/libseed_loader.so"));
    assert(!seed_root::installed_loader_path("/data/app/~~abc=/dev.ody.seedpreview-def==/lib/x86_64/libseed_loader.so"));
#elif defined(__x86_64__)
    assert(seed_root::installed_loader_path("/data/app/~~abc=/dev.ody.seedpreview-def==/lib/x86_64/libseed_loader.so"));
    assert(!seed_root::installed_loader_path("/data/app/~~abc=/dev.ody.seedpreview-def==/lib/arm64/libseed_loader.so"));
#endif
    assert(!seed_root::installed_loader_path("/data/local/tmp/libseed_loader.so"));
    assert(!seed_root::installed_loader_path("/data/app/dev.ody.seedpreview-x/lib/x86_64/libseed_display.so"));
    assert(seed_root::installed_native_path("/data/app/~~abc=/dev.ody.seedpreview-x/lib/x86_64/libseed_bridge.so","/lib/x86_64/libseed_bridge.so"));
    assert(seed_root::installed_package_path("/data/app/~~abc=/com.nianticlabs.pikmin-x/lib/arm64/libil2cpp.so","/lib/arm64/libil2cpp.so","com.nianticlabs.pikmin"));
    for(auto path:{"/data/app/~~abc=/com.nianticlabs.pikmin.fake-x/lib/arm64/libil2cpp.so",
                  "/data/app/~~abc=/com.nianticlabs.pikmin-/lib/arm64/libil2cpp.so",
                  "/data/app/com.nianticlabs.pikmin-x/lib/x86_64/libil2cpp.so",
                  "/data/local/tmp/com.nianticlabs.pikmin-x/lib/arm64/libil2cpp.so",
                  "/data/app/~~abc=/dev.ody.seedpreview-x/lib/arm64/libil2cpp.so"})
        assert(!seed_root::installed_package_path(path,"/lib/arm64/libil2cpp.so","com.nianticlabs.pikmin"));
}
