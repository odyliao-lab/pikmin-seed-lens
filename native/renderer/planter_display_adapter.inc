// Read-only planted-seed details. Reuse D01's bounded confirmed planter snapshot.
namespace planter_display_adapter {
bool checked{},compatible{};
bool setup(){
    if(checked)return compatible;checked=true;
    compatible=collection_adapter::setup()
        &&field_is(seed_class,"currentSteps_",0x54)&&field_is(seed_class,"requiredSteps_",0x50)
        &&field_is(seed_class,"plantedTimeMs_",0x60);
    return compatible;
}
display_insights::PlanterDisplay observe(const Binding &b){
    display_insights::PlanterDisplay p;
    if(!setup()||!b.seed_item||b.id.empty())return p;
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    auto manager=context.inventory;
    if(context.session.empty()||!is(manager,collection_adapter::manager_class)||!collection_adapter::ready(manager)
            ||!is(b.provider,provider_class)||read<void*>(b.provider,0x18)!=manager)return p;
    const auto revision=read<int64_t>(manager,display_game.inventory_revision);
    auto item=collection_adapter::confirmed(manager,b.seed_item);pikmin::ScopedManagedRoot item_root(item,strong_new,release);
    if(!is(item,item_class)||string_value(collection_adapter::get(item,"get_Id"))!=b.id)return p;
    auto proto=collection_adapter::get(item,"get_Proto");pikmin::ScopedManagedRoot proto_root(proto,strong_new,release);
    if(!is(proto,seed_class)||read<int64_t>(proto,0x60)<=0)return p;
    collection_adapter::capture(context);
    if(!collection_adapter::growing_complete||collection_adapter::cached_session!=context.session
            ||collection_adapter::cached_manager!=manager||collection_adapter::cached_revision!=revision
            ||!collection_adapter::growing_ids.count(b.id))return p;
    p.steps=read<int>(proto,0x54);p.required=read<int>(proto,0x50);
    auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager||read<int64_t>(manager,display_game.inventory_revision)!=revision
            ||collection_adapter::confirmed(manager,b.seed_item)!=item||collection_adapter::get(item,"get_Proto")!=proto)return {};
    p.current=p.planted=true;return p;
}
}
