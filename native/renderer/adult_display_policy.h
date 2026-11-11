#pragma once
#include <string>
#include "seed_preview_policy.h"

namespace display_insights {
// Confirmed observation only. This is never permission to feed or collect.
struct AdultState {
    bool current{};
    int flower{-1}, petals{-1}, status{-1};
};
inline std::u16string adult_state_label(const AdultState &state) {
    if(!state.current)return u"狀態未知";
    std::u16string label;
    switch(state.flower) {
        case 1:label=u"葉";break;
        case 3:label=u"花蕾";break;
        case 4:label=u"開花";break;
        case 5:label=u"可收瓣";break;
        case 6:label=u"枯萎";break;
        default:label=u"花態未知";break;
    }
    if(state.flower==5 && state.petals>=0 && state.petals<=99) {
        label=u"收";
        for(auto c:std::to_string(state.petals))label+=static_cast<char16_t>(c);
        label+=u"瓣";
    }
    if(state.status==2)label+=u"·任務";
    return label;
}
inline std::u16string adult_card_label(std::u16string_view source,const AdultState &state) {
    // The real name stays in Record/the game. Only its list presentation is
    // compacted; otherwise a long auto-name wraps over hearts and birthplace.
    std::u16string normalized(source);
    for(auto &c:normalized)if(c==u'\n'||c==u'\r'||c==u'\t')c=u' ';
    auto title=seed_preview::plain_name(normalized);
    if(title.size()>6) {
        auto keep=std::size_t{5};
        if(title[keep-1]>=0xd800&&title[keep-1]<=0xdbff)--keep;
        title=title.substr(0,keep)+u"…";
    }
    if(title.empty())return std::u16string(source);
    return title+u"\n"+adult_state_label(state);
}
}
