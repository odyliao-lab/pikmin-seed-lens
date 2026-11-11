#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
namespace seed_extended {
struct DisplayHook {size_t rva;std::array<uint32_t,6> prefix;};
struct DisplayGameProfile {
 const char *version;size_t elf_size;const char *elf_sha;
 size_t inventory_revision,inventory_items;
 DisplayHook inventory,text,canvas,expedition,detail,honey,extract;
 DisplayHook adult{}; // main adult list; unsupported legacy profiles leave this zero
 DisplayHook planted_detail{}; // planted seed detail title, exact Game157 only
};
// Each build target accepts one exact game, never a nearest-version fallback.
inline constexpr DisplayGameProfile display_game{"157.0",257043464,"6c36ef78a6d1f45cf5b36c73761ed526226a081d981001b71608984afde246a5",0x2f8,0x2d8,
 {0x66f6730,{{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}}},
 {0x6f098e4,{{0xf81a0ffd,0xa9016ffe,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}}},
 {0xe1b5ba0,{{0xa9ba7bfd,0xa9016ffc,0xa90267fa,0xa9035ff8,0xa90457f6,0xa9054ff4}}},
 {0x60c92b4,{{0xd10103ff,0xa9015ffe,0xa90257f6,0xa9034ff4,0xd004a376,0xaa0203f5}}},
 {0x60d66c0,{{0xd102c3ff,0x6d0423e9,0xf9002bfe,0xa9066ffc,0xa90767fa,0xa9085ff8}}},
 {0x6010e70,{{0xd10143ff,0xa90167fe,0xa9025ff8,0xa90357f6,0xa9044ff4,0xf004a935}}},
 {0x6003bec,{{0xd10243ff,0xf9002bfe,0xa9065ff8,0xa90757f6,0xa9084ff4,0x9004a9b4}}},
 {0x5ac78c0,{{0xfc1c0fe8,0xa9015ffe,0xa90257f6,0xa9034ff4,0xd004d375,0xaa0103f3}}},
 {0x5aa26b0,{{0xd10143ff,0xa90167fe,0xa9025ff8,0xa90357f6,0xa9044ff4,0xf004d495}}}
};
}
