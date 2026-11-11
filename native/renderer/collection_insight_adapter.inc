// Included inside the renderer's private namespace. Managed calls run only on
// its established UI thread; this adapter has no RPC or game-model write API.
namespace collection_adapter {
using Root=pikmin::ScopedManagedRoot;
using Key=std::tuple<int,int,int>; // exact colour, asset enum, texture
void *manager_class{},*adult_item_class{},*adult_proto_class{},*planter_item_class{},*planter_proto_class{};
void *entry_class{},*value_class{};
void *future_entry{};
bool checked{},compatible{};
std::string cached_session;
void *cached_manager{};
int64_t cached_revision{};
std::chrono::steady_clock::time_point captured{};
std::map<Key,int> owned,growing,waiting;
std::set<std::u16string> growing_ids;
std::map<Key,display_insights::CollectionInsight> insights;
bool owned_complete{},growing_complete{},waiting_complete{};
std::atomic<const char*> read_phase{"not-read"};
std::atomic<int> owned_published{},growing_published{};

void *get(void *self,const char *name,int count=0,void **args=nullptr) {
    return self?call(method_named(object_class(self),name,count),self,args):nullptr;
}
bool number(void *boxed,int &value) {
    if(!boxed)return false;
    auto p=unbox(boxed);if(!p)return false;value=read<int>(p,0);return true;
}
bool boolean(void *boxed,int &value) {
    if(!boxed)return false;
    auto p=unbox(boxed);if(!p)return false;
    value=read<unsigned char>(p,0);return value==0||value==1;
}
bool ready(void *manager) {
    auto task=get(manager,"get_Initialized");Root root(task,strong_new,release);
    int value=-1;return root.handle&&boolean(get(task,"get_IsCompletedSuccessfully"),value)&&value==1;
}
bool unpredicted(void *manager,std::initializer_list<void*> relevant) {
    read_phase.store("dictionary-values");
    auto dict=read<void*>(manager,display_game.inventory_items);Root dict_root(dict,strong_new,release);
    auto values=get(dict,"get_Values");Root values_root(values,strong_new,release);int count=-1;
    if(!values_root.handle||!number(get(values,"get_Count"),count)||count<0||count>32768)return false;
    read_phase.store("dictionary-enumerator");
    auto enumerator=get(values,"GetEnumerator");Root enum_root(enumerator,strong_new,release);
    if(!enum_root.handle)return false;
    auto type=object_class(enumerator);auto move=method_named(type,"MoveNext",0),current=method_named(type,"get_Current",0);
    read_phase.store("dictionary-methods");
    if(!move||!current)return false;
    void *self=value_type(type)?unbox(enumerator):enumerator;if(!self)return false;
    int rows=0;
    read_phase.store("dictionary-iterate");
    for(;;){int more=-1;if(!boolean(call(move,self),more))return false;if(!more)break;
        if(++rows>count)return false;
        auto predicted=call(current,self);Root row(predicted,strong_new,release);int has=-1;
        if(!row.handle||!boolean(get(predicted,"get_HasPredictors"),has)){read_phase.store("prediction-unreadable");return false;}
        if(has!=0){
            // Prediction of an unrelated inventory family must not erase known
            // petals/adults. Inspect BOTH versions so optimistically removed
            // or inserted relevant rows cannot masquerade as a complete list.
            auto confirmed=get(predicted,"get_ConfirmedValue");Root confirmed_root(confirmed,strong_new,release);
            auto proposed=get(predicted,"get_PredictedValue");Root proposed_root(proposed,strong_new,release);
            if(!confirmed_root.handle||!proposed_root.handle){read_phase.store("prediction-unreadable");return false;}
            for(auto family:relevant)if(!family||is(confirmed,family)||is(proposed,family)){
                read_phase.store("prediction-active");return false;
            }
        }
    }
    int after=-1;bool ok=rows==count&&number(get(values,"get_Count"),after)&&after==count;
    read_phase.store(ok?"confirmed":"dictionary-changed");return ok;
}
bool setup() {
    if(checked)return compatible;checked=true;
    manager_class=find("Niantic.Ichigo.Inventory","InventoryManager");
    adult_item_class=find("Niantic.Ichigo.Inventory","PikminInventoryItem");
    adult_proto_class=find("Ichigo.Proto","PikminProto");
    planter_item_class=find("Niantic.Ichigo.Inventory","PlanterInventoryItem");
    planter_proto_class=find("Ichigo.Proto","PlanterProto");
    entry_class=find("Niantic.Ichigo.Game.Pikmins","PictureBookEntry");
    value_class=find("Niantic.Ichigo.Game.Pikmins","PictureBookPikminEntry");
    future_entry=provider_class?method_named(provider_class,"GetNormalOrFutureEntryByProto",1):nullptr;
    auto wrapper=find("Niantic.Ichigo.Game.Pikmins","PikminEntryProtoWrapper");
    auto count=value_class?field(value_class,"NumPlucked"):nullptr;
    auto entry=value_class?field(value_class,"EntryProtoWrapper"):nullptr;
    auto colour=wrapper?field(wrapper,"Type"):nullptr;
    auto asset=wrapper?field(wrapper,"FullAssetIdEnum"):nullptr;
    auto texture=wrapper?field(wrapper,"TextureId"):nullptr;
    compatible=field_is(provider_class,"inventoryManager",0x18)
        &&field_is(manager_class,"latestServerSideTimestamp",display_game.inventory_revision)
        &&field_is(manager_class,"itemsById",display_game.inventory_items)
        &&field_is(adult_proto_class,"pikminType_",0x18)
        &&field_is(planter_proto_class,"slot_",0x20)
        &&future_entry&&value_class&&adult_item_class&&planter_item_class
        &&field_is(entry_class,"Pikmins",0x28)
        &&count&&entry&&offset(entry)-offset(count)==8
        &&colour&&asset&&texture&&offset(asset)-offset(colour)==16&&offset(texture)-offset(colour)==20;
    return compatible;
}
bool key(void *type,Key &out) {
    if(!is(type,type_class))return false;
    int colour=read<int>(type,0x18),asset=read<int>(type,0x20),texture=read<int>(type,0x24);
    if(colour<1||colour>8||asset<=0||texture<0)return false;
    out={colour,asset,texture};return true;
}
void *confirmed(void *manager,void *item) {
    Root item_root(item,strong_new,release);if(!item_root.handle)return nullptr;
    auto id=get(item,"get_Id");Root id_root(id,strong_new,release);
    if(!id_root.handle||string_value(id).empty())return nullptr;
    auto dict=read<void*>(manager,display_game.inventory_items);Root dict_root(dict,strong_new,release);
    void *args[]{id};auto predicted=get(dict,"get_Item",1,args);Root pred_root(predicted,strong_new,release);
    int has=-1;if(!pred_root.handle||!boolean(get(predicted,"get_HasPredictors"),has)||has!=0)return nullptr;
    return get(predicted,"get_ConfirmedValue");
}
template<class Each> bool list(void *object,int limit,Each each) {
    Root root(object,strong_new,release);int n=-1;
    if(!root.handle||!number(get(object,"get_Count"),n)||n<0||n>limit)return false;
    auto method=method_named(object_class(object),"get_Item",1);if(!method)return false;
    for(int i=0;i<n;++i){void *args[]{&i};auto item=call(method,object,args);Root row(item,strong_new,release);
        if(!row.handle||!each(item))return false;}
    int after=-1;return number(get(object,"get_Count"),after)&&after==n;
}
void capture(const InsightContext &context) {
    auto manager=context.inventory;
    const int64_t revision=read<int64_t>(manager,display_game.inventory_revision);
    const auto now=std::chrono::steady_clock::now();
    if(context.session==cached_session&&manager==cached_manager&&revision==cached_revision
            &&now-captured<std::chrono::seconds(2))return;
    cached_session.clear();owned.clear();growing.clear();waiting.clear();growing_ids.clear();insights.clear();owned_complete=growing_complete=waiting_complete=false;owned_published.store(0);growing_published.store(0);
    if(revision<=0)return;
    // Get*List can omit an optimistically removed item. Certify the complete
    // dictionary too, rather than only checking the rows left in a visible list.
    const bool adults_stable=unpredicted(manager,{adult_item_class});
    const bool seeds_stable=unpredicted(manager,{planter_item_class,item_class});
    std::map<Key,int> adults,seeds;
    bool adults_ok=adults_stable&&list(get(manager,"GetPikminList"),8192,[&](void *item){
        auto value=confirmed(manager,item);Root value_root(value,strong_new,release);
        if(!is(value,adult_item_class))return false;
        auto proto=get(value,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,adult_proto_class))return false;
        Key k;if(!key(read<void*>(proto,0x18),k))return false;
        ++adults[k];return true;
    });
    std::set<std::u16string> ids;
    bool seeds_ok=seeds_stable&&list(get(manager,"GetPlanterList"),8,[&](void *item){
        auto value=confirmed(manager,item);Root value_root(value,strong_new,release);
        if(!is(value,planter_item_class))return false;
        auto proto=get(value,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,planter_proto_class))return false;
        return list(get(proto,"get_Slot"),64,[&](void *slot){
            if(!field_is(object_class(slot),"pikminSeedId_",0x18))return false;
            auto id=get(slot,"get_PikminSeedId");Root id_root(id,strong_new,release);
            if(!id_root.handle)return false;
            auto text=string_value(id);if(text.empty())return true;
            if(!ids.insert(text).second)return false;
            void *args[]{id};auto item=get(manager,"GetPikminSeed",1,args);
            auto seed_item=confirmed(manager,item);Root seed_root(seed_item,strong_new,release);
            if(!is(seed_item,item_class))return false;
            auto seed=get(seed_item,"get_Proto");Root proto(seed,strong_new,release);
            if(!is(seed,seed_class))return false;
            auto type=read<void*>(seed,0x68);if(!is(type,type_class))return false;
            // Growing seeds use the exact current/future catalog rule.
            // No rare upgrade or category-level inference is allowed.
            int colour=read<int>(type,0x18);
            auto decor=display_insights::seed_decoration(read<int>(type,0x20),read<int>(type,0x24),read<int>(type,0x2c),read<int>(type,0x30));
            if(colour<1||colour>8||decor.asset<2)return false;
            ++seeds[{colour,decor.asset,decor.texture}];return true;
        });
    });
    // Waiting is a complete confirmed seed inventory minus occupied slots.
    // Never subtract growing counts from a partial/unknown inventory.
    std::map<Key,int> pending;
    std::set<std::u16string> all_seed_ids;
    bool pending_ok=seeds_ok&&list(get(manager,"GetPikminSeedList"),8192,[&](void *item){
        auto value=confirmed(manager,item);Root value_root(value,strong_new,release);
        if(!is(value,item_class))return false;
        auto id=get(value,"get_Id");Root id_root(id,strong_new,release);
        auto text=string_value(id);
        if(text.empty()||!all_seed_ids.insert(text).second)return false;
        auto seed=get(value,"get_Proto");Root seed_root(seed,strong_new,release);
        if(!is(seed,seed_class))return false;
        if(ids.count(text))return true;
        // A seed with a planted timestamp but absent from every slot is not
        // confirmed waiting inventory. Preserve UNKNOWN for inconsistent data.
        auto planted=get(seed,"get_PlantedTimeMs");Root planted_root(planted,strong_new,release);
        auto planted_value=planted_root.handle?unbox(planted):nullptr;
        if(!planted_value||read<int64_t>(planted_value,0)!=0)return false;
        auto type=read<void*>(seed,0x68);if(!is(type,type_class))return false;
        const int colour=read<int>(type,0x18);
        auto decor=display_insights::seed_decoration(read<int>(type,0x20),read<int>(type,0x24),read<int>(type,0x2c),read<int>(type,0x30));
        if(colour<1||colour>8||decor.asset<2)return false;
        ++pending[{colour,decor.asset,decor.texture}];return true;
    });
    for(const auto &id:ids)if(!all_seed_ids.count(id))pending_ok=false;
    auto reader=context_reader.load();auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager||read<int64_t>(manager,display_game.inventory_revision)!=revision)return;
    owned=std::move(adults);growing=std::move(seeds);owned_complete=adults_ok&&unpredicted(manager,{adult_item_class});growing_complete=seeds_ok&&unpredicted(manager,{planter_item_class,item_class});owned_published.store(owned_complete?1:0);growing_published.store(growing_complete?1:0);
    if(growing_complete)growing_ids=ids;
    waiting=std::move(pending);waiting_complete=pending_ok&&growing_complete;
    cached_session=context.session;cached_manager=manager;cached_revision=revision;captured=now;
}
display_insights::CollectionInsight observe(const Binding &b) {
    using namespace display_insights;
    CollectionEvidence evidence;
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    if(context.session.empty())return collection(evidence);
    if(!setup()){evidence.reason=CollectionReason::metadata;return collection(evidence);}
    if(!is(context.inventory,manager_class)||!ready(context.inventory)
            ||!is(b.provider,provider_class)||read<void*>(b.provider,0x18)!=context.inventory)return collection(evidence);
    evidence.current_session=true;
    auto decor=seed_decoration(read<int>(b.type,0x20),read<int>(b.type,0x24),read<int>(b.type,0x2c),read<int>(b.type,0x30));
    if(decor.asset<2)return collection(evidence);
    Key expected{b.colour,decor.asset,decor.texture};
    capture(context);
    auto finish=[&]() {
        auto after=reader();
        if(after.session!=context.session||after.inventory!=context.inventory
                ||cached_revision!=read<int64_t>(context.inventory,display_game.inventory_revision))return collection(CollectionEvidence{.reason=CollectionReason::changed});
        // Exact future identity belongs to the seed itself. Picture-book
        // availability controls Seen only; it must not hide confirmed counts.
        evidence.current_session=true;evidence.exact_entry=true;
        evidence.owned_complete=owned_complete&&cached_session==context.session;
        evidence.growing_complete=growing_complete&&cached_session==context.session;
        evidence.owned=evidence.owned_complete?(owned.count(expected)?owned.at(expected):0):-1;
        evidence.growing=evidence.growing_complete?(growing.count(expected)?growing.at(expected):0):-1;
        evidence.waiting_complete=waiting_complete&&cached_session==context.session;
        evidence.waiting=evidence.waiting_complete?(waiting.count(expected)?waiting.at(expected):0):-1;
        const auto result=collection(evidence);
        if(cached_session==context.session&&cached_manager==context.inventory)insights[expected]=result;
        return result;
    };
    // capture() invalidates this cache on account/manager/revision change or
    // after two seconds. Cached Seen still passes finish()'s fresh context gate.
    auto found=insights.find(expected);
    if(found!=insights.end()) {
        evidence.seen_known=found->second.status!=CollectionStatus::unknown;
        evidence.seen=found->second.status==CollectionStatus::seen;
        evidence.reason=found->second.reason;
        return finish();
    }
    void *args[]{b.type};auto entry=call(future_entry,b.provider,args);Root entry_root(entry,strong_new,release);
    if(!is(entry,entry_class)){evidence.reason=CollectionReason::book_incomplete;return finish();}
    evidence.reason=CollectionReason::book_incomplete;
    // PikminEntryFor compares CURRENT appearance even when the category above
    // selected FUTURE decoration. Read the immutable book rows instead; no
    // proto writes, array stride assumptions, or category-level inference.
    auto rows=read<void*>(entry,0x28);Root rows_root(rows,strong_new,release);
    int count=-1;
    if(!rows_root.handle||!number(get(rows,"get_Length"),count)||count<0||count>256)return finish();
    auto iterator=get(rows,"GetEnumerator");Root iterator_root(iterator,strong_new,release);
    if(!iterator_root.handle)return finish();
    auto iterator_type=object_class(iterator);
    auto move=method_named(iterator_type,"MoveNext",0),current=method_named(iterator_type,"get_Current",0);
    if(!move||!current)return finish();
    void *self=value_type(iterator_type)?unbox(iterator):iterator;
    if(!self)return finish();
    ExactBookLookup lookup{b.colour,decor.asset,decor.texture};
    int visited=0;
    for(;;) {
        int more=-1;if(!boolean(call(move,self),more))return finish();
        if(!more)break;
        if(++visited>count)return finish();
        auto value=call(current,self);Root value_root(value,strong_new,release);
        if(!value_root.handle||!is(value,value_class))return finish();
        auto data=unbox(value);if(!data)return finish();
        lookup.observe(read<int>(data,8),read<int>(data,24),read<int>(data,28),read<int>(data,0));
    }
    int after=-1;
    evidence.seen_known=lookup.known(visited==count&&number(get(rows,"get_Length"),after)&&after==count);
    evidence.seen=lookup.seen();
    if(!evidence.seen_known&&visited==count&&after==count)
        evidence.reason=lookup.matches>1?CollectionReason::book_ambiguous:
            lookup.matches==0?CollectionReason::book_missing:CollectionReason::book_incomplete;
    return finish();
}
} // namespace collection_adapter
