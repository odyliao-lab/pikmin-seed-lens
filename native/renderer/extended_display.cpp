#include "extended_display.h"
#include "extended_display_profile.h"
#include "display_contract.h"
#include "display_groups.h"
#include "standalone_context_profile.h"
#include <mutex>
#include "extended_display_policy.h"
#include "seed_preview_sha256.h"
#include "collection_insight_policy.h"
#include "stock_display_policy.h"
#include "adult_display_policy.h"
#include "decoration_display_policy.h"
#include "gift_display_policy.h"
#include "planter_display_policy.h"
#include "expedition_display_policy.h"
#include "feeding_catalogue.h"
#include "managed_gc.h"
#include "And64InlineHook.hpp"
#include "xdl.h"
#include <atomic>
#include <array>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <initializer_list>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <thread>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "session_identity.h"

namespace seed_extended { namespace {
using pikmin::GcHandle;
using pikmin::GcHandleNew;
using pikmin::GcHandleFree;
constexpr size_t max_cards=128;
constexpr const char *control=seed_lens::seed_control;
using Unary=void(*)(void*,void*);
using Binary=void(*)(void*,void*,void*);
struct IndexedData { int index; void *data; };
using IndexedUpdate=void(*)(void*,IndexedData,void*);
using AsyncUpdate=void*(*)(void*,void*,void*);
std::atomic<AsyncUpdate> original_extract{};
std::atomic<Binary> original_detail{},original_text{};
std::atomic<Binary> original_honey{},original_adult{};
std::atomic<Binary> original_planted_detail{};
std::atomic<IndexedUpdate> original_inventory{},original_expedition{};
std::atomic<Unary> original_canvas{};
std::atomic<bool> started{},wanted{},hooks_ready{},rendered_enabled{};
std::atomic<bool> collection_wanted{},collection_rendered{};
std::atomic<const char*> collection_phase{"waiting-cards"};
std::atomic<bool> stock_wanted{},stock_rendered{};
std::atomic<bool> adult_wanted{},adult_rendered{};
std::atomic<const char*> adult_phase{"waiting-cards"};
std::atomic<bool> gift_wanted{},gift_rendered{};
std::atomic<const char*> gift_phase{"waiting-cards"};
std::atomic<bool> planter_display_wanted{},planter_display_rendered{};
std::atomic<const char*> planter_display_phase{"waiting-cards"};
std::atomic<bool> expedition_display_wanted{},expedition_display_rendered{};
std::atomic<const char*> expedition_display_phase{"waiting-cards"};
std::atomic<bool> decoration_wanted{},decoration_rendered{};
std::atomic<const char*> decoration_phase{"waiting-cards"};
std::atomic<const char*> stock_phase{"waiting-cards"};
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
GcHandleNew strong_new{};
std::atomic<ContextReader> context_reader{};
void *(*target)(GcHandle);
GcHandleFree release{};
void *(*new_string)(const char16_t*,int);
void *(*object_class)(void*);
bool (*assignable)(void*,void*);
bool (*value_type)(void*);
void *(*method_named)(void*,const char*,int);
void *(*invoke)(void*,void*,void**,void**);
void *(*unbox)(void*);
void *card_class{},*text_class{},*data_class{},*item_class{},*raw_class{},*seed_class{},*type_class{};
void *expedition_class{},*detail_class{},*expedition_data_class{},*variation_class{},*provider_class{};
void *seed_getter{},*key_getter{},*name_getter{},*treasure_getter{},*font_getter{},*font_setter{};
void *raw_text_setter{},*horizontal_getter{},*horizontal_setter{};
void *overflow_getter{},*overflow_setter{},*alignment_getter{},*alignment_setter{};
void *honey_card_class{},*honey_data_class{},*honey_item_class{},*honey_proto_class{};
void *extract_card_class{},*petal_card_class{};
bool honey_metadata{},extract_metadata{},adult_card_metadata{};
void *adult_card_class{};
void *planted_detail_class{};
bool planted_detail_metadata{};
bool metadata_checked{},metadata_ok{};
thread_local bool in_setter{};
thread_local void *updating_card{},*updating_data{};
struct Record {
    GcHandle weak{},adult_item_weak{},footer_text_weak{};
    bool footer{};int footer_index{};
    std::chrono::steady_clock::time_point footer_scan{};
    std::u16string adult_id;
    std::u16string source,last,id,name;
    void *method{};
    bool has_source{};
    FontState font;
    TextLayoutState layout;
    bool ellipsis{},line_break{};
    int horizontal{};
    int treasure{};
    uint64_t signature{};
};
std::array<Record,max_cards> records;
void reset_record(Record &r) {
    if(r.weak)release(r.weak);if(r.adult_item_weak)release(r.adult_item_weak);if(r.footer_text_weak)release(r.footer_text_weak);r={};
}

template<class T> T read(void *p,size_t at) { T v{}; if(p)std::memcpy(&v,static_cast<char*>(p)+at,sizeof v);return v; }
template<class T> void write_ui(void *p,size_t at,T value) {std::memcpy(static_cast<char*>(p)+at,&value,sizeof value);}
void *call(void *method,void *self,void **args=nullptr) {
    if(!method||!self)return nullptr;
    void *exception=nullptr;auto result=invoke(method,self,args,&exception);
    return exception?nullptr:result;
}
bool is(void *p,void *k) { return p&&k&&object_class(p)==k; }
bool adult_card(void *p) {return p&&adult_card_metadata&&assignable(adult_card_class,object_class(p));}
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
    honey_card_class=find("Niantic.Ichigo.Game.Garden.Extracts.Scroll","HoneyBallEntry");
    honey_data_class=find("Niantic.Ichigo.Game.Garden.Extracts.Scroll","HoneyBallData");
    honey_item_class=find("Niantic.Ichigo.Inventory","HoneyBallInventoryItem");
    honey_proto_class=find("Ichigo.Proto","HoneyBallProto");
    extract_card_class=find("Niantic.Ichigo.Game.Garden.Extracts","ExtractBallEntry");
    petal_card_class=find("Niantic.Ichigo.Game.Garden.Extracts","PossessedPetal");
    extract_metadata=field_is(extract_card_class,"possessedPetal",0x48)
        &&field_is(extract_card_class,"honeyBallProto",0x68)
        &&field_is(petal_card_class,"count",0x30)&&honey_item_class&&honey_proto_class;
    honey_metadata=field_is(honey_card_class,"countText",0x58)&&field_is(honey_card_class,"honeyBallData",0x68)
        &&field_is(honey_data_class,"IsMoreBall",0x10)&&field_is(honey_data_class,"EntryData",0x18)
        &&honey_item_class&&honey_proto_class;
    adult_card_class=find("Niantic.Ichigo.Game.PikminManagementV2.Entries","PikminListEntry");
    adult_card_metadata=display_game.adult.rva&&assignable
        &&field_is(adult_card_class,"NameText",0x48)
        &&field_is(adult_card_class,"PikminVariationCatalog",0x98);
    planted_detail_class=find("Niantic.Ichigo.Game.PikminOrSeedDetailsView","PikminOrSeedDetailsViewTitle");
    planted_detail_metadata=display_game.planted_detail.rva
        &&field_is(planted_detail_class,"title",0x50)&&field_is(planted_detail_class,"seedSteps",0x60)
        &&field_is(planted_detail_class,"pikminVariationCatalog",0x80)
        &&field_is(planted_detail_class,"inventoryManager",0x98)&&field_is(planted_detail_class,"pikminOrSeed",0xa8);
    auto ui_text=find("UnityEngine.UI","Text");
    seed_getter=expedition_data_class?method_named(expedition_data_class,"get_PikminSeed",0):nullptr;
    key_getter=expedition_data_class?method_named(expedition_data_class,"get_Key",0):nullptr;
    name_getter=provider_class?method_named(provider_class,"GetFutureTreasureName",1):nullptr;
    treasure_getter=provider_class?method_named(provider_class,"GetFutureTreasureType",1):nullptr;
    raw_text_setter=ui_text?method_named(ui_text,"set_text",1):nullptr;
    font_getter=ui_text?method_named(ui_text,"get_fontSize",0):nullptr;
    font_setter=ui_text?method_named(ui_text,"set_fontSize",1):nullptr;
    horizontal_getter=ui_text?method_named(ui_text,"get_horizontalOverflow",0):nullptr;
    horizontal_setter=ui_text?method_named(ui_text,"set_horizontalOverflow",1):nullptr;
    overflow_getter=ui_text?method_named(ui_text,"get_verticalOverflow",0):nullptr;
    overflow_setter=ui_text?method_named(ui_text,"set_verticalOverflow",1):nullptr;
    alignment_getter=ui_text?method_named(ui_text,"get_alignment",0):nullptr;
    alignment_setter=ui_text?method_named(ui_text,"set_alignment",1):nullptr;
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
        &&data_class&&item_class&&raw_class&&seed_getter&&key_getter&&name_getter&&treasure_getter&&font_getter&&font_setter
        &&field_is(text_class,"useEllipsis",0x13c)&&field_is(text_class,"useLineBreakRules",0x13d)
        &&raw_text_setter&&horizontal_getter&&horizontal_setter&&overflow_getter&&overflow_setter&&alignment_getter&&alignment_setter;
    phase.store(metadata_ok?"ready":"metadata-mismatch");return metadata_ok;
}
struct Binding {void *title{},*type{},*provider{},*nectar_item{},*adult_item{},*expedition_data{},*seed_item{};bool nectar{},adult{},planted_detail{},expedition_footer{};int colour{},seed_type{},treasure{};uint64_t signature{};std::u16string id;};
#include "collection_insight_adapter.inc"
#include "stock_display_adapter.inc"
#include "adult_display_adapter.inc"
#include "decoration_display_adapter.inc"
#include "gift_display_adapter.inc"
#include "planter_display_adapter.inc"
#include "expedition_display_adapter.inc"
Binding binding(void *card) {
    Binding b;
    if(!card||!read<void*>(card,0x10))return b;
    if(extract_metadata&&is(card,extract_card_class)) {
        auto petal=read<void*>(card,0x48);if(!is(petal,petal_card_class))return b;
        b.title=read<void*>(petal,0x30);
        if(!is(b.title,text_class)||!read<void*>(b.title,0x10))return {};
        auto item=card==updating_card?updating_data:read<void*>(card,0x68);
        if(!is(item,honey_item_class))return b;
        b.id=string_value(collection_adapter::get(item,"get_Id"));
        auto proto=collection_adapter::get(item,"get_Proto");int colour=-1;
        if(!is(proto,honey_proto_class)||!collection_adapter::number(collection_adapter::get(proto,"get_HoneyType"),colour)
                ||colour<1||colour>4||b.id.empty())return b;
        b.nectar=true;b.nectar_item=item;b.signature=static_cast<uint64_t>(colour);return b;
    }
    if(honey_metadata&&is(card,honey_card_class)) {
        b.title=read<void*>(card,0x58);
        if(!is(b.title,text_class)||!read<void*>(b.title,0x10))return {};
        auto data=card==updating_card?updating_data:read<void*>(card,0x68);
        if(!is(data,honey_data_class)||read<bool>(data,0x10))return b;
        auto entry=read<void*>(data,0x18);
        if(!entry||!field_is(object_class(entry),"<HoneyBallItem>k__BackingField",0x10))return b;
        auto item=read<void*>(entry,0x10);if(!is(item,honey_item_class))return b;
        auto id=collection_adapter::get(item,"get_Id");b.id=string_value(id);
        auto proto=collection_adapter::get(item,"get_Proto");
        int colour=-1;if(!is(proto,honey_proto_class)||!collection_adapter::number(collection_adapter::get(proto,"get_HoneyType"),colour)
                ||colour<1||colour>4||b.id.empty())return b;
        b.nectar=true;b.nectar_item=item;b.signature=static_cast<uint64_t>(colour);return b;
    }
    if(adult_card(card)) {
        b.title=read<void*>(card,0x48);b.adult=true;
        if(!is(b.title,text_class)||!read<void*>(b.title,0x10))return {};
        if(card==updating_card)b.adult_item=updating_data;
        else for(auto &r:records)if(r.weak&&target(r.weak)==card) {
            b.adult_item=r.adult_item_weak?target(r.adult_item_weak):nullptr;b.id=r.adult_id;break;
        }
        if(b.adult_item&&adult_adapter::setup()&&is(b.adult_item,collection_adapter::adult_item_class))
            b.id=string_value(collection_adapter::get(b.adult_item,"get_Id"));
        else b.adult_item=nullptr;
        auto variation=read<void*>(card,0x98);
        if(is(variation,variation_class)) {
            auto provider=read<void*>(variation,0x28);
            if(is(provider,provider_class))b.provider=provider;
        }
        return b;
    }
    void *seed=nullptr,*variation=nullptr;
    if(planted_detail_metadata&&is(card,planted_detail_class)) {
        // The title button sizes itself to the original name. Its narrow
        // rectangle cannot host a multiline subtitle; use the wide steps row.
        b.title=read<void*>(card,0x60);b.planted_detail=true;
        auto data=card==updating_card?updating_data:read<void*>(card,0xa8);
        if(is(data,data_class)) {
            auto item=read<void*>(data,0x18);
            if(is(item,item_class)) {
                b.seed_item=item;b.id=string_value(collection_adapter::get(item,"get_Id"));
                seed=collection_adapter::get(item,"get_Proto");
            }
        }
        variation=read<void*>(card,0x80);
    } else if(is(card,card_class)) {
        b.title=read<void*>(card,0x40);
        // Incoming data, not the recycled card's previous contents, is authoritative.
        auto data=card==updating_card?updating_data:read<void*>(card,0x108);
        if(is(data,data_class)) {
            if(adult_adapter::setup()) {
                auto adult=read<void*>(data,0x10);
                if(is(adult,collection_adapter::adult_item_class)) {
                    b.adult_item=adult;b.adult=true;
                    b.id=string_value(collection_adapter::get(adult,"get_Id"));
                }
            }
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
            b.expedition_data=data;
            seed=call(seed_getter,data);b.id=string_value(call(key_getter,data));
            variation=read<void*>(data,0x58);
            if(is(variation,variation_class)){auto provider=read<void*>(variation,0x28);if(is(provider,provider_class))b.provider=provider;}
        }
    }
    if(!is(b.title,text_class)||!read<void*>(b.title,0x10))return {};
    if(b.adult&&is(variation,variation_class)) {
        auto provider=read<void*>(variation,0x28);if(is(provider,provider_class))b.provider=provider;
    }
    if(!is(seed,seed_class))return b;
    auto type=read<void*>(seed,0x68);if(!is(type,type_class))return b;
    b.type=type;b.colour=read<int>(type,0x18);b.seed_type=read<int>(seed,0x18);b.treasure=read<int>(seed,0x90);
    if(is(variation,variation_class)) {auto provider=read<void*>(variation,0x28);if(is(provider,provider_class))b.provider=provider;}
    b.signature=1469598103934665603ULL;
    for(auto at:{0x18,0x1c,0x20,0x24,0x28,0x2c,0x30})b.signature=(b.signature^read<unsigned>(type,at))*1099511628211ULL;
    b.signature=(b.signature^static_cast<unsigned>(b.treasure))*1099511628211ULL;
    return b;
}
Binding record_binding(Record &r){
    auto card=r.weak?target(r.weak):nullptr;auto b=binding(card);
    if(r.footer){
        auto label=r.footer_text_weak?target(r.footer_text_weak):nullptr;
        if(!label||expedition_display_adapter::footer(card,r.footer_index)!=label)return {};
        b.title=label;b.expedition_footer=true;
    }
    return b;
}
Record *record_for(void *text,Binding &b) {
    // Most game text is unrelated. Do not invoke expedition getters for every
    // unrelated label, especially while a list is laying out many recycled rows.
    for(auto &r:records)if(r.weak){auto card=target(r.weak);
        void *title=nullptr;
        if(r.footer)title=r.footer_text_weak?target(r.footer_text_weak):nullptr;
        else if(adult_card(card))title=read<void*>(card,0x48);
        else if(planted_detail_metadata&&is(card,planted_detail_class))title=read<void*>(card,0x60);
        else if(is(card,card_class))title=read<void*>(card,0x40);
        else if(is(card,expedition_class))title=read<void*>(card,0x70);
        else if(is(card,detail_class))title=read<void*>(card,0x28);
        else if(honey_metadata&&is(card,honey_card_class))title=read<void*>(card,0x58);
        else if(extract_metadata&&is(card,extract_card_class)) {
            auto petal=read<void*>(card,0x48);if(is(petal,petal_card_class))title=read<void*>(petal,0x30);
        }
        if(title==text){b=record_binding(r);if(b.title)return &r;}}
    return nullptr;
}
void layout_restore(Record &r,void *title) {
    if(!title)return;
    r.layout.restore([&](int overflow,int alignment){
        write_ui(title,0x13c,r.ellipsis);write_ui(title,0x13d,r.line_break);
        void *horizontal[]{&r.horizontal};call(horizontal_setter,title,horizontal);
        void *wrap[]{&overflow},*anchor[]{&alignment};
        call(overflow_setter,title,wrap);call(alignment_setter,title,anchor);
    });
}
void font_restore(Record &r,void *title) {
    if(!title)return;
    layout_restore(r,title);
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
std::u16string display_for(const Record &r,const Binding &b,bool enabled,bool insights=true) {
    if(b.expedition_footer){
        if(!insights||!expedition_display_wanted.load())return r.source;
        auto expedition=expedition_display_adapter::observe(b);
        expedition_display_phase.store(expedition_display_adapter::compatible?"ready":"metadata-mismatch");
        return display_insights::expedition_status(r.source,expedition);
    }
    if(b.planted_detail){
        if(!insights||!planter_display_wanted.load())return r.source;
        auto planted=planter_display_adapter::observe(b);
        planter_display_phase.store(planter_display_adapter::compatible?"ready":"metadata-mismatch");
        const std::u16string seed_source=b.seed_type==4?u"金色花苗":u"花苗";
        auto name=decorate(seed_source,b.seed_type,b.colour,true,r.name,r.treasure);
        if(name==seed_source&&b.colour>=1&&b.colour<=8){
            constexpr std::u16string_view colours[]{u"",u"紅色",u"藍色",u"黃色",u"白色",u"紫色",u"岩石",u"羽翅",u"冰色"};
            name=std::u16string(colours[b.colour])+u"花苗";
        }
        return display_insights::planter_detail(r.source,name,planted);
    }
    if(b.adult) {
        const bool state_on=insights&&adult_wanted.load();
        const bool decoration_on=insights&&decoration_wanted.load();
        if(!state_on&&!decoration_on)return r.source;
        display_insights::AdultState state;
        if(state_on) {
            state=adult_adapter::observe(b);
            adult_phase.store(adult_adapter::metadata_ok?"ready":"metadata-mismatch");
        }
        if(decoration_on) {
            auto decoration=decoration_adapter::observe(b);
            decoration_phase.store(decoration_adapter::compatible?"ready":"metadata-mismatch");
            if(decoration_adapter::compatible)
                return display_insights::adult_decoration_card(r.source,decoration,state_on?&state:nullptr);
        }
        return state_on&&adult_adapter::metadata_ok?display_insights::adult_card_label(r.source,state):r.source;
    }
    if(b.expedition_data&&insights&&gift_wanted.load()) {
        const auto gift=gift_adapter::observe(b);
        gift_phase.store(gift_adapter::compatible?"ready":"metadata-mismatch");
        if(gift.current&&gift.gift)return display_insights::gift_card(r.source,gift);
    }
    if(b.nectar) {
        if(!insights||!stock_wanted.load())return r.source;
        auto stock=stock_adapter::observe(b);
        stock_phase.store(stock_adapter::compatible?"ready":stock_adapter::checked?"metadata-mismatch":"waiting-cards");
        return stock_adapter::compatible?display_insights::stock_label(stock):r.source;
    }
    auto display=decorate(r.source,b.seed_type,b.colour,enabled,r.name,r.treasure);
    if(insights&&collection_wanted.load()&&b.type&&b.seed_type>0&&b.colour>=1&&b.colour<=8) {
        auto insight=collection_adapter::observe(b);
        collection_phase.store(collection_adapter::compatible?"ready":
            collection_adapter::checked?"metadata-mismatch":"waiting-cards");
        if(collection_adapter::compatible)display=compact_card_title(display)+u"\n"+display_insights::collection_card_label(insight);
    }
    return display;
}
void apply(Record &r,const Binding &b,const std::u16string &source,void *method,bool enable,bool insights=true) {
    if(!b.expedition_footer&&(!r.has_source||r.id!=b.id||r.signature!=b.signature||r.source!=source))resolve_name(r,b);
    Record input;input.source=source;input.name=r.name;input.treasure=r.treasure;
    auto display=display_for(input,b,enable,insights);
    // New managed strings and the game's own setter maintain the GC and UI queues.
    auto managed=new_string(display.data(),static_cast<int>(display.size()));if(!managed)return;
    pikmin::ScopedManagedRoot managed_root(managed,strong_new,release);if(!managed_root.handle)return;
    if(display!=source&&(b.expedition_footer||display.find(u'\n')!=std::u16string::npos)) {
        if(!r.layout.saved) {
            r.ellipsis=read<bool>(b.title,0x13c);r.line_break=read<bool>(b.title,0x13d);
            auto wrap=call(overflow_getter,b.title);auto wrap_value=wrap?unbox(wrap):nullptr;
            const int old_wrap=wrap_value?read<int>(wrap_value,0):-1;
            auto anchor=call(alignment_getter,b.title);auto anchor_value=anchor?unbox(anchor):nullptr;
            auto horizontal=call(horizontal_getter,b.title);auto horizontal_value=horizontal?unbox(horizontal):nullptr;
            const int old_horizontal=horizontal_value?read<int>(horizontal_value,0):-1;
            if(anchor_value&&old_horizontal>=0&&old_horizontal<=1){r.horizontal=old_horizontal;r.layout.capture(old_wrap,read<int>(anchor_value,0));}
        }
        if(r.layout.saved) {
            write_ui(b.title,0x13c,false);write_ui(b.title,0x13d,false);
            int horizontal=b.expedition_footer?1:0;void *horizontal_args[]{&horizontal};call(horizontal_setter,b.title,horizontal_args);
            // Existing title/count rectangles are single-line. Permit the
            // compact display-only subtitle without changing card geometry.
            int overflow=1,alignment=4;void *wrap[]{&overflow},*anchor[]{&alignment};
            call(overflow_setter,b.title,wrap);call(alignment_setter,b.title,anchor);
        }
    } else layout_restore(r,b.title);
    if(display!=source) {
        if(!r.font.saved) {
            auto boxed=call(font_getter,b.title);
            const int size=boxed?*static_cast<int*>(unbox(boxed)):0;
            r.font.capture(read<bool>(b.title,0x148),read<int>(b.title,0x14c),read<int>(b.title,0x150),size);
        }
        if(r.font.saved) {
            const bool fixed=b.expedition_footer||display.find(u'\n')!=std::u16string::npos;
            const int display_size=std::min((b.nectar||b.adult||b.expedition_footer)?14:20,r.font.size);
            write_ui(b.title,0x148,!fixed);
            write_ui(b.title,0x14c,fixed?display_size:minimum_font_size(r.font.size));
            write_ui(b.title,0x150,fixed?display_size:r.font.size);
            if(fixed){int size=display_size;void *args[]{&size};call(font_setter,b.title,args);}
        }
    } else font_restore(r,b.title);
    r.source=source;r.last=display;r.id=b.id;r.method=method;r.has_source=true;
    managed=target(managed_root.handle);if(!managed)return;
    in_setter=true;original_text.load()(b.title,managed,method);
    if(display!=source&&(b.expedition_footer||display.find(u'\n')!=std::u16string::npos)) {
        // Preserve IchigoText's normal rule/original-text bookkeeping, then
        // submit the complete display-only subtitle through Unity's setter.
        // The game's ellipsis rule otherwise strips extra lines and narrow
        // active-footer text before the final Unity layout receives it.
        managed=target(managed_root.handle);
        if(managed){void *args[]{managed};call(raw_text_setter,b.title,args);}
    }
    in_setter=false;
    if(display!=source){if(b.colour>=1&&b.colour<=8)decorated_mask.fetch_or(1u<<(b.colour-1));applied.fetch_add(1);}else restored.fetch_add(1);
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
void bind_footers(void *card){
    if(!expedition_display_wanted.load()||!is(card,expedition_class))return;
    if(!expedition_display_adapter::setup()){expedition_display_phase.store("metadata-mismatch");return;}
    for(int index=0;index<4;++index){
        auto label=expedition_display_adapter::footer(card,index);if(!label)continue;
        Record *slot=nullptr;
        for(auto &r:records){auto owner=r.weak?target(r.weak):nullptr;
            if(owner==card&&r.footer&&r.footer_index==index){slot=&r;break;}
            if(!owner&&!slot)slot=&r;
        }
        if(!slot)continue;
        if(!slot->weak||target(slot->weak)!=card||!slot->footer_text_weak||target(slot->footer_text_weak)!=label){
            reset_record(*slot);slot->weak=weak_new(card,false);slot->footer_text_weak=weak_new(label,false);
            slot->footer=true;slot->footer_index=index;
        }
    }
}
void begin_update(void *self,void *data) {
    seed_calls.fetch_add(1);
    int expected=0;ui_tid.compare_exchange_strong(expected,gettid());
    const bool on_ui=gettid()==ui_tid.load();
    if(on_ui){updating_card=self;updating_data=data;}
    if(hooks_ready.load()&&gettid()==ui_tid.load()&&metadata()){
            Record *slot=nullptr;
            for(auto &r:records){auto card=r.weak?target(r.weak):nullptr;
                if(card==self&&!r.footer){slot=&r;break;}
                if(!card&&!slot)slot=&r;
            }
            for(auto &r:records)if(r.footer&&r.weak&&target(r.weak)==self&&r.has_source){
                auto b=record_binding(r);
                if(b.title&&r.id!=b.id){
                    // Retire our own previous footer before the game's async
                    // location refresh; OFF must never restore a status line.
                    apply(r,b,r.source,r.method,false,false);r.has_source=false;
                }
            }
            if(slot){
                if(!slot->weak||target(slot->weak)!=self){reset_record(*slot);slot->weak=weak_new(self,false);}
                if(slot->weak)tracked.fetch_add(1);
                // Remove the previous binding's decoration before the original asynchronous refresh.
                auto b=binding(self);
                if(adult_card(self)) {
                    if(slot->adult_item_weak)release(slot->adult_item_weak);
                    slot->adult_item_weak=b.adult_item?weak_new(b.adult_item,false):0;
                    slot->adult_id=b.id;
                }

                if(slot->has_source&&(slot->id!=b.id||(b.seed_type==0&&!b.nectar&&!b.adult&&!b.expedition_data))){
                    font_restore(*slot,b.title);
                    if(b.title&&string_value(read<void*>(b.title,0x108))==slot->last){
                        apply(*slot,b,slot->source,slot->method,false,false);
                    }
                    slot->has_source=false;
                }
            }
            bind_footers(self);
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
void adult_hook(void *self,void *item,void *method) {
    UpdateScope scope(self,item);original_adult.load()(self,item,method);
}
void planted_detail_hook(void *self,void *data,void *method) {
    UpdateScope scope(self,data);original_planted_detail.load()(self,data,method);
}
void honey_hook(void *self,void *data,void *method) {
    UpdateScope scope(self,data);original_honey.load()(self,data,method);
}
void *extract_hook(void *self,void *data,void *method) {
    UpdateScope scope(self,data);return original_extract.load()(self,data,method);
}
void canvas_hook(void *self,void *method) {
    canvas_calls.fetch_add(1);canvas_tid.store(gettid());
    if(hooks_ready.load()&&gettid()==ui_tid.load()&&metadata_ok){
        const bool enabled=wanted.load();
        const auto footer_now=std::chrono::steady_clock::now();
        for(auto &r:records)if(r.weak&&!r.footer&&footer_now-r.footer_scan>=std::chrono::seconds(2)){
            bind_footers(target(r.weak));r.footer_scan=footer_now;
        }
        unsigned alive=0;
        for(auto &r:records)if(r.weak){
            auto b=record_binding(r);
            if(!b.title){reset_record(r);continue;}++alive;
            auto current=string_value(read<void*>(b.title,0x108));
            if(r.footer&&(!r.has_source||r.id!=b.id)){
                // A recycled card cannot keep another task's stored location.
                font_restore(r,b.title);if(current!=r.last)r.source=current;r.last=current;r.id=b.id;
                r.method=expedition_display_adapter::setter;r.has_source=true;
            }
            if(!r.has_source)continue;
            // A reused card must never inherit another seedling's stored title.
            if(!r.footer&&(b.id!=r.id||(b.seed_type==0&&!b.nectar&&!b.adult&&!b.expedition_data))){font_restore(r,b.title);r.has_source=false;continue;}
            if(current!=r.last){r.source=current;r.last=current;}
            if(!b.expedition_footer&&b.signature!=r.signature)resolve_name(r,b);
            if(display_for(r,b,enabled)!=current)apply(r,b,r.source,r.method,enabled);
            if(r.footer&&!expedition_display_wanted.load()){
                font_restore(r,b.title);reset_record(r);
            }
        }
        bound_count.store(alive);
        rendered_enabled.store(enabled);
        collection_rendered.store(collection_wanted.load());
        stock_rendered.store(stock_wanted.load());
        adult_rendered.store(adult_wanted.load());
        decoration_rendered.store(decoration_wanted.load());
        gift_rendered.store(gift_wanted.load());
        planter_display_rendered.store(planter_display_wanted.load());
        expedition_display_rendered.store(expedition_display_wanted.load());
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
bool control_on(const char *path=control) {
    int fd=open(path,O_RDONLY|O_CLOEXEC|O_NOFOLLOW);if(fd<0)return false;
    struct stat st{};char b[8]{};ssize_t n=-1;
    if(!fstat(fd,&st)&&S_ISREG(st.st_mode)&&st.st_uid==0&&(st.st_mode&0022)==0&&st.st_size<=7)n=::read(fd,b,7);
    close(fd);return n>0&&enabled_control({b,static_cast<size_t>(n)});
}
#include "standalone_context.inc"
void worker(std::string directory) {
    std::ifstream proc("/proc/self/stat");std::string line,start;std::getline(proc,line);auto end=line.rfind(')');if(end==line.npos)return;std::istringstream fields(line.substr(end+2));for(int i=3;i<=22;++i)if(!(fields>>start))return;
    void *lib=nullptr;for(int i=0;i<180&&!lib;++i){lib=xdl_open("libil2cpp.so",XDL_DEFAULT);if(!lib)sleep(1);}
    xdl_info_t info{};struct stat st{};
    bool valid=lib&&xdl_info(lib,XDL_DI_DLINFO,&info)==0&&info.dli_fbase&&info.dli_fname
        &&!stat(info.dli_fname,&st)&&st.st_size==static_cast<off_t>(display_game.elf_size)
        &&static_cast<Elf64_Ehdr*>(info.dli_fbase)->e_machine==EM_AARCH64&&seed_preview::verified_game_profile(info.dli_fname) == &seed_preview::game_profiles[2];
    auto base=static_cast<unsigned char*>(info.dli_fbase);
    for(const auto *entry:{&display_game.inventory,&display_game.text,&display_game.canvas,
            &display_game.expedition,&display_game.detail,&display_game.honey,&display_game.extract})
        valid=valid&&!memcmp(base+entry->rva,entry->prefix.data(),sizeof(uint32_t)*entry->prefix.size());
    if(display_game.adult.rva)valid=valid&&!memcmp(base+display_game.adult.rva,display_game.adult.prefix.data(),sizeof(uint32_t)*display_game.adult.prefix.size());
    if(display_game.planted_detail.rva)valid=valid&&!memcmp(base+display_game.planted_detail.rva,display_game.planted_detail.prefix.data(),sizeof(uint32_t)*display_game.planted_detail.prefix.size());
#define RESOLVE(var,name) var=reinterpret_cast<decltype(var)>(xdl_sym(lib,name,nullptr));valid=valid&&var
    if(valid){
        RESOLVE(domain_get,"il2cpp_domain_get");RESOLVE(assemblies,"il2cpp_domain_get_assemblies");
        RESOLVE(image,"il2cpp_assembly_get_image");RESOLVE(klass,"il2cpp_class_from_name");
        RESOLVE(field,"il2cpp_class_get_field_from_name");RESOLVE(offset,"il2cpp_field_get_offset");
        RESOLVE(weak_new,"il2cpp_gchandle_new_weakref");RESOLVE(target,"il2cpp_gchandle_get_target");
        RESOLVE(strong_new,"il2cpp_gchandle_new");
        RESOLVE(release,"il2cpp_gchandle_free");RESOLVE(new_string,"il2cpp_string_new_utf16");
        RESOLVE(object_class,"il2cpp_object_get_class");RESOLVE(assignable,"il2cpp_class_is_assignable_from");
        RESOLVE(value_type,"il2cpp_class_is_valuetype");
        RESOLVE(method_named,"il2cpp_class_get_method_from_name");RESOLVE(invoke,"il2cpp_runtime_invoke");RESOLVE(unbox,"il2cpp_object_unbox");
    }
#undef RESOLVE
    if(valid){expedition_display_adapter::configure(lib);valid=standalone_context::configure(lib,base);}
    if(valid){valid=hook(base,display_game.text.rva,reinterpret_cast<void*>(text_hook),original_text)
        &&hook(base,display_game.canvas.rva,reinterpret_cast<void*>(canvas_hook),original_canvas)
        &&hook(base,display_game.inventory.rva,reinterpret_cast<void*>(inventory_hook),original_inventory)
        &&hook(base,display_game.expedition.rva,reinterpret_cast<void*>(expedition_hook),original_expedition)
        &&hook(base,display_game.detail.rva,reinterpret_cast<void*>(detail_hook),original_detail)
        &&hook(base,display_game.honey.rva,reinterpret_cast<void*>(honey_hook),original_honey)
        &&hook(base,display_game.extract.rva,reinterpret_cast<void*>(extract_hook),original_extract);
        if(valid&&display_game.adult.rva)valid=hook(base,display_game.adult.rva,reinterpret_cast<void*>(adult_hook),original_adult);
        if(valid&&display_game.planted_detail.rva)valid=hook(base,display_game.planted_detail.rva,reinterpret_cast<void*>(planted_detail_hook),original_planted_detail);}
    phase.store(valid?"waiting-cards":"incompatible");hooks_ready.store(valid);
    const auto path=directory+"/files/seed_lens_seed_preview_status.tsv",tmp=path+".tmp";
    for(;;){
        wanted.store(valid&&control_on());
        collection_wanted.store(valid&&control_on("/data/local/tmp/seed-lens-seed-collection.txt"));
        stock_wanted.store(valid&&control_on("/data/local/tmp/seed-lens-nectar-stock-preview.txt"));
        adult_wanted.store(valid&&control_on("/data/local/tmp/seed-lens-adult-state-preview.txt"));
        decoration_wanted.store(valid&&display_game.adult.rva&&control_on("/data/local/tmp/seed-lens-adult-decoration-preview.txt"));
        gift_wanted.store(valid&&display_game.adult.rva&&control_on("/data/local/tmp/seed-lens-gift-preview.txt"));
        planter_display_wanted.store(valid&&display_game.planted_detail.rva&&control_on("/data/local/tmp/seed-lens-planter-display.txt"));
        expedition_display_wanted.store(valid&&display_game.adult.rva&&control_on("/data/local/tmp/seed-lens-expedition-display.txt"));
        auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        FILE *f=fopen(tmp.c_str(),"w");if(f){bool ok=fprintf(f,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t%u\t%u\t%u\t%s\n",
            static_cast<long long>(now),getpid(),display_game.version,phase.load(),(!strcmp(phase.load(),"ready")?rendered_enabled.load():wanted.load())?1:0,bound_count.load(),decorated_mask.load(),applied.load(),restored.load(),start.c_str())>0;
            if(fclose(f))ok=false;if(ok){chmod(tmp.c_str(),0644);rename(tmp.c_str(),path.c_str());}}
        FILE *diag=fopen((directory+"/files/seed_lens_seed_preview_diagnostic.tsv").c_str(),"w");
        if(diag){fprintf(diag,"%u\t%u\t%u\t%u\t%u\t%d\t%d\t%d\n",seed_calls.load(),text_calls.load(),canvas_calls.load(),tracked.load(),text_matches.load(),ui_tid.load(),text_tid.load(),canvas_tid.load());fclose(diag);}
        const auto collection_path=directory+"/files/seed_lens_seed_collection_status.tsv",collection_tmp=collection_path+".tmp";
        if(FILE *out=fopen(collection_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid?collection_phase.load():"incompatible",
                collection_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(collection_tmp.c_str(),0644);rename(collection_tmp.c_str(),collection_path.c_str());}
        }
        const auto stock_path=directory+"/files/seed_lens_nectar_stock_preview_status.tsv",stock_tmp=stock_path+".tmp";
        if(FILE *out=fopen(stock_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid?stock_phase.load():"incompatible",
                stock_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(stock_tmp.c_str(),0644);rename(stock_tmp.c_str(),stock_path.c_str());}
        }
        const auto adult_path=directory+"/files/seed_lens_adult_state_preview_status.tsv",adult_tmp=adult_path+".tmp";
        if(FILE *out=fopen(adult_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid?adult_phase.load():"incompatible",
                adult_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(adult_tmp.c_str(),0644);rename(adult_tmp.c_str(),adult_path.c_str());}
        }
        const auto decoration_path=directory+"/files/seed_lens_adult_decoration_preview_status.tsv",decoration_tmp=decoration_path+".tmp";
        if(FILE *out=fopen(decoration_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid&&display_game.adult.rva?decoration_phase.load():"incompatible",
                decoration_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(decoration_tmp.c_str(),0644);rename(decoration_tmp.c_str(),decoration_path.c_str());}
        }
        const auto gift_path=directory+"/files/seed_lens_gift_preview_status.tsv",gift_tmp=gift_path+".tmp";
        if(FILE *out=fopen(gift_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid&&display_game.adult.rva?gift_phase.load():"incompatible",
                gift_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(gift_tmp.c_str(),0644);rename(gift_tmp.c_str(),gift_path.c_str());}
        }
        const auto planter_path=directory+"/files/seed_lens_planter_display_status.tsv",planter_tmp=planter_path+".tmp";
        if(FILE *out=fopen(planter_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid&&display_game.planted_detail.rva?planter_display_phase.load():"incompatible",
                planter_display_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(planter_tmp.c_str(),0644);rename(planter_tmp.c_str(),planter_path.c_str());}
        }
        const auto expedition_path=directory+"/files/seed_lens_expedition_display_status.tsv",expedition_tmp=expedition_path+".tmp";
        if(FILE *out=fopen(expedition_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%u\t0\t0\t0\t%s\n",
                static_cast<long long>(now),getpid(),display_game.version,valid&&display_game.adult.rva?expedition_display_phase.load():"incompatible",
                expedition_display_rendered.load()?1:0,bound_count.load(),start.c_str())>0;
            if(fclose(out))ok=false;if(ok){chmod(expedition_tmp.c_str(),0644);rename(expedition_tmp.c_str(),expedition_path.c_str());}
        }
        const auto diagnostic_path=directory+"/files/seed_lens_display_inventory_read_status.tsv",diagnostic_tmp=diagnostic_path+".tmp";
        if(FILE *out=fopen(diagnostic_tmp.c_str(),"w")) {
            bool ok=fprintf(out,"v1\t%lld\t%d\t%s\t%s\t%d\t%d\n",static_cast<long long>(now),getpid(),
                collection_adapter::read_phase.load(),stock_adapter::read_phase.load(),
                collection_adapter::owned_published.load(),collection_adapter::growing_published.load())>0;
            if(fclose(out))ok=false;if(ok){chmod(diagnostic_tmp.c_str(),0644);rename(diagnostic_tmp.c_str(),diagnostic_path.c_str());}
        }
        sleep(1);
    }
}
} // namespace
void start(const char *directory,ContextReader reader) {
    context_reader.store(reader?reader:standalone_context::read_context);
    if(!directory||started.exchange(true))return;
    struct stat st{};
    if(getuid()/100000!=0||lstat(directory,&st)||!S_ISDIR(st.st_mode)||st.st_uid!=getuid())return;
    std::thread(worker,std::string(directory)).detach();
}
} // namespace seed_extended
