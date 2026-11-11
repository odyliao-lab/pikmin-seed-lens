#include "../renderer/display_groups.h"
#include "../renderer/display_contract.h"
#include <cassert>
#include <string>
int main(){
 using namespace seed_lens;
 assert(std::string(display_members[0].control)==seed_control_leaf);
 assert(std::string(display_members[0].report)==seed_report_leaf);
 assert(std::string(display_members[7].key)=="nectar-stock-preview");
 for(auto group:display_groups){
  for(unsigned mask=0;mask<(1u<<group.count);++mask){
   Setting values[3]{};for(unsigned i=0;i<group.count;++i)values[i]=(mask&(1u<<i))?Setting::On:Setting::Off;
   assert(reconcile(Setting::Missing,values,group.count)==(mask?Setting::On:Setting::Off));
   assert(reconcile(Setting::Off,values,group.count)==Setting::Off);
   assert(reconcile(Setting::On,values,group.count)==Setting::On);
   for(unsigned bad=0;bad<group.count;++bad){auto prior=values[bad];values[bad]=Setting::Invalid;
    assert(reconcile(Setting::Off,values,group.count)==Setting::Invalid);values[bad]=prior;}
  }
 }
 for(auto bad:{"on","off","on\r\n","on\noff\n","","garbage"})assert(parse_setting(bad)==Setting::Invalid);
 assert(parse_setting("on\n")==Setting::On&&parse_setting("off\n")==Setting::Off);
}
