#pragma once
#include "seed_preview_policy.h"
#include <limits>
namespace display_insights {
struct PlanterDisplay {
    bool current{},planted{};
    int steps{-1},required{-1};
};
inline std::u16string planter_remaining(const PlanterDisplay &p) {
    if(!p.current||!p.planted||p.steps<0||p.required<=0)return u"剩餘步數未知";
    if(p.steps>=p.required)return u"步數已足";
    auto n=std::to_string(p.required-p.steps);
    return u"剩"+std::u16string(n.begin(),n.end())+u"步";
}
inline std::u16string planter_detail(std::u16string_view source,std::u16string_view name,const PlanterDisplay &p) {
    if(!p.current||!p.planted)return std::u16string(source);
    return seed_preview::compact_card_title(name.empty()?source:name)+u"\n"+planter_remaining(p);
}
}
