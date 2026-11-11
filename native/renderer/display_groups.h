#pragma once
#include <array>
#include <string_view>

namespace seed_lens {
struct DisplayMember {const char *key,*control,*report;};
// Independent member contracts. D02 deliberately retains the full
// nectar-stock-preview identity, checked against renderer and caller separately.
inline constexpr std::array<DisplayMember,8> display_members{{
 {"seed-preview","seed-lens-seed-preview.txt","seed_lens_seed_preview_status.tsv"},
 {"seed-collection","seed-lens-seed-collection.txt","seed_lens_seed_collection_status.tsv"},
 {"planter-display","seed-lens-planter-display.txt","seed_lens_planter_display_status.tsv"},
 {"adult-state-preview","seed-lens-adult-state-preview.txt","seed_lens_adult_state_preview_status.tsv"},
 {"adult-decoration-preview","seed-lens-adult-decoration-preview.txt","seed_lens_adult_decoration_preview_status.tsv"},
 {"gift-preview","seed-lens-gift-preview.txt","seed_lens_gift_preview_status.tsv"},
 {"expedition-display","seed-lens-expedition-display.txt","seed_lens_expedition_display_status.tsv"},
 {"nectar-stock-preview","seed-lens-nectar-stock-preview.txt","seed_lens_nectar_stock_preview_status.tsv"}
}};
struct DisplayGroup {const char *key;unsigned first,count;};
inline constexpr std::array<DisplayGroup,4> display_groups{{
 {"seedlings",0,3},{"pikmin",3,2},{"expeditions",5,2},{"stock",7,1}
}};
enum class Setting {Missing,Off,On,Invalid};
inline Setting parse_setting(std::string_view value) {
 if(value=="on\n")return Setting::On;
 if(value=="off\n")return Setting::Off;
 return Setting::Invalid;
}
// A persisted intention is authoritative, including OFF after interrupted fan-out.
// Invalid member/master reads stop reconciliation instead of becoming OFF.
inline Setting reconcile(Setting master,const Setting *members,unsigned count) {
 if(!members||count<1||count>3||master==Setting::Invalid)return Setting::Invalid;
 bool on=false;
 for(unsigned i=0;i<count;++i){if(members[i]==Setting::Invalid)return Setting::Invalid;on|=members[i]==Setting::On;}
 if(master!=Setting::Missing)return master;
 return on?Setting::On:Setting::Off;
}
}
