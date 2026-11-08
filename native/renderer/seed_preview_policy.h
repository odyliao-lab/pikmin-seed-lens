#pragma once
#include <string>
#include <string_view>
#include <algorithm>

namespace seed_preview {
// These are actual treasure classifications, NOT rare-upgrade predictions.
inline std::u16string_view subtype(int treasure) {
    switch(treasure) {
        case 25:return u"三葉"; case 26:return u"四葉";
        case 64:return u"大吉"; case 65:return u"吉"; case 66:return u"中吉";
        case 67:return u"小吉"; case 68:return u"末吉";
        default:return {};
    }
}
inline std::u16string plain_name(std::u16string_view text) {
    std::u16string result; bool tag=false;
    for(auto c:text) {
        if(c==u'<'){tag=true;continue;}
        if(c==u'>'){if(!tag)return {};tag=false;continue;}
        if(!tag)result+=c;
    }
    if(tag||result.size()>64)return {};
    while(!result.empty()&&(result.front()==u' '||result.front()==u'\n'))result.erase(0,1);
    while(!result.empty()&&(result.back()==u' '||result.back()==u'\n'))result.pop_back();
    return result;
}
// Only decorate Traditional Chinese seed labels. Unknown data/languages keep the
// original text. The name comes from the game's localized future-decor catalog.
inline std::u16string decorate(std::u16string_view source,int seed_type,int colour,bool enabled,
                              std::u16string_view catalog_name={},int treasure=0) {
    constexpr std::u16string_view colours[]{u"",u"紅色",u"藍色",u"黃色",u"白色",u"紫色",u"岩石",u"羽翅",u"冰色"};
    auto original=plain_name(source);
    if(!enabled||seed_type<=0||colour<1||colour>8||original.find(u"花苗")==std::u16string::npos)
        return std::u16string(source);
    auto kind=std::u16string(subtype(treasure));
    if(kind.empty())kind=plain_name(catalog_name);
    if(kind.empty()) {
        // Existing gold colour preview remains available when decor is unknown.
        if(seed_type==4&&source.starts_with(u"金色花苗"))
            return std::u16string(colours[colour])+u"金苗"+std::u16string(source.substr(4));
        return std::u16string(source);
    }
    // Catalog labels normally contain the decoration only. Defensive normalization
    // prevents duplicated 苗 suffixes without inventing an unknown decoration.
    if(kind.ends_with(u"花苗"))kind.resize(kind.size()-2);
    else if(kind.ends_with(u"苗"))kind.pop_back();
    if(kind.empty())return std::u16string(source);
    const auto suffix=seed_type==4?u"金苗":(treasure==25||treasure==26?u"花苗":u"苗");
    return std::u16string(colours[colour])+kind+suffix;
}
inline int minimum_font_size(int original) { return std::max(12,(original*3+3)/4); }
struct FontState {
    bool saved{},automatic{};
    int minimum{},maximum{},size{};
    void capture(bool enabled,int low,int high,int original) {
        if(saved||original<12||original>256)return;
        saved=true;automatic=enabled;minimum=low;maximum=high;size=original;
    }
    template<class Restore> void restore(Restore apply) {
        if(!saved)return;
        apply(automatic,minimum,maximum,size);saved=false;
    }
};
inline bool enabled_control(std::string_view bytes) { return bytes=="on\n"; }
}
