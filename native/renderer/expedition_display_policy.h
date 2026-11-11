#pragma once
#include "seed_preview_policy.h"
#include <cstdint>
namespace display_insights {
struct ExpeditionDisplay {
    bool current{},time_known{};
    int state{-1};
    int64_t ticks{-1};
};
inline std::u16string expedition_duration(int64_t ticks) {
    if(ticks<=0)return u"時間待核";
    // Round UP, so a positive sub-minute value never claims completion.
    const int64_t minutes=ticks/600000000+(ticks%600000000!=0);
    const int64_t amount=minutes>=1440?(minutes/1440+(minutes%1440!=0)):
        minutes>=60?(minutes/60+(minutes%60!=0)):minutes;
    const auto n=std::to_string(amount);
    return std::u16string(n.begin(),n.end())+(minutes>=1440?u"天":minutes>=60?u"時":u"分");
}
inline std::u16string expedition_status(std::u16string_view source,const ExpeditionDisplay &e) {
    if(!e.current)return std::u16string(source);
    constexpr std::u16string_view states[]{u"待派",u"去程",u"搬運",u"返程",u"已返"};
    if(e.state<0||e.state>4)return u"狀態待核";
    std::u16string result(states[e.state]);
    if(e.state==4)return result+u"·可領";
    if(!e.time_known)result+=e.state==0?u"·期限未知":u"·未知";
    else if(e.ticks<=0)result+=e.state==0?u"·期限待核":u"·待核";
    else result+=e.state==0?u"·"+expedition_duration(e.ticks)+u"到期":u"·剩"+expedition_duration(e.ticks);
    // The narrower active-card footer keeps state/time readable; no team or
    // future reward/capacity claim is appended to compete for this space.
    return result;
}
}
