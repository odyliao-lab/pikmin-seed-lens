#include "seed_preview.h"
#include "display_contract.h"
#include "extended_display.h"
#include "seed_preview_policy.h"
#include "seed_preview_sha256.h"
#include "managed_gc.h"
#include "And64InlineHook.hpp"
#include "xdl.h"
#include <atomic>
#include <array>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <thread>
#include <fstream>
#include <sstream>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace seed_preview { namespace {
using pikmin::GcHandle;
using pikmin::GcHandleNew;
using pikmin::GcHandleFree;
constexpr size_t max_cards=128;
constexpr const char *control=seed_lens::seed_control;
using Unary=void(*)(void*,void*);
using Binary=void(*)(void*,void*,void*);
struct IndexedData { int index; void *data; };
using IndexedUpdate=void(*)(void*,IndexedData,void*);
std::atomic<Binary> original_detail{},original_text{};
std::atomic<IndexedUpdate> original_inventory{},original_expedition{};
std::atomic<Unary> original_canvas{};
std::atomic<bool> started{},wanted{},hooks_ready{},rendered_enabled{};
std::atomic<const char*> phase{"loading"};
std::atomic<unsigned> decorated_mask{},applied{},restored{},bound_count{};
std::atomic<int> ui_tid{};
std::atomic<unsigned> seed_calls{},text_calls{},canvas_calls{},tracked{},text_matches{};
std::atomic<int> text_tid{},canvas_tid{};
void *(*domain_get)();
const void **(*assemblies)(void*,size_t*);
void *(*image)(const void*);
void *(*klass)(void*,const char*,const char*);
void *(*field)(void*,const char*);
size_t (*offset)(void*);
GcHandleNew weak_new{};
void *(*target)(GcHandle);
GcHandleFree release{};
void *(*new_string)(const char16_t*,int);
void *(*object_class)(void*);
void *(*method_named)(void*,const char*,int);
void *(*invoke)(void*,void*,void**,void**);
void *(*unbox)(void*);
void *card_class{},*text_class{},*data_class{},*item_class{},*raw_class{},*seed_class{},*type_class{};
void *expedition_class{},*detail_class{},*expedition_data_class{},*variation_class{},*provider_class{};
void *seed_getter{},*key_getter{},*name_getter{},*treasure_getter{},*font_getter{},*font_setter{};
bool metadata_checked{},metadata_ok{};
thread_local bool in_setter{};
thread_local void *updating_card{},*updating_data{};
struct Record {
    GcHandle weak{};
    std::u16string source,last,id,name;
    void *method{};
    bool has_source{};
    FontState font;
    int treasure{};
    uint64_t signature{};
};
std::array<Record,max_cards> records;
template<class T> T read(void *p,size_t at) { T v{}; if(p)std::memcpy(&v,static_cast<char*>(p)+at,sizeof v);return v; }
template<class T> void write_ui(void *p,size_t at,T value) {std::memcpy(static_cast<char*>(p)+at,&value,sizeof value);}
void *call(void *method,void *self,void **args=nullptr) {
    if(!method||!self)return nullptr;
    void *exception=nullptr;auto result=invoke(method,self,args,&exception);
    return exception?nullptr:result;
}
bool is(void *p,void *k) { return p&&k&&object_class(p)==k; }
std::u16string string_value(void *p) {
    if(!p)return {};const int n=read<int>(p,16);
    if(n<0||n>2048)return {};return {reinterpret_cast<const char16_t*>(static_cast<char*>(p)+20),static_cast<size_t>(n)};
}
void *find(const char *ns,const char *name) {
    auto d=domain_get();if(!d)return nullptr;size_t n=0;auto all=assemblies(d,&n);
    if(!all||n>4096)return nullptr;
    for(size_t i=0;i<n;++i)if(auto k=klass(image(all[i]),ns,name))return k;
    return nullptr;
}
bool field_is(void *k,const char *name,size_t expected) { auto f=k?field(k,name):nullptr;return f&&offset(f)==expected; }
bool metadata() {
    if(metadata_checked)return metadata_ok;metadata_checked=true;
    card_class=find("Niantic.Ichigo.Game.AnnotatedMap.Panels.PikminOrSeedPicker","PikminOrSeedPickerItem");
    text_class=find("Niantic.Ichigo.UI","IchigoText");
    data_class=find("Niantic.Ichigo.ModelControllers","PikminOrSeed");
    item_class=find("Niantic.Ichigo.Inventory","PikminSeedInventoryItem");
    raw_class=find("Ichigo.Proto","IchigoInventoryItemProto");
    seed_class=find("Ichigo.Proto","PikminSeedProto");type_class=find("Ichigo.Proto","PikminTypeProto");
    expedition_class=find("Niantic.Ichigo.Game.Expedition.ListPanel","ExpeditionListItem");
    detail_class=find("Niantic.Ichigo.Game.Expedition.DetailPanel","ExpeditionDetailPageV2");
    expedition_data_class=find("Niantic.Ichigo.Game.Expedition.Data","ExpeditionItemData");
    variation_class=find("Niantic.Ichigo.ModelControllers.PikminVariations","PikminVariationCatalog");
    provider_class=find("Niantic.Ichigo.Game.Pikmins","PikminPictureBookTrackerV2");
    auto ui_text=find("UnityEngine.UI","Text");
    seed_getter=expedition_data_class?method_named(expedition_data_class,"get_PikminSeed",0):nullptr;
    key_getter=expedition_data_class?method_named(expedition_data_class,"get_Key",0):nullptr;
    name_getter=provider_class?method_named(provider_class,"GetFutureTreasureName",1):nullptr;
    treasure_getter=provider_class?method_named(provider_class,"GetFutureTreasureType",1):nullptr;
    font_getter=ui_text?method_named(ui_text,"get_fontSize",0):nullptr;
    font_setter=ui_text?method_named(ui_text,"set_fontSize",1):nullptr;
    metadata_ok=field_is(card_class,"title",0x40)&&field_is(card_class,"contents",0x100)
        &&field_is(card_class,"pikminVariationCatalog",0xd0)
        &&field_is(text_class,"originalText",0x108)&&field_is(text_class,"autoSize",0x148)
        &&field_is(text_class,"autoSizeRange",0x14c)
        &&field_is(expedition_class,"spawnName",0x70)&&field_is(expedition_class,"currentTask",0xd8)
        &&field_is(detail_class,"titleText",0x28)&&field_is(detail_class,"<Expedition>k__BackingField",0x1c0)
        &&field_is(expedition_data_class,"pikminVariationCatalog",0x58)
        &&field_is(variation_class,"treasureNameProvider",0x28)
        &&field_is(seed_class,"pikmin_",0x68)&&field_is(seed_class,"seedType_",0x18)
        &&field_is(seed_class,"treasureType_",0x90)&&field_is(type_class,"type_",0x18)
        &&field_is(type_class,"fullAssetId_",0x20)&&field_is(type_class,"futureFullAssetId_",0x2c)
        &&field_is(type_class,"textureId_",0x24)&&field_is(type_class,"futureTextureId_",0x30)
        &&data_class&&item_class&&raw_class&&seed_getter&&key_getter&&name_getter&&treasure_getter&&font_getter&&font_setter;
    phase.store(metadata_ok?"ready":"metadata-mismatch");return metadata_ok;
}
struct Binding {void *title{},*type{},*provider{};int colour{},seed_type{},treasure{};uint64_t signature{};std::u16string id;bool inventory{};};
Binding binding(void *card) {
    Binding b;
    if(!card||!read<void*>(card,0x10))return b;
    void *seed=nullptr,*variation=nullptr;
    if(is(card,card_class)) {
        b.inventory=true;
        b.title=read<void*>(card,0x40);
        // Incoming data, not the recycled card's previous contents, is authoritative.
        auto data=card==updating_card?updating_data:read<void*>(card,0x108);
        if(is(data,data_class)) {
            auto item=read<void*>(data,0x18);
            if(is(item,item_class)) {
                auto raw=read<void*>(item,0x10);
                if(is(raw,raw_class)&&read<int>(raw,0x30)==17) {
                    seed=read<void*>(raw,0x28);b.id=string_value(read<void*>(raw,0x18));
                }
            }
        }
        variation=read<void*>(card,0xd0);
    } else if(is(card,expedition_class)||is(card,detail_class)) {
        const bool list=is(card,expedition_class);
        b.title=read<void*>(card,list?0x70:0x28);
        auto data=card==updating_card?updating_data:read<void*>(card,list?0xd8:0x1c0);
        if(is(data,expedition_data_class)) {
            seed=call(seed_getter,data);b.id=string_value(call(key_getter,data));
            variation=read<void*>(data,0x58);
        }
    }
    if(!is(b.title,text_class)||!read<void*>(b.title,0x10))return {};
    if(!is(seed,seed_class))return b;
    auto type=read<void*>(seed,0x68);if(!is(type,type_class))return b;
    b.type=type;b.colour=read<int>(type,0x18);b.seed_type=read<int>(seed,0x18);b.treasure=read<int>(seed,0x90);
    if(is(variation,variation_class)) {auto provider=read<void*>(variation,0x28);if(is(provider,provider_class))b.provider=provider;}
    b.signature=1469598103934665603ULL;
    for(auto at:{0x18,0x1c,0x20,0x24,0x28,0x2c,0x30})b.signature=(b.signature^read<unsigned>(type,at))*1099511628211ULL;
    b.signature=(b.signature^static_cast<unsigned>(b.treasure))*1099511628211ULL;
    return b;
}
Record *record_for(void *text,Binding &b) {
    // Most game text is unrelated. Do not invoke expedition getters for every
    // unrelated label, especially while a list is laying out many recycled rows.
    for(auto &r:records)if(r.weak){auto card=target(r.weak);
        void *title=nullptr;
        if(is(card,card_class))title=read<void*>(card,0x40);
        else if(is(card,expedition_class))title=read<void*>(card,0x70);
        else if(is(card,detail_class))title=read<void*>(card,0x28);
        if(title==text){b=binding(card);if(b.title)return &r;}}
    return nullptr;
}
void font_restore(Record &r,void *title) {
    if(!title)return;
    r.font.restore([&](bool automatic,int minimum,int maximum,int size){
        write_ui(title,0x148,automatic);write_ui(title,0x14c,minimum);write_ui(title,0x150,maximum);
        // IchigoText.AutoSize mutates Text.fontSize, not just its range.
        // Restore via the real setter so layout is dirtied as well.
        void *args[]{&size};call(font_setter,title,args);
    });
}
void resolve_name(Record &r,const Binding &b) {
    r.name.clear();r.treasure=b.treasure;r.signature=b.signature;
    if(b.type&&b.provider) {
        void *args[]{b.type};
        auto boxed=call(treasure_getter,b.provider,args);
        if(boxed) {const int value=*static_cast<int*>(unbox(boxed));if(value>0)r.treasure=value;}
        r.name=string_value(call(name_getter,b.provider,args));
    }
}
std::u16string display_for(const Record &r,const Binding &b,bool enabled) {
    return decorate(r.source,b.seed_type,b.colour,enabled,r.name,r.treasure);
}
void apply(Record &r,const Binding &b,const std::u16string &source,void *method,bool enable) {
    if(!r.has_source||r.id!=b.id||r.signature!=b.signature||r.source!=source)resolve_name(r,b);
    auto display=decorate(source,b.seed_type,b.colour,enable,r.name,r.treasure);
    // New managed strings and the game's own setter maintain the GC and UI queues.
    auto managed=new_string(display.data(),static_cast<int>(display.size()));if(!managed)return;
    if(display!=source) {
        if(!r.font.saved) {
            auto boxed=call(font_getter,b.title);
            const int size=boxed?*static_cast<int*>(unbox(boxed)):0;
            r.font.capture(read<bool>(b.title,0x148),read<int>(b.title,0x14c),read<int>(b.title,0x150),size);
        }
        if(r.font.saved) {write_ui(b.title,0x148,true);write_ui(b.title,0x14c,minimum_font_size(r.font.size,b.inventory));write_ui(b.title,0x150,r.font.size);}
    } else font_restore(r,b.title);
    r.source=source;r.last=display;r.id=b.id;r.method=method;r.has_source=true;
    in_setter=true;original_text.load()(b.title,managed,method);in_setter=false;
    if(display!=source){decorated_mask.fetch_or(1u<<(b.colour-1));applied.fetch_add(1);}else restored.fetch_add(1);
}
void text_hook(void *self,void *value,void *method) {
    text_calls.fetch_add(1);text_tid.store(gettid());
    if(!hooks_ready.load()||gettid()!=ui_tid.load()||in_setter||!metadata_ok){original_text.load()(self,value,method);return;}
    Binding b;auto r=record_for(self,b);
    if(!r){original_text.load()(self,value,method);return;}text_matches.fetch_add(1);
    if(!value||read<int>(value,16)<0||read<int>(value,16)>2048){font_restore(*r,self);r->has_source=false;original_text.load()(self,value,method);return;}
    auto source=string_value(value);
    // Layout may resubmit our displayed string. Preserve the unmodified rich text.
    if(r->has_source&&source==r->last)source=r->source;
    apply(*r,b,source,method,wanted.load());
}
void begin_update(void *self,void *data) {
    seed_calls.fetch_add(1);
    int expected=0;ui_tid.compare_exchange_strong(expected,gettid());
    const bool on_ui=gettid()==ui_tid.load();
    if(on_ui){updating_card=self;updating_data=data;}
    if(hooks_ready.load()&&gettid()==ui_tid.load()&&metadata()){
            Record *slot=nullptr;
            for(auto &r:records){auto card=r.weak?target(r.weak):nullptr;
                if(card==self){slot=&r;break;}
                if(!card&&!slot)slot=&r;
            }
            if(slot){
                if(!slot->weak||target(slot->weak)!=self){if(slot->weak)release(slot->weak);*slot={};slot->weak=weak_new(self,false);}
                if(slot->weak)tracked.fetch_add(1);
                // Remove the previous binding's decoration before the original asynchronous refresh.
                auto b=binding(self);
                if(slot->has_source&&(slot->id!=b.id||b.seed_type==0)){
                    font_restore(*slot,b.title);
                    if(b.title&&string_value(read<void*>(b.title,0x108))==slot->last){
                        apply(*slot,b,slot->source,slot->method,false);
                    }
                    slot->has_source=false;
                }
            }
    }
}
struct UpdateScope {
    void *card=updating_card,*data=updating_data;
    UpdateScope(void *self,void *incoming){begin_update(self,incoming);}
    ~UpdateScope(){updating_card=card;updating_data=data;}
};
void inventory_hook(void *self,IndexedData data,void *method) {
    UpdateScope scope(self,data.data);original_inventory.load()(self,data,method);
}
void expedition_hook(void *self,IndexedData data,void *method) {
    UpdateScope scope(self,data.data);original_expedition.load()(self,data,method);
}
void detail_hook(void *self,void *data,void *method) {
    UpdateScope scope(self,data);original_detail.load()(self,data,method);
}
void canvas_hook(void *self,void *method) {
    canvas_calls.fetch_add(1);canvas_tid.store(gettid());
    if(hooks_ready.load()&&gettid()==ui_tid.load()&&metadata_ok){
        const bool enabled=wanted.load();
        unsigned alive=0;
        for(auto &r:records)if(r.weak){
            auto card=target(r.weak);auto b=binding(card);
            if(!b.title){release(r.weak);r={};continue;}++alive;
            if(!r.has_source)continue;
            auto current=string_value(read<void*>(b.title,0x108));
            // A reused card must never inherit another seedling's stored title.
            if(b.id!=r.id||b.seed_type==0){font_restore(r,b.title);r.has_source=false;continue;}
            if(current!=r.last){r.source=current;r.last=current;}
            if(b.signature!=r.signature)resolve_name(r,b);
            if(display_for(r,b,enabled)!=current)apply(r,b,r.source,r.method,enabled);
        }
        bound_count.store(alive);
        rendered_enabled.store(enabled);
    }
    original_canvas.load()(self,method);
}
template<class F> bool hook(unsigned char *base,size_t rva,void *replacement,std::atomic<F>&original) {
    void *storage=mmap(nullptr,4096,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(storage==MAP_FAILED)return false;
    original.store(reinterpret_cast<F>(storage),std::memory_order_release);
    // Trampoline is published before the entry patch, so translated callbacks never spin.
    return A64HookFunctionV(base+rva,replacement,storage,4096)!=nullptr;
}
bool control_on() {
    int fd=open(control,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);if(fd<0)return false;
    struct stat st{};char b[8]{};ssize_t n=-1;
    if(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_uid==0&&(st.st_mode&0022)==0&&st.st_size<=7)n=::read(fd,b,7);
    close(fd);return n>0&&enabled_control({b,static_cast<size_t>(n)});
}
void worker(std::string directory) {
    std::ifstream process("/proc/self/stat");std::string statLine,start;
    std::getline(process,statLine);auto close=statLine.rfind(')');
    if(close==statLine.npos)return;
    std::istringstream fields(statLine.substr(close+2));
    for(int i=3;i<=22;++i)if(!(fields>>start))return;
    if(start.empty()||start.find_first_not_of("0123456789")!=start.npos)return;
    void *lib=nullptr;for(int i=0;i<180&&!lib;++i){lib=xdl_open("libil2cpp.so",XDL_DEFAULT);if(!lib)sleep(1);}
    xdl_info_t info{};
    bool valid=lib&&xdl_info(lib,XDL_DI_DLINFO,&info)==0&&info.dli_fbase&&info.dli_fname
        &&static_cast<Elf64_Ehdr*>(info.dli_fbase)->e_machine==EM_AARCH64;
    const auto* profile=valid?verified_game_profile(info.dli_fname):nullptr;
    valid=valid&&profile&&profile->layout==1;
    // Only exact reviewed Game157 receives the additional display hooks.
    // Older profiles retain the independently verified original renderer.
    if(valid&&profile==&game_profiles[2]){seed_extended::start(directory.c_str());return;}
    auto base=static_cast<unsigned char*>(info.dli_fbase);
    if(valid){
        for(const auto* entry:{&profile->inventory,&profile->text,&profile->canvas,&profile->expedition,&profile->detail})
            valid=valid&&!memcmp(base+entry->rva,entry->prefix.data(),sizeof(entry->prefix));
        // Both reviewed text entry points are a B +4 veneer. Hook the body,
        // including callers which skip that veneer; verify both bytes/paths.
        uint32_t veneer=0;memcpy(&veneer,base+profile->text.rva-4,sizeof veneer);
        valid=valid&&veneer==0x14000001;
    }
#define RESOLVE(var,name) var=reinterpret_cast<decltype(var)>(xdl_sym(lib,name,nullptr));valid=valid&&var
    if(valid){
        RESOLVE(domain_get,"il2cpp_domain_get");RESOLVE(assemblies,"il2cpp_domain_get_assemblies");
        RESOLVE(image,"il2cpp_assembly_get_image");RESOLVE(klass,"il2cpp_class_from_name");
        RESOLVE(field,"il2cpp_class_get_field_from_name");RESOLVE(offset,"il2cpp_field_get_offset");
        RESOLVE(weak_new,"il2cpp_gchandle_new_weakref");RESOLVE(target,"il2cpp_gchandle_get_target");
        RESOLVE(release,"il2cpp_gchandle_free");RESOLVE(new_string,"il2cpp_string_new_utf16");
        RESOLVE(object_class,"il2cpp_object_get_class");
        RESOLVE(method_named,"il2cpp_class_get_method_from_name");RESOLVE(invoke,"il2cpp_runtime_invoke");RESOLVE(unbox,"il2cpp_object_unbox");
    }
#undef RESOLVE
    if(valid){valid=hook(base,profile->text.rva,reinterpret_cast<void*>(text_hook),original_text)
        &&hook(base,profile->canvas.rva,reinterpret_cast<void*>(canvas_hook),original_canvas)
        &&hook(base,profile->inventory.rva,reinterpret_cast<void*>(inventory_hook),original_inventory)
        &&hook(base,profile->expedition.rva,reinterpret_cast<void*>(expedition_hook),original_expedition)
        &&hook(base,profile->detail.rva,reinterpret_cast<void*>(detail_hook),original_detail);}
    phase.store(valid?"waiting-cards":"incompatible");hooks_ready.store(valid);
    const auto path=directory+"/files/"+seed_lens::seed_report_leaf,tmp=path+".tmp";
    for(;;){
        wanted.store(valid&&control_on());
        auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        FILE *f=fopen(tmp.c_str(),"w");if(f){bool ok=fprintf(f,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t%u\t%u\t%u\t%s\n",
            static_cast<long long>(now),getpid(),profile?profile->version:"unsupported",phase.load(),(!strcmp(phase.load(),"ready")?rendered_enabled.load():wanted.load())?1:0,bound_count.load(),decorated_mask.load(),applied.load(),restored.load(),start.c_str())>0;
            if(fclose(f))ok=false;if(ok){chmod(tmp.c_str(),0644);rename(tmp.c_str(),path.c_str());}}
        FILE *diag=fopen((directory+"/files/seed_lens_seed_preview_diagnostic.tsv").c_str(),"w");
        if(diag){fprintf(diag,"%u\t%u\t%u\t%u\t%u\t%d\t%d\t%d\n",seed_calls.load(),text_calls.load(),canvas_calls.load(),tracked.load(),text_matches.load(),ui_tid.load(),text_tid.load(),canvas_tid.load());fclose(diag);}
        sleep(1);
    }
}
} // namespace
void start(const char *directory) {
    if(!directory||started.exchange(true))return;
    struct stat st{};
    if(getuid()/100000!=0||lstat(directory,&st)||!S_ISDIR(st.st_mode)||st.st_uid!=getuid())return;
    std::thread(worker,std::string(directory)).detach();
}
} // namespace seed_preview
