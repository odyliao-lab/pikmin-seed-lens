// Standalone, display-only context. No core globals, RPCs or exported TSV reads.
namespace standalone_context {
using Root=pikmin::ScopedManagedRoot;
using Login=void(*)(void*,void*,void*,void*,void*);
std::atomic<Login> original_login{};
std::atomic<Unary> original_logout{};
std::atomic<uint64_t> epoch{1};
std::atomic<bool> suspended{};
std::mutex account_mutex;
std::string expected_account;
std::atomic<void*> string_class{};
GcHandle player_weak{};
uint64_t player_epoch{};
std::chrono::steady_clock::time_point searched{};
void *player_class{},*info_class{},*inventory_class{},*get_info{},*lookup{};
void *array_class{},*array_get{},*array_length{},*array_rank{};
const void *(*class_type)(void*);
void *(*type_object)(const void*);
bool checked{},compatible{};

std::string digest_id(void *value) {
 auto klass=string_class.load();if(!klass){klass=find("System","String");if(klass)string_class.store(klass);}
 if(!is(value,klass))return {};
 auto id=string_value(value);if(id.empty()||id.size()>256)return {};
 std::string canonical;for(auto c:id){canonical+=static_cast<char>(c&255);canonical+=static_cast<char>(c>>8);}
 return pikmin::account_digest(canonical);
}
void logout(void *self,void *method) {
 suspended.store(true);epoch.fetch_add(1);
 {std::lock_guard<std::mutex> lock(account_mutex);expected_account.clear();}
 original_logout.load()(self,method);
}
void login(void *self,void *server,void *player,void *token,void *method) {
 suspended.store(true);const auto current=epoch.fetch_add(1)+1;
 Root root(player,strong_new,release);
 auto digest=root.handle?digest_id(player):std::string{};
 original_login.load()(self,server,player,token,method);
 std::lock_guard<std::mutex> lock(account_mutex);
 if(epoch.load()==current){expected_account=digest;suspended.store(digest.empty());}
}
bool configure(void *lib,unsigned char *base) {
 class_type=reinterpret_cast<decltype(class_type)>(xdl_sym(lib,"il2cpp_class_get_type",nullptr));
 type_object=reinterpret_cast<decltype(type_object)>(xdl_sym(lib,"il2cpp_type_get_object",nullptr));
 if(!class_type||!type_object
   ||memcmp(base+seed_lens::login_rva,seed_lens::login_prefix.data(),24)
   ||memcmp(base+seed_lens::logout_rva,seed_lens::logout_prefix.data(),24))return false;
 return hook(base,seed_lens::logout_rva,reinterpret_cast<void*>(logout),original_logout)
     &&hook(base,seed_lens::login_rva,reinterpret_cast<void*>(login),original_login);
}
bool setup() {
 if(checked)return compatible;checked=true;
 player_class=find("Niantic.Ichigo.Player","PlayerManager");
 info_class=find("Niantic.Ichigo.Player","PlayerInfo");
 inventory_class=find("Niantic.Ichigo.Inventory","InventoryManager");
 auto object=find("UnityEngine","Object"),type=find("System","Type"),integer=find("System","Int32");
 array_class=find("System","Array");
 auto &api=expedition_display_adapter::methods;
 if(!api||!object||!type||!integer||!array_class)return false;
 void *iterator=nullptr;unsigned count=0;
 while(auto m=api(object,&iterator)){
  if(++count>4096)return false;
  if(!strcmp(expedition_display_adapter::method_name(m),"FindObjectsOfType")
     &&expedition_display_adapter::param_count(m)==1
     &&expedition_display_adapter::from_type(expedition_display_adapter::param_type(m,0))==type){
   if(lookup)return false;lookup=m;
  }
 }
 iterator=nullptr;count=0;
 while(auto m=api(array_class,&iterator)){
  if(++count>4096)return false;
  if(!strcmp(expedition_display_adapter::method_name(m),"GetValue")
     &&expedition_display_adapter::param_count(m)==1
     &&expedition_display_adapter::from_type(expedition_display_adapter::param_type(m,0))==integer){
   if(array_get)return false;array_get=m;
  }
 }
 array_length=method_named(array_class,"GetLength",1);array_rank=method_named(array_class,"get_Rank",0);
 get_info=player_class?method_named(player_class,"get_PlayerInfo",0):nullptr;
 compatible=lookup&&array_get&&array_length&&array_rank&&get_info
   &&field_is(player_class,"inventoryManager",0x28)&&field_is(info_class,"PlayerId",0x10)
   &&field_is(inventory_class,"latestServerSideTimestamp",display_game.inventory_revision)
   &&field_is(inventory_class,"itemsById",display_game.inventory_items);
 return compatible;
}
void *player(uint64_t current) {
 if(player_epoch!=current){if(player_weak)release(player_weak);player_weak=0;player_epoch=current;searched={};}
 auto value=player_weak?target(player_weak):nullptr;
 if(is(value,player_class)&&read<void*>(value,0x10))return value;
 if(player_weak){release(player_weak);player_weak=0;}
 const auto now=std::chrono::steady_clock::now();if(now-searched<std::chrono::seconds(2))return nullptr;searched=now;
 auto type=type_object(class_type(player_class));Root type_root(type,strong_new,release);
 if(!type_root.handle)return nullptr;
 void *args[]{type},*exception=nullptr;
 auto array=invoke(lookup,nullptr,args,&exception);Root array_root(exception?nullptr:array,strong_new,release);
 if(!array_root.handle||!assignable(array_class,object_class(array)))return nullptr;
 int rank=-1,length=-1,zero=0;void *dimension[]{&zero};
 if(!collection_adapter::number(call(array_rank,array),rank)||rank!=1
   ||!collection_adapter::number(call(array_length,array,dimension),length)||length!=1)return nullptr;
 auto candidate=call(array_get,array,dimension);
 if(!is(candidate,player_class)||!read<void*>(candidate,0x10))return nullptr;
 player_weak=weak_new(candidate,false);return player_weak?candidate:nullptr;
}
InsightContext read_context() {
 if(gettid()!=ui_tid.load()||suspended.load()||!setup())return {};
 const auto current=epoch.load();auto owner=player(current);Root owner_root(owner,strong_new,release);
 if(!owner_root.handle)return {};
 auto inventory=read<void*>(owner,0x28);Root inventory_root(inventory,strong_new,release);
 if(!is(inventory,inventory_class)||!collection_adapter::ready(inventory))return {};
 const auto revision=read<int64_t>(inventory,display_game.inventory_revision);if(revision<=0)return {};
 auto info=call(get_info,owner);Root info_root(info,strong_new,release);if(!is(info,info_class))return {};
 const auto account=digest_id(read<void*>(info,0x10));if(account.empty())return {};
 {std::lock_guard<std::mutex> lock(account_mutex);if(!expected_account.empty()&&account!=expected_account)return {};}
 auto after=call(get_info,owner);Root after_root(after,strong_new,release);
 if(!is(after,info_class)||account!=digest_id(read<void*>(after,0x10))||epoch.load()!=current||suspended.load()
    ||read<void*>(owner,0x28)!=inventory||read<int64_t>(inventory,display_game.inventory_revision)!=revision)return {};
 return {std::to_string(getpid())+":"+std::to_string(current)+":"+account,inventory};
}
}
