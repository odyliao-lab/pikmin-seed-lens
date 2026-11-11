// Existing expedition list/detail bindings; no new hook, RPC or inventory write.
namespace gift_adapter {
using Root=pikmin::ScopedManagedRoot;
void *task_class{},*task_proto_class{},*expedition_proto_class{},*gift_class{},*deco_class{},*rare_class{};
void *owned_class{},*owned_conversion{},*display_name{};
bool checked{},compatible{};
struct Cached {
    display_insights::GiftPreview result;
    std::chrono::steady_clock::time_point at;
    void *manager{},*provider{};int64_t revision{};
    std::u16string recipient_id;
};
std::string session;
std::map<std::u16string,Cached> rows;
bool setup() {
    if(checked)return compatible;checked=true;
    task_class=find("Niantic.Ichigo.Inventory","PikminTaskInventoryItem");
    task_proto_class=find("Ichigo.Proto","PikminTaskProto");
    expedition_proto_class=find("Ichigo.Proto","ExpeditionTaskProto");
    gift_class=find("Ichigo.Proto","PikminGiftProto");
    deco_class=find("Ichigo.Proto","PikminDecorationProto");
    rare_class=find("Ichigo.Proto","PikminRareDecorationProto");
    owned_class=find("Niantic.Ichigo.Utils.Proto","PlayerOwnedPikmin");
    owned_conversion=owned_class?method_named(owned_class,"op_Implicit",1):nullptr;
    display_name=variation_class?method_named(variation_class,"GetDisplayName",1):nullptr;
    compatible=decoration_adapter::setup()&&task_class&&task_proto_class&&rare_class
        &&owned_class&&value_type(owned_class)&&owned_conversion&&display_name
        &&field_is(expedition_data_class,"item",0x78)
        &&field_is(task_proto_class,"taskCase_",0x50)
        &&field_is(expedition_proto_class,"targetCase_",0x70)
        &&field_is(expedition_proto_class,"restriction_",0x58)
        &&field_is(gift_class,"itemCase_",0x28)
        &&field_is(deco_class,"pikminId_",0x18)
        &&field_is(deco_class,"pikminType_",0x20)
        &&field_is(deco_class,"boxType_",0x28)
        &&method_named(expedition_data_class,"get_Gift",0)
        &&method_named(collection_adapter::manager_class,"GetPikmin",1)
        &&method_named(collection_adapter::adult_proto_class,"get_Name",0);
    return compatible;
}
void *recipient(void *manager,const std::u16string &id) {
    if(id.empty())return nullptr;
    auto managed=new_string(id.data(),static_cast<int>(id.size()));Root id_root(managed,strong_new,release);
    if(!id_root.handle)return nullptr;
    void *args[]{managed};auto item=collection_adapter::get(manager,"GetPikmin",1,args);
    auto value=collection_adapter::confirmed(manager,item);
    return is(value,collection_adapter::adult_item_class)
        &&string_value(collection_adapter::get(value,"get_Id"))==id?value:nullptr;
}
display_insights::GiftPreview observe(const Binding &b) {
    display_insights::GiftPreview unknown;
    if(!setup()||!b.expedition_data||b.id.empty())return unknown;
    Root data_root(b.expedition_data,strong_new,release);
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    if(context.session.empty()||!is(context.inventory,collection_adapter::manager_class)) {
        rows.clear();session.clear();return unknown;
    }
    if(session!=context.session){rows.clear();session=context.session;}
    auto manager=context.inventory;auto revision=read<int64_t>(manager,display_game.inventory_revision);
    if(revision<=0||!collection_adapter::ready(manager))return unknown;
    auto item=read<void*>(b.expedition_data,0x78);
    auto confirmed=collection_adapter::confirmed(manager,item);Root item_root(confirmed,strong_new,release);
    if(!is(confirmed,task_class)||confirmed!=item
            ||string_value(collection_adapter::get(confirmed,"get_Id"))!=b.id)return unknown;
    const auto now=std::chrono::steady_clock::now();auto observed=now;
    display_insights::GiftPreview result;std::u16string recipient_id;void *adult=nullptr;
    auto old=rows.find(b.id);
    if(old!=rows.end()&&old->second.manager==manager&&old->second.provider==b.provider
            &&old->second.revision==revision&&now-old->second.at<std::chrono::seconds(2)) {
        result=old->second.result;recipient_id=old->second.recipient_id;observed=old->second.at;
        if(!recipient_id.empty()&&!(adult=recipient(manager,recipient_id)))return unknown;
    } else {
        auto proto=collection_adapter::get(confirmed,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,task_proto_class))return unknown;
        auto expedition=collection_adapter::get(proto,"get_Expedition");Root expedition_root(expedition,strong_new,release);
        if(!is(expedition,expedition_proto_class))return unknown;
        auto gift=collection_adapter::get(expedition,"get_Gift");Root gift_root(gift,strong_new,release);
        if(!is(gift,gift_class))return unknown; // Fruit and seed cards unchanged.
        auto shown=collection_adapter::get(b.expedition_data,"get_Gift");Root shown_root(shown,strong_new,release);
        if(shown!=gift)return unknown;
        result.gift=true;result.current=true;
        int which=-1,box=-1;
        auto deco=collection_adapter::get(gift,"get_Deco");Root deco_root(deco,strong_new,release);
        auto rare=collection_adapter::get(gift,"get_RareDeco");Root rare_root(rare,strong_new,release);
        if(!collection_adapter::number(collection_adapter::get(gift,"get_ItemCase"),which))return unknown;
        if(is(deco,deco_class))collection_adapter::number(collection_adapter::get(deco,"get_BoxType"),box);
        result.kind=pikmin::gift_dispatch_type(true,which,is(deco,deco_class),is(rare,rare_class),box);
        if(result.kind==pikmin::GiftDispatchType::ordinary_white) {
            const auto decoration_id=string_value(collection_adapter::get(deco,"get_PikminId"));
            auto restriction=collection_adapter::get(expedition,"get_Restriction");Root restriction_root(restriction,strong_new,release);
            int condition=0;std::u16string allowed_id;
            if(restriction) {
                condition=-1;
                if(field_is(object_class(restriction),"conditionCase_",0x20)
                        &&collection_adapter::number(collection_adapter::get(restriction,"get_ConditionCase"),condition)
                        &&condition==1)
                    allowed_id=string_value(collection_adapter::get(restriction,"get_AllowedPikminId"));
            }
            const auto identity=display_insights::gift_recipient_identity(decoration_id,condition,allowed_id);
            recipient_id=identity.id;
            adult=recipient(manager,recipient_id);Root adult_root(adult,strong_new,release);
            result.reason=identity.reason!=display_insights::GiftReason::none?identity.reason:
                !adult?display_insights::GiftReason::recipient_missing:display_insights::GiftReason::name_missing;
            if(adult_root.handle) {
                auto adult_proto=collection_adapter::get(adult,"get_Proto");Root adult_proto_root(adult_proto,strong_new,release);
                auto variation=read<void*>(b.expedition_data,0x58);Root variation_root(variation,strong_new,release);
                if(is(adult_proto,collection_adapter::adult_proto_class)&&is(variation,variation_class)) {
                    // The game's own main list uses GetDisplayName(PlayerOwnedPikmin),
                    // not raw Proto.Name (which can legitimately be empty). Use
                    // its real implicit conversion and boxed value; no guessed
                    // value-type layout, generated name, or model mutation.
                    void *convert_args[]{adult_proto},*exception=nullptr;
                    auto wrapped=invoke(owned_conversion,nullptr,convert_args,&exception);
                    Root wrapped_root(wrapped,strong_new,release);
                    if(!exception&&wrapped_root.handle&&is(wrapped,owned_class)) {
                        auto value=unbox(wrapped);
                        if(value){void *name_args[]{value};
                            auto name=call(display_name,variation,name_args);Root name_root(name,strong_new,release);
                            result.recipient=seed_preview::plain_name(string_value(name));}
                    }
                }
            }
            // Explicit ordinary gift type, NOT the recipient's future type.
            auto type=collection_adapter::get(deco,"get_PikminType");Root type_root(type,strong_new,release);
            collection_adapter::Key key;
            if(adult&&collection_adapter::key(type,key)&&std::get<1>(key)>=2
                    &&is(b.provider,provider_class)&&read<void*>(b.provider,0x18)==manager) {
                void *args[]{type};int treasure=-1;
                if(collection_adapter::number(call(decoration_adapter::type_getter,b.provider,args),treasure)&&treasure>0)
                    result.decoration=seed_preview::plain_name(string_value(call(decoration_adapter::name_getter,b.provider,args)));
            }
        }
    }
    auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager
            ||read<int64_t>(manager,display_game.inventory_revision)!=revision
            ||read<void*>(b.expedition_data,0x78)!=item
            ||collection_adapter::confirmed(manager,item)!=confirmed
            ||(!recipient_id.empty()&&recipient(manager,recipient_id)!=adult))return unknown;
    // A missing recipient remains a fresh partial result, never a cache hit
    // that could alternate between an unknown subtitle and the original name.
    if(!recipient_id.empty()&&!adult)return result;
    if(rows.size()>=max_cards&&old==rows.end())rows.clear();
    rows[b.id]={result,observed,manager,b.provider,revision,recipient_id};return result;
}
}
