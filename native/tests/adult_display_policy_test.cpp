#include "../renderer/adult_display_policy.h"
#include <cassert>
#include <iostream>
using namespace display_insights;
int main() {
    for(int flower=-1;flower<=8;++flower)for(int status:{-1,0,1,2,32,99})
        for(int petals:{-1,0,1,2,1000000,1000001}) {
            AdultState state{false,flower,petals,status};
            assert(adult_state_label(state)==u"狀態未知");
            state.current=true;auto label=adult_state_label(state);
            assert(!label.empty());
            assert((label.find(u"·任務")!=label.npos)==(status==2));
            assert(label.find(u"瓣")==label.npos || flower==5);
            assert(label.size()<=7);
            if(flower==5&&(petals<0||petals>99))assert(label.starts_with(u"可收瓣"));
            assert(label.find(u"可餵")==label.npos);
        }
    assert(adult_state_label({true,1,-1,1})==u"葉");
    assert(adult_state_label({true,3,-1,32})==u"花蕾");
    assert(adult_state_label({true,4,999,1})==u"開花");
    assert(adult_state_label({true,5,2,2})==u"收2瓣·任務");
    assert(adult_state_label({true,5,0,1})==u"收0瓣");
    assert(adult_state_label({true,6,-1,1})==u"枯萎");
    assert(adult_state_label({true,0,-1,1})==u"花態未知");
    for(auto name:{u"Electronic Discovery Service附近的仙女燈的黃色皮克敏",
                   u"國營昭和記念公園附近的白色皮克敏",u"自訂\n名稱\t甲",u"自訂紅",u"\U0001f331\U0001f33b\U0001f338\U0001f337\U0001f33c長名稱"}) {
        std::u16string source(name),before=source;
        auto label=adult_card_label(source,{true,6,-1,1});
        assert(source==before);
        auto line=label.find(u'\n');assert(line!=label.npos&&line<=6);
        assert(label.find(u'\n',line+1)==label.npos);
        assert(line==0||label[line-1]<0xd800||label[line-1]>0xdbff);
        assert(label.substr(line+1)==u"枯萎");
    }
    std::cout<<"PASS adult confirmed display states, 360 unknown/yield/task combinations; no feeding authorization or counter inference\n";
}
