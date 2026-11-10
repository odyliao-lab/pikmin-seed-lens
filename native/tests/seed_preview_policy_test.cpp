#include "../renderer/seed_preview_policy.h"
#undef NDEBUG
#include <cassert>
int main(){
    using seed_preview::decorate;
    const std::u16string source=u"金色花苗<block>（咖啡杯）</block>";
    const std::u16string names[]{u"紅色",u"藍色",u"黃色",u"白色",u"紫色",u"岩石",u"羽翅",u"冰色"};
    for(int colour=1;colour<=8;++colour){
        assert(decorate(source,4,colour,true)==names[colour-1]+u"金苗<block>（咖啡杯）</block>");
        assert(decorate(source,4,colour,false)==source);
        assert(decorate(source,0,colour,true)==source);
    }
    assert(decorate(source,4,0,true)==source&&decorate(source,4,9,true)==source);
    assert(decorate(u"",4,1,true).empty());
    assert(decorate(u"普通花苗",4,1,true)==u"普通花苗");
    assert(decorate(u"Golden Seedling",4,1,true)==u"Golden Seedling");
    // Idempotent output: feeding a decorated label back cannot add another prefix.
    auto red=decorate(source,4,1,true);assert(decorate(red,4,1,true)==red);
    for(int seed:{1,3,4,5,6,7,8,9,10,23,24,25,26,27,28,29,30,31,32}) {
        assert(decorate(u"大花苗",seed,1,true,u"幸運草",26)==(seed==4?u"紅色四葉金苗":u"紅色四葉花苗"));
        assert(decorate(u"花苗",seed,2,true,u"御神籤",64)==(seed==4?u"藍色大吉金苗":u"藍色大吉苗"));
        assert(decorate(u"花苗",seed,2,true,u"咖啡杯")== (seed==4?u"藍色咖啡杯金苗":u"藍色咖啡杯苗"));
    }
    assert(decorate(u"黃色花苗",1,3,true,u"幸運草",25)==u"黃色三葉花苗");
    for(auto pair:{std::pair{64,u"大吉"},std::pair{65,u"吉"},std::pair{66,u"中吉"},std::pair{67,u"小吉"},std::pair{68,u"末吉"}})
        assert(decorate(u"大花苗",3,2,true,u"御神籤",pair.first)==std::u16string(u"藍色")+pair.second+u"苗");
    assert(decorate(source,4,2,true,u"<b>咖啡杯</b>")==u"藍色咖啡杯金苗");
    assert(decorate(source,4,2,true,u"咖啡杯花苗")==u"藍色咖啡杯金苗");
    assert(decorate(source,4,2,false,u"咖啡杯")==source);
    assert(decorate(u"大花苗",3,0,true,u"咖啡杯")==u"大花苗");
    assert(decorate(u"大花苗",3,1,true)==u"大花苗");
    assert(decorate(u"Golden Seedling",4,1,true,u"咖啡杯")==u"Golden Seedling");
    for(auto name:{u"水果",u"禮物盒",u"紅色皮克敏"})assert(decorate(name,4,1,true,u"咖啡杯")==name);
    assert(seed_preview::subtype(137).empty()&&seed_preview::subtype(138).empty()); // no upgrade prediction
    assert(seed_preview::plain_name(u"<b broken").empty());
    assert(seed_preview::minimum_font_size(24)==12&&seed_preview::minimum_font_size(16)==12);
    assert(seed_preview::minimum_font_size(12)==12);
    seed_preview::FontState font;
    int restores=0;
    font.capture(false,10,30,24);
    font.capture(true,18,24,18); // repeated layout must not replace the original
    font.restore([&](bool a,int lo,int hi,int size){++restores;assert(!a&&lo==10&&hi==30&&size==24);});
    font.restore([&](bool,int,int,int){++restores;});
    assert(restores==1&&!font.saved);
    font.capture(true,12,36,32); // another row can have a different original font
    font.restore([&](bool a,int lo,int hi,int size){++restores;assert(a&&lo==12&&hi==36&&size==32);});
    font.capture(false,0,0,0);assert(!font.saved);
    assert(seed_preview::enabled_control("on\n"));
    for(auto invalid:{"on","off\n","on\nextra","ON\n"," on\n",""})assert(!seed_preview::enabled_control(invalid));
}
