#pragma once
#include <array>
#include <cstdint>
#include <string_view>

namespace seed_preview {
struct HookProfile {uint64_t rva;std::array<uint32_t,6> prefix;};
struct GameProfile {
    const char* version;uint64_t versionCode,elfSize;const char* elfSha;
    // Reviewed versions share the checked runtime layout. A future
    // layout change needs a separate implementation, not just another RVA row.
    unsigned layout;
    HookProfile inventory,text,canvas,expedition,detail;
};
inline constexpr GameProfile game_profiles[]{
    {"156.0",1792142832,256261664,"b1a9781e8e7ddbdf984f6e3e90c7f753dd3ac149881fc9c14a46892eda6f6b5d",1,
     {0x669c4a4,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x6e96ca0,{0xf81a0ffd,0xa9016ffe,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0xe1031dc,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x6070ac0,{0xd10103ff,0xa9015ffe,0xa90257f6,0xa9034ff4,0x9004a056,0xaa0203f5}},
     {0x607de08,{0xd102c3ff,0x6d0423e9,0xf9002bfe,0xa9066ffc,0xa90767fa,0xa9085ff8}}},
    {"156.1",1793076318,256384336,"97ef6650f73d99ca8c7ebbbdf2d059d0d41ea68c036fde05634003f6a052aefb",1,
     {0x66ab9cc,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x6eaee6c,{0xf81a0ffd,0xa9016ffe,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0xe11dd28,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x6082dc8,{0xd10103ff,0xa9015ffe,0xa90257f6,0xa9034ff4,0x9004a0b6,0xaa0203f5}},
     {0x6090110,{0xd102c3ff,0x6d0423e9,0xf9002bfe,0xa9066ffc,0xa90767fa,0xa9085ff8}}},
    {"157.0",1793360009,257043464,"6c36ef78a6d1f45cf5b36c73761ed526226a081d981001b71608984afde246a5",1,
     {0x66f6730,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x6f098e4,{0xf81a0ffd,0xa9016ffe,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0xe1b5ba0,{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}},
     {0x60c92b4,{0xd10103ff,0xa9015ffe,0xa90257f6,0xa9034ff4,0xd004a376,0xaa0203f5}},
     {0x60d66c0,{0xd102c3ff,0x6d0423e9,0xf9002bfe,0xa9066ffc,0xa90767fa,0xa9085ff8}}},
};
inline const GameProfile* profile_for_digest(uint64_t size,std::string_view sha){
    for(const auto& p:game_profiles)if(p.elfSize==size&&sha==p.elfSha)return &p;
    return nullptr;
}
inline const GameProfile* profile_for_version(std::string_view version,uint64_t code){
    for(const auto& p:game_profiles)if(version==p.version&&code==p.versionCode)return &p;
    return nullptr;
}
inline bool known_game_size(uint64_t size){for(const auto& p:game_profiles)if(p.elfSize==size)return true;return false;}
}
