#pragma once
#include "control_transaction.h"
#include "display_groups.h"
#include <dirent.h>

namespace seed_root {
inline seed_lens::Setting read_setting(int parent,const char *leaf) {
 int fd=openat(parent,leaf,O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
 if(fd<0)return errno==ENOENT?seed_lens::Setting::Missing:seed_lens::Setting::Invalid;
 struct stat st{};char bytes[8]{};ssize_t count=-1;
 if(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_uid==0&&st.st_nlink==1&&!(st.st_mode&0022)&&st.st_size>0&&st.st_size<8)
  count=read(fd,bytes,sizeof(bytes));
 close(fd);return count>0?seed_lens::parse_setting({bytes,static_cast<size_t>(count)}):seed_lens::Setting::Invalid;
}
// Old standalone releases used one shared-looking filename. Import it only
// once, with evidence of our canonical loader's private control audit. Never
// poll it for display or read any of the other application's eight settings.
enum class LegacyHistory {Absent,Owned,Unreadable};
inline LegacyHistory standalone_config_history(const LoadLease &lease){
 DIR *dir=fdopendir(dup(lease.directory()));if(!dir)return LegacyHistory::Unreadable;rewinddir(dir);
 bool found=false,uncertain=false;unsigned count=0;
 for(;;){
  errno=0;auto entry=readdir(dir);if(!entry){if(errno)uncertain=true;break;}
  if(++count>4096){uncertain=true;break;}
  std::string_view name=entry->d_name;if(!name.starts_with("control-")||name.size()!=40)continue;
  if(name.substr(8).find_first_not_of("0123456789abcdef")!=name.npos)continue;
  int fd=openat(lease.directory(),entry->d_name,O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
  char bytes[257]{};ssize_t n=fd>=0&&owned_file(fd,0)?read(fd,bytes,sizeof(bytes)):-1;if(fd>=0)close(fd);
  if(n<=0||n>256){uncertain=true;continue;}std::string_view text(bytes,static_cast<size_t>(n));auto end=text.find('\n');
  if(end==text.npos){uncertain=true;continue;}auto line=text.substr(0,end);std::vector<std::string_view> f;size_t at=0;
  for(size_t i=0;i<=line.size();++i)if(i==line.size()||line[i]=='\t'){f.push_back(line.substr(at,i-at));at=i+1;}
  uint64_t pid=0;
  if(f.size()==5&&f[0]=="v1"&&f[1]=="INTENT"&&decimal(f[2],pid)&&pid>1&&pid<=2147483647
     &&start_ticks(f[3])&&(f[4]=="on"||f[4]=="off")
     &&text.substr(end+1)=="v1\tCOMMITTED_NOT_EFFECT_VERIFIED\n"){found=true;break;}
  uncertain=true;
 }
 closedir(dir);return found?LegacyHistory::Owned:uncertain?LegacyHistory::Unreadable:LegacyHistory::Absent;
}
// Persist master first. Retrying interrupted OFF never imports a remaining ON.
inline bool display_setting(const LoadLease &lease,int parent,unsigned group,std::string_view explicitValue,std::string_view ownerStart) {
 using namespace seed_lens;if(group>=display_groups.size())return false;
 const auto &g=display_groups[group];Setting members[3]{};
 for(unsigned i=0;i<g.count;++i)members[i]=read_setting(parent,display_members[g.first+i].control);
 auto leaf=std::string("seed-lens-display-group-")+g.key+".txt";
 const auto master=read_setting(parent,leaf.c_str());
 Setting legacy=Setting::Missing;
 if(group==0&&master==Setting::Missing&&members[0]==Setting::Missing&&members[1]==Setting::Missing&&members[2]==Setting::Missing){
  const auto history=standalone_config_history(lease);
  if(history==LegacyHistory::Unreadable)return false;
  if(history==LegacyHistory::Owned){legacy=read_setting(parent,"pikmin-seed-preview.txt");if(legacy==Setting::Invalid)return false;}
 }
 auto wanted=reconcile(master,members,g.count);if(wanted==Setting::Invalid)return false;
 if(master==Setting::Missing&&legacy==Setting::On)wanted=Setting::On;
 if(explicitValue!="read"){
  if(explicitValue!="on"&&explicitValue!="off")return false;
  wanted=explicitValue=="on"?Setting::On:Setting::Off;
 }
 const bool on=wanted==Setting::On;
 if(master!=wanted&&!commit_control(lease,parent,leaf.c_str(),on,getpid(),ownerStart))return false;
 for(unsigned i=0;i<g.count;++i)
  if(members[i]!=wanted&&!commit_control(lease,parent,display_members[g.first+i].control,on,getpid(),ownerStart))return false;
 if(read_setting(parent,leaf.c_str())!=wanted)return false;
 for(unsigned i=0;i<g.count;++i)if(read_setting(parent,display_members[g.first+i].control)!=wanted)return false;
 printf("LENS_GROUP\t%u\t%s\n",group,on?"on":"off");return true;
}
// One lease, validate before writes, persist all four OFF intentions before
// fan-out. App also persists pending intent before invoking this transaction.
inline bool display_all_off(const LoadLease &lease,int parent,std::string_view ownerStart) {
 using namespace seed_lens;
 if(!lease.held())return false;
 if(read_setting(parent,gps_control_leaf)==Setting::Invalid)return false;
 for(const auto &g:display_groups){
  auto master=std::string("seed-lens-display-group-")+g.key+".txt";
  if(read_setting(parent,master.c_str())==Setting::Invalid)return false;
 }
 for(const auto &m:display_members)if(read_setting(parent,m.control)==Setting::Invalid)return false;
 if(!commit_control(lease,parent,gps_control_leaf,false,getpid(),ownerStart))return false;
 for(const auto &g:display_groups){
  auto master=std::string("seed-lens-display-group-")+g.key+".txt";
  if(!commit_control(lease,parent,master.c_str(),false,getpid(),ownerStart))return false;
 }
 for(const auto &m:display_members)
  if(!commit_control(lease,parent,m.control,false,getpid(),ownerStart))return false;
 for(unsigned i=0;i<display_groups.size();++i)
  if(!display_setting(lease,parent,i,"read",ownerStart))return false;
 puts("LENS_ALL_OFF_COMMITTED");return true;
}

inline bool gps_setting(const LoadLease& lease,int parent,std::string_view value,std::string_view ownerStart){
 using namespace seed_lens;auto saved=read_setting(parent,gps_control_leaf);
 if(saved==Setting::Invalid)return false;
 if(value!="read"&&value!="on"&&value!="off")return false;
 bool on=value=="read"?saved==Setting::On:value=="on";
 if(value!="read"||saved==Setting::Missing){if(!commit_control(lease,parent,gps_control_leaf,on,getpid(),ownerStart))return false;}
 if(read_setting(parent,gps_control_leaf)!=(on?Setting::On:Setting::Off))return false;
 printf("LENS_GPS\t%s\n",on?"on":"off");return true;
}

}
