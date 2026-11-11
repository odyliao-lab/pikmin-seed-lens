#include "../renderer/planter_display_policy.h"
#include <cassert>
#include <climits>
#include <cstdio>
int main(){
    using namespace display_insights;
    const std::u16string source=u"<b>灰色花苗</b>",name=u"岩石三葉草苗";
    int cases=0;
    for(bool current:{false,true})for(bool planted:{false,true})
    for(int steps:{-1,0,230,3001,5000,5001,INT_MAX})for(int required:{-1,0,1000,5000,INT_MAX}){
        PlanterDisplay p{current,planted,steps,required};auto s=planter_detail(source,name,p);
        if(!current||!planted)assert(s==source);
        else if(steps<0||required<=0)assert(s.ends_with(u"剩餘步數未知"));
        else if(steps>=required)assert(s.ends_with(u"步數已足"));
        else{auto n=std::to_string(required-steps);assert(s.ends_with(u"剩"+std::u16string(n.begin(),n.end())+u"步"));}
        ++cases;
    }
    assert(planter_detail(source,name,{true,true,3001,5000})==u"岩石三葉草苗\n剩1999步");
    assert(planter_detail(source,u"",{false,true,0,1000})==source);
    assert(planter_detail(source,u"",{true,true,0,1000}).starts_with(u"灰色花苗\n"));
    assert(planter_detail(u"3,001 / 5,000步",name,{true,true,3001,5000})==u"岩石三葉草苗\n剩1999步");
    assert(planter_detail(u"3,001 / 5,000步",name,{false,true,3001,5000})==u"3,001 / 5,000步");
    for(auto n:{u"長長長長長長長長長長花苗",u"ABC😀😀😀😀花苗"}){
        auto s=planter_detail(source,n,{true,true,0,1000});auto first=s.substr(0,s.find(u'\n'));
        assert(first.size()<=8);for(size_t i=0;i<first.size();++i)if(first[i]>=0xd800&&first[i]<=0xdbff)assert(++i<first.size()&&first[i]>=0xdc00&&first[i]<=0xdfff);
    }
    std::printf("PASS planter display %d current/membership/unknown/zero/overflow cases; display only\n",cases);
}
