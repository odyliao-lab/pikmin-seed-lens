#include "display_settings.h"
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
static void legacy_case(bool owned,bool invalid){
#if defined(__ANDROID__)
 char name[]="/data/local/tmp/seed-lens-migration-XXXXXX";
#else
 char name[]="/tmp/seed-lens-migration-XXXXXX";
#endif
 assert(mkdtemp(name));const std::string directory=name;
 using namespace seed_root;using namespace seed_lens;
 int parent=open(name,O_RDONLY|O_DIRECTORY);assert(parent>=0);
 {
  LoadLease lease(name);assert(lease.held());
  if(owned)assert(commit_control(lease,parent,"pikmin-seed-preview.txt",true,123,"456"));
  else{int fd=openat(parent,"pikmin-seed-preview.txt",O_CREAT|O_WRONLY,0644);assert(fd>=0&&write(fd,"on\n",3)==3);close(fd);}
  const std::string boot="baf6009f-3eca-45bf-a05d-086cf5c2d2fb";
  {LoadAttempt unknown(lease,boot,123,"456",10000,42,"888","nativebridge");assert(unknown.ready());}
  auto journal=directory+"/attempt-"+boot+"-123-456";
  auto readJournal=[&](){std::ifstream in(journal);return std::string(std::istreambuf_iterator<char>(in),{});};
  const auto before=readJournal();assert(before.find("UNKNOWN")!=std::string::npos);
  if(invalid){int fd=openat(parent,"pikmin-seed-preview.txt",O_WRONLY|O_TRUNC);assert(fd>=0&&write(fd,"bad\n",4)==4);close(fd);}
  bool result=display_setting(lease,parent,0,"read","123");
  if(owned&&invalid){assert(!result);assert(read_setting(parent,"seed-lens-display-group-seedlings.txt")==Setting::Missing);}
  else{
   assert(result);const auto expected=owned?Setting::On:Setting::Off;
   for(unsigned i=0;i<3;i++)assert(read_setting(parent,display_members[i].control)==expected);
   assert(display_setting(lease,parent,0,"off","123"));
   assert(display_setting(lease,parent,0,"read","123"));
   assert(read_setting(parent,display_members[0].control)==Setting::Off);
  }
  assert(readJournal()==before); // Migration never changes UNKNOWN history.
 }
 close(parent);std::filesystem::remove_all(directory);
}
int main(){
 if(getuid()!=0)return 2;
 legacy_case(false,false);legacy_case(true,false);legacy_case(true,true);
 const std::string row="v1\t10000\t123\t157.0\tready\t1\t3\t0\t0\t0\t456\n";
 assert(!seed_root::fresh_display_status(row,123,"456",10000,"157.0"));
 const auto paired=row.substr(0,row.size()-1)+"\t"+seed_lens::display_build+"\n";
 assert(seed_root::fresh_display_status(paired,123,"456",10000,"157.0"));
 assert(!seed_root::fresh_display_status(row.substr(0,row.size()-1)+"\tother-build\n",123,"456",10000,"157.0"));
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
   assert(!display_all_off(lease,parent,"123"));
   assert(unlink(member.c_str())==0);

  }

  // All-OFF must override every valid ON, persist across reads and recover
  // after every prefix of master/member commits (the App retains pending intent).
  for(unsigned cut=0;cut<=12;cut++){
   for(unsigned g=0;g<4;g++)assert(display_setting(lease,parent,g,"on","123"));
   for(unsigned step=0;step<cut;step++){
    std::string leaf=step<4?std::string("seed-lens-display-group-")+display_groups[step].key+".txt":display_members[step-4].control;
    assert(commit_control(lease,parent,leaf.c_str(),false,getpid(),"123"));
   }
   assert(display_all_off(lease,parent,"123"));
   for(unsigned g=0;g<4;g++)assert(display_setting(lease,parent,g,"read","123"));
   for(const auto &m:display_members)assert(read_setting(parent,m.control)==Setting::Off);
  }
 }
 close(parent);std::filesystem::remove_all(directory);
 puts("Display settings: actual writer migration, OFF interruption, symlink and invalid-value rejection passed");
}
