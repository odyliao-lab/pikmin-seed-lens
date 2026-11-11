#include "../renderer/expedition_display_policy.h"
#include <cassert>
#include <limits>
#include <iostream>
int main(){
    using namespace display_insights;int cases=0;
    for(int state=-1;state<=5;++state)for(bool current:{false,true})for(bool known:{false,true})
        for(int64_t ticks:{int64_t(-1),int64_t(0),int64_t(1),int64_t(600000000),int64_t(600000001),std::numeric_limits<int64_t>::max()}){
            ExpeditionDisplay e{current,known,state,ticks};auto s=expedition_status(u"原地點",e);++cases;
            assert(!s.empty());if(!current)assert(s==u"原地點");
            else if(state<0||state>4)assert(s==u"狀態待核");
            else if(state==4)assert(s==u"已返·可領");
            else if(!known)assert(s.find(u"未知")!=s.npos);
            else if(ticks<=0)assert(s.find(u"待核")!=s.npos&&s.find(u"可領")==s.npos);
            else assert(s.find(u"待核")==s.npos&&s.find(u"未知")==s.npos&&s.find(u"可領")==s.npos);
        }
    assert(expedition_duration(1)==u"1分");assert(expedition_duration(600000001)==u"2分");
    assert(expedition_duration(36000000000)==u"1時");assert(expedition_duration(864000000000)==u"1天");
    assert(expedition_status(u"x",{true,true,0,1})==u"待派·1分到期");
    assert(expedition_status(u"x",{true,true,3,1})==u"返程·剩1分");
    assert(expedition_status(u"x",{true,true,3,0})==u"返程·待核");
    assert(expedition_status(u"x",{true,false,3,-1})==u"返程·未知");
    std::cout<<"PASS expedition display "<<cases<<" current/state/missing/zero/overflow cases plus eight rounding/state assertions; display only\n";
}
