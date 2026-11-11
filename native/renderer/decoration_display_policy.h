#pragma once
#include "adult_display_policy.h"

namespace display_insights {
struct AdultDecoration {
    bool current{};
    int asset{-1};
    std::u16string name;
};
inline std::u16string compact_adult_text(std::u16string_view source,std::size_t limit) {
    if(!limit)return {};
    auto text=seed_preview::plain_name(source);
    for(auto &c:text)if(c==u'\n'||c==u'\r'||c==u'\t')c=u' ';
    if(text.size()<=limit)return text;
    auto keep=limit-1;
    if(keep&&text[keep-1]>=0xd800&&text[keep-1]<=0xdbff)--keep;
    return text.substr(0,keep)+u"…";
}
inline std::u16string decoration_label(const AdultDecoration &decoration) {
    if(!decoration.current||decoration.asset<1)return u"裝飾未知";
    if(decoration.asset==1)return u"未裝飾";
    auto name=seed_preview::plain_name(decoration.name);
    return name.empty()?u"裝飾未知":name;
}
inline std::u16string adult_short_state(const AdultState &state) {
    if(!state.current)return u"？";
    std::u16string label;
    switch(state.flower) {
        case 1:label=u"葉";break;
        case 3:label=u"蕾";break;
        case 4:label=u"花";break;
        case 5:
            label=u"收";
            if(state.petals>=0&&state.petals<=99)
                for(auto c:std::to_string(state.petals))label+=static_cast<char16_t>(c);
            break;
        case 6:label=u"枯";break;
        default:label=u"？";break;
    }
    if(state.status==2)label+=u"任";
    return label;
}
inline std::u16string adult_decoration_card(std::u16string_view source,
        const AdultDecoration &decoration,const AdultState *state) {
    // Reuse D03's bounded title and normalization; retain the actual model name.
    auto title=adult_card_label(source,{});
    auto newline=title.find(u'\n');
    if(newline==title.npos)return std::u16string(source);
    title.resize(newline);
    auto subtitle=decoration_label(decoration);
    if(state) {
        if(subtitle==u"裝飾未知")subtitle=u"？";
        else if(decoration.current&&decoration.asset==1)subtitle=u"無飾";
        auto status=adult_short_state(*state);
        subtitle=compact_adult_text(subtitle,7-1-status.size())+u"·"+status;
    } else subtitle=compact_adult_text(subtitle,6);
    return title+u"\n"+subtitle;
}
}
