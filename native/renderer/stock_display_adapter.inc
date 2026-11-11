namespace stock_adapter {
using collection_adapter::get;
using collection_adapter::number;
using collection_adapter::confirmed;
using collection_adapter::list;
using Root=pikmin::ScopedManagedRoot;
void *manager_class{},*petal_class{},*petal_item_class{},*capacity_item_class{},*kind_to_text{},*flower_to_honey{};
bool checked{},compatible{};
std::atomic<const char*> read_phase{"not-read"};
std::string session;
void *manager{};
int64_t revision{};
std::chrono::steady_clock::time_point captured{};
std::map<std::pair<std::string,int>,int> petals;
bool complete{};
int capacity{-1};
bool setup() {
    if(checked)return compatible;checked=true;
    manager_class=find("Niantic.Ichigo.Inventory","InventoryManager");
    petal_class=find("Ichigo.Proto","FlowerPetalProto");
    petal_item_class=find("Niantic.Ichigo.Inventory","FlowerPetalInventoryItem");
    capacity_item_class=find("Niantic.Ichigo.Inventory","ItemCapacityInventoryItem");
    auto mapping=find("Niantic.Ichigo.ModelControllers.Flowers","FlowerKindLookup");
    auto conversion=find("Niantic.Ichigo.Utils.Proto","HoneyBallProtoExtensions");
    kind_to_text=mapping?method_named(mapping,"KindToObsoleteString",1):nullptr;
    flower_to_honey=conversion?method_named(conversion,"ToHoneyType",1):nullptr;
    compatible=honey_metadata&&field_is(manager_class,"latestServerSideTimestamp",display_game.inventory_revision)
        &&field_is(manager_class,"itemsById",display_game.inventory_items)&&petal_class&&petal_item_class&&capacity_item_class&&kind_to_text&&flower_to_honey;
    return compatible;
}
void *static_call(void *method,void **args) {
    if(!method)return nullptr;void *exception=nullptr;
    auto result=invoke(method,nullptr,args,&exception);return exception?nullptr:result;
}
std::string family(void *proto,bool nectar) {
    int legacy=-1;if(!number(get(proto,nectar?"get_HoneyFlowerKind":"get_Kind"),legacy)||legacy<0)return {};
    auto value=get(proto,"get_FlowerKind");Root root(value,strong_new,release);
    if(!root.handle)return {};auto text=string_value(value);
    if(text.empty()&&legacy!=0&&legacy!=5){void *args[]{&legacy};text=string_value(static_call(kind_to_text,args));}
    std::string ascii;for(auto c:text){if(c==0||c>127||ascii.size()>=80)return {};ascii+=static_cast<char>(c);}
    return feeding_catalogue::family(legacy,ascii);
}
void capture(const InsightContext &ctx) {
    auto inv=ctx.inventory;auto rev=read<int64_t>(inv,display_game.inventory_revision);auto now=std::chrono::steady_clock::now();
    if(ctx.session==session&&inv==manager&&revision==rev&&now-captured<std::chrono::seconds(2))return;
    session.clear();petals.clear();complete=false;capacity=-1;
    if(rev<=0)return;
    read_phase.store("checking-predictions");
    if(!collection_adapter::unpredicted(inv,{petal_item_class,capacity_item_class})) {
        session=ctx.session;manager=inv;revision=rev;captured=now;return;
    }
    std::map<std::pair<std::string,int>,int> rows;int cap=-1;
    read_phase.store("capacity-list");
    bool caps_ok=list(get(inv,"GetItemCapacityList"),1,[&](void *item){
        auto value=confirmed(inv,item);Root value_root(value,strong_new,release);
        if(!is(value,capacity_item_class))return false;
        auto proto=get(value,"get_Proto");Root proto_root(proto,strong_new,release);
        auto petal=get(proto,"get_Petal");Root petal_root(petal,strong_new,release);
        return number(get(petal,"get_CurrentCount"),cap)&&cap>0&&cap<=1000000;
    });
    read_phase.store("petal-list");
    bool rows_ok=list(get(inv,"GetFlowerPetalList"),512,[&](void *item){
        auto value=confirmed(inv,item);Root value_root(value,strong_new,release);
        if(!is(value,petal_item_class))return false;
        auto proto=get(value,"get_Proto");Root proto_root(proto,strong_new,release);
        if(!is(proto,petal_class))return false;
        int count=-1,flower=-1,colour=-1;
        if(!number(get(proto,"get_NumPetal"),count)||count<0||count>1000000
                ||!number(get(proto,"get_FlowerType"),flower)||flower<0||flower>7)return false;
        void *args[]{&flower};if(!number(static_call(flower_to_honey,args),colour)||colour<1||colour>4)return false;
        auto kind=family(proto,false);if(kind.empty())return false;
        return rows.emplace(std::make_pair(kind,colour),count).second;
    });
    auto reader=context_reader.load();auto after=reader?reader():InsightContext{};
    if(after.session!=ctx.session||after.inventory!=inv||rev!=read<int64_t>(inv,display_game.inventory_revision)||!collection_adapter::unpredicted(inv,{petal_item_class,capacity_item_class}))return;
    petals=std::move(rows);complete=rows_ok;capacity=caps_ok?cap:-1;
    read_phase.store(caps_ok?(rows_ok?"complete":"petal-incomplete"):(rows_ok?"capacity-incomplete":"both-incomplete"));
    session=ctx.session;manager=inv;revision=rev;captured=now;
}
display_insights::StockDisplay observe(const Binding &b) {
    display_insights::StockDisplay out;
    auto reader=context_reader.load();auto ctx=reader?reader():InsightContext{};
    if(ctx.session.empty()||!setup()||!is(ctx.inventory,manager_class)||!collection_adapter::ready(ctx.inventory))return out;
    const auto before=read<int64_t>(ctx.inventory,display_game.inventory_revision);
    auto item=confirmed(ctx.inventory,b.nectar_item);Root item_root(item,strong_new,release);
    if(!is(item,honey_item_class))return out;
    auto proto=get(item,"get_Proto");Root proto_root(proto,strong_new,release);
    if(!is(proto,honey_proto_class))return out;
    int colour=-1,count=-1;
    if(!number(get(proto,"get_HoneyType"),colour)||colour<1||colour>4)return out;
    auto kind=family(proto,true);if(kind.empty())return out;
    if(number(get(proto,"get_NumBalls"),count)&&count>=0&&count<=1000000)out.nectar=count;
    capture(ctx);
    if(session==ctx.session&&manager==ctx.inventory){
        out.capacity=capacity;
        if(complete){auto found=petals.find({kind,colour});out.petals=found==petals.end()?0:found->second;}
    }
    auto after=reader();if(after.session!=ctx.session||after.inventory!=ctx.inventory
            ||before<=0||before!=read<int64_t>(ctx.inventory,display_game.inventory_revision)||revision!=before)return {};
    return out;
}
} // namespace stock_adapter
