#pragma once
// Read-only research candidate; not used by either product backend yet.
#include <fcntl.h>
#include <linux/magic.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <cerrno>
#include <string_view>
namespace seed_selinux_research {
enum class State { Unknown, Enforcing, Permissive };
inline State parse(std::string_view text){
    if(text=="1"||text=="1\n")return State::Enforcing;
    if(text=="0"||text=="0\n")return State::Permissive;
    return State::Unknown;
}
inline State readVerifiedFd(int fd){
    struct stat st{};struct statfs fs{};
    if(fd<0||fstat(fd,&st)||!S_ISREG(st.st_mode)||fstatfs(fd,&fs)||
       static_cast<unsigned long>(fs.f_type)!=static_cast<unsigned long>(SELINUX_MAGIC))return State::Unknown;
    char text[4]{};ssize_t n;
    do{n=read(fd,text,sizeof(text));}while(n<0&&errno==EINTR);
    if(n<=0||size_t(n)==sizeof(text))return State::Unknown;
    char extra=0;ssize_t more;
    do{more=read(fd,&extra,1);}while(more<0&&errno==EINTR);
    return more==0?parse(std::string_view(text,size_t(n))):State::Unknown;
}
inline State read(){
    int fd=open("/sys/fs/selinux/enforce",O_RDONLY|O_CLOEXEC|O_NOFOLLOW);
    if(fd<0)return State::Unknown;
    auto result=readVerifiedFd(fd);close(fd);return result;
}
}
