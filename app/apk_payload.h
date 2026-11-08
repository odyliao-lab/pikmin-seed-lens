#pragma once
#include "seed_preview_sha256.h"
#include <cerrno>
#include <string_view>
#include <vector>

namespace seed_root {
// Deliberately narrow ZIP32 reader for a known, uncompressed APK payload.
// No extraction, decompression, ZIP64, split archives or guessed fallback.
// The caller must separately establish the PM path, certificate and ownership.
struct ApkEntry { uint64_t offset=0, size=0; };
namespace apk_detail {
inline uint16_t u16(const unsigned char* p){return uint16_t(p[0])|(uint16_t(p[1])<<8);}
inline uint32_t u32(const unsigned char* p){return uint32_t(u16(p))|(uint32_t(u16(p+2))<<16);}
inline bool range(uint64_t at,uint64_t n,uint64_t end){return at<=end&&n<=end-at;}
inline bool extra(const unsigned char* p,size_t n){
    while(n){if(n<4)return false;auto id=u16(p),len=u16(p+2);if(id==1||len>n-4)return false;p+=4+len;n-=4+len;}
    return true;
}
}
template<class Read> bool find_stored_apk_entry(Read read,uint64_t fileSize,
        std::string_view wanted,ApkEntry& result){
    using namespace apk_detail;
    result={};
    if(fileSize<22||fileSize>512ULL*1024*1024||wanted.empty()||wanted.size()>1024
       ||wanted.front()=='/'||wanted.find("..")!=wanted.npos||wanted.find('!')!=wanted.npos
       ||wanted.find('\0')!=wanted.npos)return false;
    const auto tailSize=static_cast<size_t>(std::min<uint64_t>(fileSize,65557));
    std::vector<unsigned char> tail(tailSize);
    if(!read(fileSize-tailSize,tail.data(),tail.size()))return false;
    size_t eoc=0,candidates=0;
    for(size_t i=0;i+22<=tail.size();++i)
        if(u32(tail.data()+i)==0x06054b50&&i+22+u16(tail.data()+i+20)==tail.size()){eoc=i;++candidates;}
    if(candidates!=1)return false;
    const auto* e=tail.data()+eoc;
    const uint64_t eocAt=fileSize-tailSize+eoc,cdSize=u32(e+12),cdAt=u32(e+16);
    const auto count=u16(e+10);
    if(u16(e+4)||u16(e+6)||!count||count>4096||u16(e+8)!=count
       ||cdSize>4*1024*1024||!range(cdAt,cdSize,eocAt)||cdAt+cdSize!=eocAt)return false;
    std::vector<unsigned char> cd(static_cast<size_t>(cdSize));
    if(!read(cdAt,cd.data(),cd.size()))return false;
    size_t pos=0,matches=0;ApkEntry found{};
    for(unsigned i=0;i<count;++i){
        if(!range(pos,46,cd.size()))return false;
        const auto* c=cd.data()+pos;
        if(u32(c)!=0x02014b50)return false;
        auto nameLen=u16(c+28),extraLen=u16(c+30),commentLen=u16(c+32);
        const uint64_t total=46ULL+nameLen+extraLen+commentLen;
        if(!nameLen||!range(pos,total,cd.size())||u16(c+34)
           ||u32(c+20)==0xffffffff||u32(c+24)==0xffffffff||u32(c+42)==0xffffffff)return false;
        std::string_view name(reinterpret_cast<const char*>(c+46),nameLen);
        if(name==wanted){
            // Older zipalign writes raw padding in unrelated resource entries.
            // Do not interpret their extras; strictly parse only our payload's.
            if(++matches!=1||!extra(c+46+nameLen,extraLen))return false;
            auto flags=u16(c+8);const uint64_t size=u32(c+24),localAt=u32(c+42);
            if((flags&~0x800)||u16(c+10)||!size||size>32*1024*1024||u32(c+20)!=size
               ||!range(localAt,30,cdAt))return false;
            unsigned char local[30]{};
            if(!read(localAt,local,sizeof(local))||u32(local)!=0x04034b50
               ||u16(local+6)!=flags||u16(local+8)||u32(local+14)!=u32(c+16)
               ||u32(local+18)!=size||u32(local+22)!=size||u16(local+26)!=nameLen)return false;
            auto localExtra=u16(local+28);
            uint64_t dataAt=localAt+30+nameLen+localExtra;
            if(!range(dataAt,size,cdAt)||dataAt%16384)return false;
            std::vector<unsigned char> fields(static_cast<size_t>(nameLen)+localExtra);
            if(!read(localAt+30,fields.data(),fields.size())
               ||std::memcmp(fields.data(),wanted.data(),nameLen)
               ||!extra(fields.data()+nameLen,localExtra))return false;
            found={dataAt,size};
        }
        pos+=static_cast<size_t>(total);
    }
    if(pos!=cd.size()||matches!=1)return false;
    result=found;return true;
}
// Keep the FD open through the eventual load and recheck the PM path identity.
// This validates bytes, not a signature or a guarantee against privileged edits.
inline bool verified_apk_payload(int fd,std::string_view entry,std::string_view sha,ApkEntry& out){
    out={};
    if(sha.size()!=64||sha.find_first_not_of("0123456789abcdef")!=sha.npos)return false;
    struct stat before{},after{};
    if(fstat(fd,&before)||!S_ISREG(before.st_mode)||before.st_size<0)return false;
    auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);
    auto read=[&](uint64_t at,void* dest,size_t n){
        if(!apk_detail::range(at,n,static_cast<uint64_t>(before.st_size)))return false;
        auto* bytes=static_cast<unsigned char*>(dest);
        while(n){
            if(std::chrono::steady_clock::now()>deadline)return false;
            auto got=pread(fd,bytes,n,static_cast<off_t>(at));
            if(got<0&&errno==EINTR)continue;
            if(got<=0)return false;
            bytes+=got;at+=got;n-=static_cast<size_t>(got);
        }
        return true;
    };
    ApkEntry found;if(!find_stored_apk_entry(read,before.st_size,entry,found))return false;
    seed_preview::Digest digest;std::array<unsigned char,65536> bytes{};
    for(uint64_t at=0;at<found.size;){
        auto n=static_cast<size_t>(std::min<uint64_t>(bytes.size(),found.size-at));
        if(!read(found.offset+at,bytes.data(),n))return false;
        digest.update(bytes.data(),n);at+=n;
    }
    if(fstat(fd,&after)||before.st_dev!=after.st_dev||before.st_ino!=after.st_ino
       ||before.st_size!=after.st_size||before.st_mtim.tv_sec!=after.st_mtim.tv_sec
       ||before.st_mtim.tv_nsec!=after.st_mtim.tv_nsec||before.st_ctim.tv_sec!=after.st_ctim.tv_sec
       ||before.st_ctim.tv_nsec!=after.st_ctim.tv_nsec||digest.finish()!=sha)return false;
    out=found;return true;
}
}
