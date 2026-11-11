#pragma once
#include "game_profiles.h"
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
namespace seed_preview {
// Same SHA-256 digest implementation as the sealed MuMu payload verifier.
class Digest {
    std::array<uint32_t,8> state_{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
                                  0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    std::array<unsigned char,64> pending_{};size_t used_{};uint64_t bytes_{};
    void block(const unsigned char *p){
        static constexpr uint32_t k[]{
            0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
            0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
            0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
            0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
            0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
            0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
            0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
            0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
        uint32_t w[64]{};
        for(unsigned i=0;i<16;++i)w[i]=(uint32_t(p[4*i])<<24)|(uint32_t(p[4*i+1])<<16)|(uint32_t(p[4*i+2])<<8)|p[4*i+3];
        for(unsigned i=16;i<64;++i){
            const auto x=w[i-15],y=w[i-2];
            w[i]=w[i-16]+(std::rotr(x,7)^std::rotr(x,18)^(x>>3))+w[i-7]+(std::rotr(y,17)^std::rotr(y,19)^(y>>10));
        }
        auto a=state_[0],b=state_[1],c=state_[2],d=state_[3],e=state_[4],f=state_[5],g=state_[6],h=state_[7];
        for(unsigned i=0;i<64;++i){
            const auto t1=h+(std::rotr(e,6)^std::rotr(e,11)^std::rotr(e,25))+((e&f)^(~e&g))+k[i]+w[i];
            const auto t2=(std::rotr(a,2)^std::rotr(a,13)^std::rotr(a,22))+((a&b)^(a&c)^(b&c));
            h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
        }
        state_[0]+=a;state_[1]+=b;state_[2]+=c;state_[3]+=d;state_[4]+=e;state_[5]+=f;state_[6]+=g;state_[7]+=h;
    }
public:
    void update(const unsigned char *p,size_t count){
        bytes_+=count;
        while(count){const auto n=std::min(count,64-used_);std::memcpy(pending_.data()+used_,p,n);used_+=n;p+=n;count-=n;
            if(used_==64){block(pending_.data());used_=0;}}
    }
    std::string finish(){
        const auto bits=bytes_*8;pending_[used_++]=0x80;
        if(used_>56){std::memset(pending_.data()+used_,0,64-used_);block(pending_.data());used_=0;}
        std::memset(pending_.data()+used_,0,56-used_);
        for(unsigned i=0;i<8;++i)pending_[63-i]=static_cast<unsigned char>(bits>>(8*i));
        block(pending_.data());constexpr char hex[]="0123456789abcdef";std::string out(64,'0');
        for(unsigned i=0;i<32;++i){const auto byte=static_cast<unsigned char>(state_[i/4]>>(24-8*(i%4)));out[2*i]=hex[byte>>4];out[2*i+1]=hex[byte&15];}
        return out;
    }
};
inline const GameProfile* verified_game_profile(const char *path) {
    int fd=open(path,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);if(fd<0)return nullptr;
    struct stat before{},after{};bool ok=!fstat(fd,&before)&&S_ISREG(before.st_mode)&&known_game_size(before.st_size);
    Digest digest;std::array<unsigned char,65536> bytes{};off_t at=0;
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(30);
    while(ok&&at<before.st_size){
        if(std::chrono::steady_clock::now()>deadline){ok=false;break;}
        auto n=pread(fd,bytes.data(),bytes.size(),at);if(n<=0){ok=false;break;}
        digest.update(bytes.data(),static_cast<size_t>(n));at+=n;
    }
    ok=ok&&!fstat(fd,&after)&&before.st_ino==after.st_ino&&before.st_dev==after.st_dev&&before.st_size==after.st_size;
    close(fd);return ok?profile_for_digest(before.st_size,digest.finish()):nullptr;
}
inline bool verified_game_digest(const char *path){return verified_game_profile(path)!=nullptr;}
}
