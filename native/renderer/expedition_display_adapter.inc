// Optional read-only list footer. No scene scan, action, selector, RPC or model writes.
namespace expedition_display_adapter {
using Root=pikmin::ScopedManagedRoot;
void *(*methods)(void*,void**);
const char *(*method_name)(void*);
uint32_t (*param_count)(void*);
const void *(*param_type)(void*,uint32_t);
void *(*from_type)(const void*);
void *array_class{},*array_get{},*array_length{},*array_rank{},*loader_class{},*span_class{},*ticks_getter{},*setter{};
bool checked{},compatible{};
void configure(void *lib){
#define OPTIONAL(var,name) var=reinterpret_cast<decltype(var)>(xdl_sym(lib,name,nullptr))
    OPTIONAL(methods,"il2cpp_class_get_methods");OPTIONAL(method_name,"il2cpp_method_get_name");
    OPTIONAL(param_count,"il2cpp_method_get_param_count");OPTIONAL(param_type,"il2cpp_method_get_param");
    OPTIONAL(from_type,"il2cpp_class_from_type");
#undef OPTIONAL
}
bool setup(){
    if(checked)return compatible;checked=true;
    array_class=find("System","Array");auto integer=find("System","Int32");
    loader_class=find("Niantic.Ichigo.Utils","AsyncMessageLoader");span_class=find("System","TimeSpan");
    if(!methods||!method_name||!param_count||!param_type||!from_type||!array_class||!integer)return false;
    // GetValue(int) and GetValue(long) have the same arity. Select the exact
    // managed Int32 overload rather than guessing an array layout or stride.
    void *iterator=nullptr;int n=0;
    while(auto m=methods(array_class,&iterator)){
        if(++n>4096)return false;
        if(!strcmp(method_name(m),"GetValue")&&param_count(m)==1&&from_type(param_type(m,0))==integer){if(array_get)return false;array_get=m;}
    }
    array_length=method_named(array_class,"GetLength",1);array_rank=method_named(array_class,"get_Rank",0);
    ticks_getter=span_class?method_named(span_class,"get_Ticks",0):nullptr;setter=method_named(text_class,"set_text",1);
    compatible=collection_adapter::setup()&&gift_adapter::setup()&&array_get&&array_length&&array_rank&&ticks_getter&&setter
        &&field_is(expedition_class,"locations",0x98)&&field_is(loader_class,"text",0x30)
        &&method_named(expedition_data_class,"get_State",0)
        &&method_named(expedition_data_class,"get_RemainingDurationToExpire",0)
        &&method_named(expedition_data_class,"get_RemainingDurationToComplete",0);
    return compatible;
}
void *footer(void *card,int index){
    if(!setup()||!is(card,expedition_class)||index<0||index>=4)return nullptr;
    auto array=read<void*>(card,0x98);Root array_root(array,strong_new,release);
    if(!array_root.handle||!assignable(array_class,object_class(array)))return nullptr;
    int rank=-1,length=-1,dimension=0;void *dim_args[]{&dimension};
    if(!collection_adapter::number(call(array_rank,array),rank)||rank!=1
            ||!collection_adapter::number(call(array_length,array,dim_args),length)||length<1||length>4||index>=length)return nullptr;
    void *args[]{&index};auto loader=call(array_get,array,args);Root loader_root(loader,strong_new,release);
    if(!is(loader,loader_class))return nullptr;
    auto text=read<void*>(loader,0x30);return is(text,text_class)&&read<void*>(text,0x10)?text:nullptr;
}
struct Cached {
    display_insights::ExpeditionDisplay result;
    std::chrono::steady_clock::time_point at;
    void *manager{},*provider{};int64_t revision{};GcHandle data{};
};
std::map<std::u16string,Cached> rows;std::string session;
void clear(){for(auto &r:rows)if(r.second.data)release(r.second.data);rows.clear();}
display_insights::ExpeditionDisplay observe(const Binding &b){
    display_insights::ExpeditionDisplay unknown;
    if(!setup()||!is(b.expedition_data,expedition_data_class)||b.id.empty())return unknown;
    Root data_root(b.expedition_data,strong_new,release);
    auto reader=context_reader.load();auto context=reader?reader():InsightContext{};
    auto manager=context.inventory;
    if(context.session.empty()||!is(manager,collection_adapter::manager_class)){clear();session.clear();return unknown;}
    if(session!=context.session){clear();session=context.session;}
    if(!is(b.provider,provider_class)||read<void*>(b.provider,0x18)!=manager||!collection_adapter::ready(manager))return unknown;
    const auto revision=read<int64_t>(manager,display_game.inventory_revision);if(revision<=0)return unknown;
    auto item=read<void*>(b.expedition_data,0x78);Root item_root(item,strong_new,release);
    if(!is(item,gift_adapter::task_class)||collection_adapter::confirmed(manager,item)!=item
            ||string_value(collection_adapter::get(item,"get_Id"))!=b.id)return unknown;
    const auto now=std::chrono::steady_clock::now();auto at=now;auto old=rows.find(b.id);
    display_insights::ExpeditionDisplay result;
    if(old!=rows.end()&&old->second.manager==manager&&old->second.provider==b.provider
            &&old->second.revision==revision&&target(old->second.data)==b.expedition_data
            &&now-old->second.at<std::chrono::seconds(2)){
        result=old->second.result;at=old->second.at;
    }else{
        if(!collection_adapter::number(collection_adapter::get(b.expedition_data,"get_State"),result.state))return unknown;
        if(result.state>=0&&result.state<4){
            auto span=collection_adapter::get(b.expedition_data,result.state==0?"get_RemainingDurationToExpire":"get_RemainingDurationToComplete");Root span_root(span,strong_new,release);
            if(is(span,span_class))if(auto value=unbox(span)){
                auto boxed=call(ticks_getter,value);Root ticks_root(boxed,strong_new,release);
                if(ticks_root.handle)if(auto ticks=unbox(boxed)){result.ticks=read<int64_t>(ticks,0);result.time_known=true;}
            }
        }
        result.current=true;
    }
    auto after=reader?reader():InsightContext{};
    if(after.session!=context.session||after.inventory!=manager||read<int64_t>(manager,display_game.inventory_revision)!=revision
            ||read<void*>(b.expedition_data,0x78)!=item||collection_adapter::confirmed(manager,item)!=item)return unknown;
    if(rows.size()>=max_cards&&old==rows.end()){clear();old=rows.end();}
    auto weak=old!=rows.end()?old->second.data:0;
    if(weak&&target(weak)!=b.expedition_data){release(weak);weak=0;}
    if(!weak)weak=weak_new(b.expedition_data,false);
    if(weak)rows[b.id]={result,at,manager,b.provider,revision,weak};
    return result;
}
}
