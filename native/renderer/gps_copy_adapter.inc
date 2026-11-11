// Selective adaptation of control-center 8abb7ee; no shared files or loader.
bool control_on(const char *path);
namespace gps_adapter {
std::array<GcHandle,8> pages{};
seed_lens::gps::Selection selection;
std::string owner;
void *active_getter{};bool checked{},compatible{},overflow{};
std::atomic<bool> enabled{},ack{},metadata_ready{};
std::atomic<unsigned> copies{},failures{},state{};
int32_t (*value_size)(void*,uint32_t*);
void clear(){for(auto &h:pages)if(h){release(h);h=0;}selection.cancel();overflow=false;}
std::string session(){auto reader=context_reader.load();return reader?reader().session:std::string{};}
bool context(){auto current=session();if(current.empty()){clear();owner.clear();state.store(3);return false;}if(current!=owner){clear();owner=std::move(current);}return true;}
bool metadata(){
 if(checked)return compatible;checked=true;
 auto behaviour=find("UnityEngine","Behaviour");active_getter=behaviour?method_named(behaviour,"get_isActiveAndEnabled",0):nullptr;
 compatible=active_getter&&key_getter&&value_size&&field_is(detail_class,"isFocused",0x1c8)&&field_is(detail_class,"<Expedition>k__BackingField",0x1c0);
 metadata_ready.store(compatible);return compatible;
}
void track(void *page){
 if(!metadata()||!is(page,detail_class)||!context())return;
 for(auto h:pages)if(h&&target(h)==page)return;
 for(auto &h:pages)if(!h||!target(h)){if(h)release(h);h=weak_new(page,false);return;}
 overflow=true;selection.cancel();state.store(5);
}
void tick(){
 bool on=enabled.load()&&control_on(seed_lens::gps_control);ack.store(on);
 if(!on){selection.cancel();state.store(0);return;}
 if(!metadata()||!context()||overflow){state.store(3);return;}
 void *page=nullptr;unsigned count=0;
 for(auto &h:pages)if(h){auto p=target(h);if(!p){release(h);h=0;continue;}
  if(!read<bool>(p,0x1c8))continue;auto active=call(active_getter,p);auto v=active?unbox(active):nullptr;
  if(v&&read<bool>(v,0)){page=p;++count;}}
 auto data=count==1?read<void*>(page,0x1c0):nullptr;
 auto key=is(data,expedition_data_class)?string_value(call(key_getter,data)):std::u16string{};
 auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
 if(count!=1||key.empty())state.store(4);else if(!selection.same(owner,key))state.store(1);
 if(!selection.observe(owner,key,now,count==1&&!key.empty()))return;
 // Weak handles identify pages, strong roots protect the current data this tick.
 auto pageRoot=strong_new(page,false),dataRoot=strong_new(data,false);
 struct Roots{GcHandle p,d;~Roots(){if(d)release(d);if(p)release(p);}} roots{pageRoot,dataRoot};
 if(!pageRoot||!dataRoot){selection.attempted(now,false);failures.fetch_add(1);state.store(3);return;}
 bool copied=false;
 auto method=method_named(object_class(data),"get_SpawnLocation",0);
 auto boxed=call(method,data);uint32_t alignment=0;
 auto raw=boxed&&value_type(object_class(boxed))&&value_size(object_class(boxed),&alignment)==16?unbox(boxed):nullptr;
 if(raw){double lat=read<double>(raw,0),lng=read<double>(raw,8);
  if(seed_lens::gps::coordinate(lat,lng)&&session()==owner&&target(pageRoot)==page&&target(dataRoot)==data
      &&read<void*>(page,0x1c0)==data&&string_value(call(key_getter,data))==key&&read<bool>(page,0x1c8)
      &&enabled.load()&&control_on(seed_lens::gps_control)){
   char coordinates[64];snprintf(coordinates,sizeof coordinates,"%.7f,%.7f",lat,lng);
   copied=seed_lens::clipboard_copy(coordinates);
  }
 }
 selection.attempted(now,copied);if(copied){copies.fetch_add(1);state.store(2);}else{failures.fetch_add(1);state.store(3);}
}
}
