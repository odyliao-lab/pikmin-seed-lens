#include "../renderer/decoration_display_policy.h"
#include <cassert>
#include <iostream>
using namespace display_insights;
int main() {
    assert(decoration_label({false,500,u"已知名字"})==u"裝飾未知");
    assert(decoration_label({true,-1,u"已知名字"})==u"裝飾未知");
    assert(decoration_label({true,1,u"未來餐廳"})==u"未裝飾");
    assert(decoration_label({true,2,{}})==u"裝飾未知");
    assert(decoration_label({true,2,u"<b>餐廳</b>"})==u"餐廳");
    assert(decoration_label({true,2,u"<b餐廳"})==u"裝飾未知");
    assert(decoration_label({true,2,u"餐廳"})==u"餐廳");
    assert(adult_short_state({true,5,0,1})==u"收0");
    assert(adult_short_state({true,5,-1,1})==u"收");
    assert(adult_short_state({true,5,99,2})==u"收99任");
    assert(adult_short_state({false,5,99,2})==u"？");
    int cases=0;
    for(bool current:{false,true})for(int asset:{-1,0,1,2,500})
    for(auto name:{u"",u"餐廳",u"餐\n廳\t",u"日本寺廟與神社",u"\U0001f33b\U0001f338\U0001f331裝飾"})
    for(int flower:{-1,0,1,3,4,5,6,99})for(int petals:{-1,0,2,99,1000000})for(int status:{-1,1,2}) {
        AdultDecoration decoration{current,asset,name};AdultState state{current,flower,petals,status};
        std::u16string original=u"很長的自訂皮克敏名稱",before=original;
        for(bool both:{false,true}) {
            auto label=adult_decoration_card(original,decoration,both?&state:nullptr);
            assert(original==before);auto line=label.find(u'\n');
            assert(line!=label.npos&&line<=6&&label.find(u'\n',line+1)==label.npos);
            auto subtitle=label.substr(line+1);assert(subtitle.size()<=7);
            for(auto text:{label.substr(0,line),subtitle})
                assert(text.empty()||text.back()<0xd800||text.back()>0xdbff);
            if(!current)assert(subtitle.find(u"未知")!=subtitle.npos||subtitle.find(u"？")!=subtitle.npos);
            ++cases;
        }
    }
    std::cout<<"PASS acquired decoration/unknown/non-decorated and combined D03 layout "<<cases<<" cases; no future outcome\n";
}
