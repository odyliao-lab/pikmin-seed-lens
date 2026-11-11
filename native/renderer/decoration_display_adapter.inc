// Current acquired appearance only; never call the future decoration provider.
namespace decoration_adapter {
using Root=pikmin::ScopedManagedRoot;
struct Cached {
    display_insights::AdultDecoration value;
    std::chrono::steady_clock::time_point at;
    const void *manager{},*provider{};
    int64_t revision{};
};
std::map<std::u16string,Cached> rows;
std::string session;
bool checked{},compatible{};
void *name_getter{},*type_getter{};
bool setup() {
    if(checked)return compatible;checked=true;
    name_getter=provider_class?method_named(provider_class,"GetTreasureName",1):nullptr;
    type_getter=provider_class?method_named(provider_class,"GetTreasureType",1):nullptr;
    compatible=collection_adapter::setup()&&name_getter&&type_getter
        &&field_is(collection_adapter::adult_proto_class,"pikminType_",0x18)
        &&field_is(type_class,"fullAssetId_",0x20)&&field_is(type_class,"textureId_",0x24)
        &&field_is(variation_class,"treasureNameProvider",0x28)
        &&field_is(provider_class,"inventoryManager",0x18);
    return compatible;
}
display_insights::AdultDecoration observe(const Binding &b) {
    display_insights::AdultDecoration unknown;
    if(!setup()||!b.adult_item||b.id.empty())return unknown;
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    if(context.session.empty()||!is(context.inventory,collection_adapter::manager_class)) {
        rows.clear();session.clear();return unknown;
    }
    if(session!=context.session){rows.clear();session=context.session;}
    auto manager=context.inventory;
    const int64_t revision=read<int64_t>(manager,display_game.inventory_revision);
    if(revision<=0||!collection_adapter::ready(manager))return unknown;
    auto item=collection_adapter::confirmed(manager,b.adult_item);Root item_root(item,strong_new,release);
    if(!is(item,collection_adapter::adult_item_class)
            ||string_value(collection_adapter::get(item,"get_Id"))!=b.id)return unknown;
    auto now=std::chrono::steady_clock::now();auto observed_at=now;
    display_insights::AdultDecoration result;
    auto old=rows.find(b.id);
    if(old!=rows.end()&&old->second.manager==manager&&old->second.provider==b.provider
            &&old->second.revision==revision&&now-old->second.at<std::chrono::seconds(2)) {
        result=old->second.value;observed_at=old->second.at;
    } else {
        auto proto=collection_adapter::get(item,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,collection_adapter::adult_proto_class))return unknown;
        auto type=read<void*>(proto,0x18);Root type_root(type,strong_new,release);
        collection_adapter::Key identity;
        if(!collection_adapter::key(type,identity))return unknown;
        result.asset=std::get<1>(identity);
        if(result.asset==1)result.current=true;
        else {
            if(!is(b.provider,provider_class)||read<void*>(b.provider,0x18)!=manager)return unknown;
            Root provider_root(b.provider,strong_new,release);if(!provider_root.handle)return unknown;
            void *args[]{type};int treasure=-1;
            auto boxed=call(type_getter,b.provider,args);Root treasure_root(boxed,strong_new,release);
            if(!collection_adapter::number(boxed,treasure)||treasure<=0)return unknown;
            auto name=call(name_getter,b.provider,args);Root name_root(name,strong_new,release);
            result.name=seed_preview::plain_name(string_value(name));
            result.current=!result.name.empty();
        }
    }
    auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager
            ||read<int64_t>(manager,display_game.inventory_revision)!=revision
            ||collection_adapter::confirmed(manager,b.adult_item)!=item)return unknown;
    if(rows.size()>=max_cards&&old==rows.end())rows.clear();
    rows[b.id]={result,observed_at,manager,b.provider,revision};return result;
}
}
