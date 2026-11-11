#pragma once
#include "control_transaction.h"
#include "display_groups.h"

namespace seed_root {
inline seed_lens::Setting read_setting(int parent,const char *leaf) {
 int fd=openat(parent,leaf,O_RDONLY|O_CLOEXEC|O_NOFOLLOW|O_NONBLOCK);
 if(fd<0)return errno==ENOENT?seed_lens::Setting::Missing:seed_lens::Setting::Invalid;
 struct stat st{};char bytes[8]{};ssize_t count=-1;
 if(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_uid==0&&st.st_nlink==1&&!(st.st_mode&0022)&&st.st_size>0&&st.st_size<8)
  count=read(fd,bytes,sizeof(bytes));
 close(fd);return count>0?seed_lens::parse_setting({bytes,static_cast<size_t>(count)}):seed_lens::Setting::Invalid;
}
// Persist master first. Retrying interrupted OFF never imports a remaining ON.
inline bool display_setting(const LoadLease &lease,int parent,unsigned group,std::string_view explicitValue,std::string_view ownerStart) {
 using namespace seed_lens;if(group>=display_groups.size())return false;
 const auto &g=display_groups[group];Setting members[3]{};
 for(unsigned i=0;i<g.count;++i)members[i]=read_setting(parent,display_members[g.first+i].control);
 auto leaf=std::string("seed-lens-display-group-")+g.key+".txt";
 const auto master=read_setting(parent,leaf.c_str());
 auto wanted=reconcile(master,members,g.count);if(wanted==Setting::Invalid)return false;
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
}
