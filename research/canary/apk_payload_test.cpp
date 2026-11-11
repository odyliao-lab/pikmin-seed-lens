#include "apk_payload.h"
#include <cstdio>
#include <cstdlib>
#undef NDEBUG
#include <cassert>
using Bytes=std::vector<unsigned char>;
static constexpr std::string_view name="assets/bridge/libcanary.so";
static void w16(Bytes& b,size_t p,uint16_t x){b[p]=x;b[p+1]=x>>8;}
static void w32(Bytes& b,size_t p,uint32_t x){w16(b,p,x);w16(b,p+2,x>>16);}
static Bytes valid(){
    constexpr size_t data=16384,size=3;const size_t cd=data+size,cs=46+name.size(),eoc=cd+cs;
    Bytes b(eoc+22);w32(b,0,0x04034b50);w16(b,4,20);w32(b,14,0x352441c2);
    w32(b,18,size);w32(b,22,size);w16(b,26,name.size());
    auto extra=data-30-name.size();w16(b,28,extra);
    std::memcpy(b.data()+30,name.data(),name.size());w16(b,30+name.size(),0xffff);w16(b,32+name.size(),extra-4);
    std::memcpy(b.data()+data,"abc",size);
    w32(b,cd,0x02014b50);w16(b,cd+4,20);w16(b,cd+6,20);w32(b,cd+16,0x352441c2);
    w32(b,cd+20,size);w32(b,cd+24,size);w16(b,cd+28,name.size());
    std::memcpy(b.data()+cd+46,name.data(),name.size());
    w32(b,eoc,0x06054b50);w16(b,eoc+8,1);w16(b,eoc+10,1);w32(b,eoc+12,cs);w32(b,eoc+16,cd);
    return b;
}
static bool parse(const Bytes& b,seed_root::ApkEntry& out,std::string_view entry=name){
    return seed_root::find_stored_apk_entry([&](uint64_t at,void* dst,size_t n){
        if(!seed_root::apk_detail::range(at,n,b.size()))return false;
        std::memcpy(dst,b.data()+at,n);return true;
    },b.size(),entry,out);
}
int main(int argc,char**argv){
    setbuf(stdout,nullptr);
    auto good=valid();const size_t cd=16387,eoc=good.size()-22;
    seed_root::ApkEntry out;assert(parse(good,out)&&out.offset==16384&&out.size==3);
    // Unrelated ZIP entries can contain legacy zipalign raw padding.
    auto padded=good;Bytes resource(good.begin()+cd,good.begin()+eoc);
    resource[46]='x';w16(resource,30,3);resource.insert(resource.end(),3,0);
    padded.insert(padded.begin()+eoc,resource.begin(),resource.end());
    auto ep=padded.size()-22;w16(padded,ep+8,2);w16(padded,ep+10,2);w32(padded,ep+12,eoc-cd+resource.size());
    assert(parse(padded,out));
    unsigned rejected=0;
    auto bad=[&](Bytes b){out={123,456};assert(!parse(b,out));assert(!out.offset&&!out.size);++rejected;};
    bad({});auto b=good;b.pop_back();bad(b);
    b=good;b.push_back(0);bad(b);
    b=good;w16(b,eoc+4,1);bad(b);
    b=good;w16(b,eoc+8,2);bad(b);
    b=good;w16(b,eoc+10,0xffff);bad(b);
    b=good;w32(b,eoc+12,0xffffffff);bad(b);
    b=good;w32(b,eoc+16,0xffffffff);bad(b);
    b=good;w16(b,cd+10,8);bad(b); // compressed
    b=good;w16(b,cd+8,1);bad(b); // encrypted
    b=good;w16(b,cd+8,8);bad(b); // data descriptor
    b=good;w32(b,cd+20,4);bad(b);
    b=good;w32(b,cd+24,0xffffffff);bad(b);
    b=good;w16(b,cd+34,1);bad(b);
    b=good;w32(b,cd+42,cd);bad(b);
    b=good;w16(b,cd+28,0xffff);bad(b);
    b=good;b[30]='x';bad(b); // local/central name mismatch
    b=good;w32(b,18,4);bad(b);
    b=good;w32(b,14,0);bad(b);
    b=good;w16(b,6,0x800);bad(b);
    b=good;w16(b,28,0);bad(b); // no 16KB alignment
    b=good;w16(b,30+name.size(),1);bad(b); // ZIP64 extra
    b=good;w16(b,32+name.size(),0xffff);bad(b);
    b=good;b[cd+46]='x';bad(b); // missing target
    b=good;w16(b,eoc+10,2);w16(b,eoc+8,2);bad(b); // count mismatch
    b=good;Bytes record(b.begin()+cd,b.begin()+eoc);b.insert(b.begin()+eoc,record.begin(),record.end());
    auto e2=b.size()-22;w16(b,e2+8,2);w16(b,e2+10,2);w32(b,e2+12,2*record.size());bad(b); // duplicate
    assert(!parse(good,out,"../lib.so"));++rejected;
    assert(!seed_root::find_stored_apk_entry([](uint64_t,void*,size_t){return false;},good.size(),name,out));++rejected;
    seed_preview::Digest empty;assert(empty.finish()=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    seed_preview::Digest abc;abc.update(reinterpret_cast<const unsigned char*>("abc"),3);
    assert(abc.finish()=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    printf("APK_POLICY_PASS positive=2 negative=%u sha_vectors=2\n",rejected);
    if(argc==3){
        int fd=open(argv[1],O_RDONLY|O_CLOEXEC|O_NOFOLLOW);assert(fd>=0);
        assert(seed_root::verified_apk_payload(fd,name,argv[2],out));
        assert(out.offset%16384==0&&out.size>0);
        assert(!seed_root::verified_apk_payload(fd,name,std::string(64,'0'),out));
        close(fd);puts("REAL_APK_SHA_AND_WRONG_SHA_PASS");
    }else assert(argc==1);
}
