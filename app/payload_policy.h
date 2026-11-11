#pragma once
#include <string_view>
#include <sys/stat.h>
namespace seed_root {
inline bool owned_app_directory(const struct stat& st,uid_t uid){
    // Android files/ commonly uses 0771 with the application's own private GID.
    // That is not a foreign writer; world-write or a different writable GID is.
    return uid>=10000&&uid<100000&&S_ISDIR(st.st_mode)&&st.st_uid==uid
        &&!(st.st_mode&0002)&&(!(st.st_mode&0020)||st.st_gid==uid);
}
inline bool install_token(std::string_view value){
    if(value.empty())return false;
    for(char c:value)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='='||c=='~'||c=='+'))return false;
    return true;
}
inline bool installed_package_path(std::string_view path,std::string_view suffix,std::string_view package) {
    constexpr std::string_view prefix="/data/app/";
    if(!path.starts_with(prefix)||!path.ends_with(suffix))return false;
    for(char c:path)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='/'||c=='.'||c=='_'||c=='-'||c=='='||c=='~'||c=='+'))return false;
    if(path.find("/../")!=path.npos||path.find("/./")!=path.npos||path.find("//")!=path.npos)return false;
    auto parent=path.substr(0,path.size()-suffix.size());
    auto component=parent.substr(parent.rfind('/')+1);
    if(package.empty()||component.size()<=package.size()+1||!component.starts_with(package)
       ||component[package.size()]!='-'||!install_token(component.substr(package.size()+1)))return false;
    auto ancestor=parent.substr(prefix.size(),parent.size()-prefix.size()-component.size());
    return ancestor.empty()||(ancestor.starts_with("~~")&&ancestor.size()>3&&ancestor.back()=='/'&&install_token(ancestor.substr(2,ancestor.size()-3)));
}
inline bool installed_native_path(std::string_view path,std::string_view suffix){return installed_package_path(path,suffix,"dev.ody.seedpreview");}
inline bool installed_payload_path(std::string_view path){return installed_native_path(path,"/lib/arm64/libseed_display.so");}
inline bool installed_loader_path(std::string_view path){
#if defined(__aarch64__)
    return installed_native_path(path,"/lib/arm64/libseed_loader.so");
#elif defined(__x86_64__)
    return installed_native_path(path,"/lib/x86_64/libseed_loader.so");
#else
    return false;
#endif
}
}
