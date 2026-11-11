#include "../renderer/gps_copy_policy.h"
#include <cassert>
#include <limits>
int main(){using namespace seed_lens::gps;Selection s;
 assert(!s.observe("owner",u"A",0,true));assert(!s.observe("owner",u"A",399,true));assert(s.observe("owner",u"A",400,true));s.attempted(400,true);assert(!s.observe("owner",u"A",900,true));
 assert(!s.observe("owner",u"B",901,true));assert(s.observe("owner",u"B",1301,true));s.attempted(1301,true);
 assert(!s.observe("owner",u"A",1302,true));assert(s.observe("owner",u"A",1702,true));
 assert(!s.observe("owner",u"A",1703,false));assert(!s.observe("owner",u"A",1704,true));assert(s.observe("owner",u"A",2104,true));
 assert(!s.observe("",u"A",2105,true));assert(!s.observe("new-owner",u"A",2106,true));assert(s.observe("new-owner",u"A",2506,true));
 for(int i=0;i<3;i++){assert(s.observe("new-owner",u"A",2506+i*400,true));s.attempted(2506+i*400,false);}assert(!s.observe("new-owner",u"A",5000,true));
 assert(!s.observe("owner",u"",5001,true));assert(coordinate(1,2));assert(!coordinate(0,0));assert(!coordinate(91,2));assert(!coordinate(1,181));assert(!coordinate(std::numeric_limits<double>::quiet_NaN(),2));
}
