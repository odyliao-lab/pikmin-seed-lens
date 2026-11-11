#include "display_settings.h"
#include <cassert>
#include <cstdlib>
#include <filesystem>
int main(){
 if(getuid()!=0)return 2;
#if defined(__ANDROID__)
 char name[]="/data/local/tmp/seed-lens-settings-XXXXXX";
#else
 char name[]="/tmp/seed-lens-settings-XXXXXX";
#endif
 assert(mkdtemp(name));
 using namespace seed_root;using namespace seed_lens;
 const std::string directory=name;int parent=open(name,O_RDONLY|O_DIRECTORY);assert(parent>=0);
 {
  LoadLease lease(name);assert(lease.held());
  for(unsigned group=0;group<4;group++){
   const auto &g=display_groups[group];
   assert(commit_control(lease,parent,display_members[g.first].control,true,getpid(),"123"));
   assert(display_setting(lease,parent,group,"read","123"));
   for(unsigned i=0;i<g.count;i++)assert(read_setting(parent,display_members[g.first+i].control)==Setting::On);
   auto master=std::string("seed-lens-display-group-")+g.key+".txt";
   // Crash boundaries: only master OFF, then each prefix of the fan-out OFF.
   for(unsigned done=0;done<=g.count;done++){
    assert(display_setting(lease,parent,group,"on","123"));
    assert(commit_control(lease,parent,master.c_str(),false,getpid(),"123"));
    for(unsigned i=0;i<done;i++)assert(commit_control(lease,parent,display_members[g.first+i].control,false,getpid(),"123"));
    assert(display_setting(lease,parent,group,"read","123"));
    for(unsigned i=0;i<g.count;i++)assert(read_setting(parent,display_members[g.first+i].control)==Setting::Off);
   }
   auto member=directory+"/"+display_members[g.first].control;
   assert(unlink(member.c_str())==0&&symlink("missing",member.c_str())==0);
   assert(!display_setting(lease,parent,group,"on","123"));
   assert(read_setting(parent,master.c_str())==Setting::Off);
   assert(unlink(member.c_str())==0);
   int fd=open(member.c_str(),O_CREAT|O_WRONLY,0644);assert(fd>=0);assert(write(fd,"bad\n",4)==4);close(fd);
   assert(!display_setting(lease,parent,group,"off","123"));
   assert(read_setting(parent,master.c_str())==Setting::Off);
  }
 }
 close(parent);std::filesystem::remove_all(directory);
 puts("Display settings: actual writer migration, OFF interruption, symlink and invalid-value rejection passed");
}
