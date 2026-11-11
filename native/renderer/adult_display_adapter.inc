// UI-thread only, confirmed per-item reads; no inventory scan and no RPC.
namespace adult_adapter {
using Root=pikmin::ScopedManagedRoot;
struct Cached {
    display_insights::AdultState state;
    std::chrono::steady_clock::time_point at;
    const void *manager{};
    int64_t revision{};
};
std::string session;
std::map<std::u16string,Cached> rows;
bool metadata_checked{},metadata_ok{};
bool setup() {
    if(metadata_checked)return metadata_ok;metadata_checked=true;
    if(!collection_adapter::setup())return false;
    auto proto=collection_adapter::adult_proto_class;
    metadata_ok=field_is(data_class,"Pikmin",0x10)&&field_is(data_class,"Seed",0x18)
        &&field_is(proto,"flowerState_",0x48)&&field_is(proto,"nextNumPetals_",0x84)
        &&field_is(proto,"statusCase_",0xd0)
        &&method_named(proto,"get_FlowerState",0)&&method_named(proto,"get_NextNumPetals",0)
        &&method_named(proto,"get_StatusCase",0);
    return metadata_ok;
}
display_insights::AdultState observe(const Binding &b) {
    display_insights::AdultState unknown;
    if(!setup()||!b.adult_item||b.id.empty())return unknown;
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    if(context.session.empty()||!is(context.inventory,collection_adapter::manager_class)) {
        rows.clear();session.clear();return unknown;
    }
    if(session!=context.session){rows.clear();session=context.session;}
    auto manager=context.inventory;
    const int64_t revision=read<int64_t>(manager,display_game.inventory_revision);
    if(revision<=0||!collection_adapter::ready(manager))return unknown;
    // Even a cache hit rechecks this exact item's prediction and membership.
    auto item=collection_adapter::confirmed(manager,b.adult_item);Root item_root(item,strong_new,release);
    if(!is(item,collection_adapter::adult_item_class)
            ||string_value(collection_adapter::get(item,"get_Id"))!=b.id)return unknown;
    auto now=std::chrono::steady_clock::now();
    display_insights::AdultState state;
    auto old=rows.find(b.id);auto observed_at=now;
    if(old!=rows.end()&&old->second.manager==manager&&old->second.revision==revision
            &&now-old->second.at<std::chrono::seconds(2)){state=old->second.state;observed_at=old->second.at;}
    else {
        auto proto=collection_adapter::get(item,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,collection_adapter::adult_proto_class))return unknown;
        state.current=collection_adapter::number(collection_adapter::get(proto,"get_FlowerState"),state.flower)
            &&collection_adapter::number(collection_adapter::get(proto,"get_StatusCase"),state.status);
        collection_adapter::number(collection_adapter::get(proto,"get_NextNumPetals"),state.petals);
    }
    auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager
            ||read<int64_t>(manager,display_game.inventory_revision)!=revision
            ||collection_adapter::confirmed(manager,b.adult_item)!=item)return unknown;
    if(rows.size()>=max_cards&&old==rows.end())rows.clear();
    rows[b.id]={state,observed_at,manager,revision};return state;
}
}
