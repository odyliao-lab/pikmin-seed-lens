#include "../renderer/gift_display_policy.h"
#include <cassert>
#include <cstdio>
using namespace display_insights;
int main() {
    using K=pikmin::GiftDispatchType;
    const std::u16string source=u"<b>原始禮物盒地點</b>";
    assert(gift_recipient_identity(u"",1,u"required-id").id==u"required-id");
    assert(gift_recipient_identity(u"legacy-id",0,u"").id==u"legacy-id");
    assert(gift_recipient_identity(u"same",1,u"same").id==u"same");
    assert(gift_recipient_identity(u"old",1,u"other").reason==GiftReason::recipient_conflict);
    assert(gift_recipient_identity(u"",0,u"").reason==GiftReason::recipient_id_missing);
    for(int c:{-2,-1,2,3})assert(gift_recipient_identity(u"legacy",c,u"other").reason==GiftReason::restriction_unknown);
    assert(gift_recipient_identity(u"legacy",1,u"").reason==GiftReason::restriction_unknown);
    assert(gift_recipient_identity(u"legacy",0,u"unexpected").reason==GiftReason::restriction_unknown);
    GiftPreview g;
    assert(gift_card(source,g)==source);
    g.gift=true;assert(gift_card(source,g)==source);
    g.current=true;assert(gift_card(source,g).find(u"禮盒資訊未知")!=std::u16string::npos);
    g.kind=K::rare;g.recipient=u"不應顯示";g.decoration=u"稀有結果不應顯示";
    auto rare=gift_card(source,g);assert(rare.find(u"稀有內容未知")!=std::u16string::npos);
    assert(rare.find(u"不應顯示")==std::u16string::npos);
    g.kind=K::ordinary_white;g.recipient.clear();assert(gift_card(source,g).find(u"對象資料不足")!=std::u16string::npos);
    g.reason=GiftReason::recipient_id_missing;assert(gift_card(source,g).find(u"缺少對象編號")!=std::u16string::npos);
    g.reason=GiftReason::recipient_missing;assert(gift_card(source,g).find(u"對象尚未同步")!=std::u16string::npos);
    g.reason=GiftReason::name_missing;assert(gift_card(source,g).find(u"對象名稱未知")!=std::u16string::npos);
    g.recipient=u"台灣一號";g.decoration=u"廚師帽";
    assert(gift_card(source,g)==u"給台灣一號\n一般·廚師帽");
    g.decoration.clear();assert(gift_card(source,g)==u"給台灣一號\n一般·裝飾未知");
    int cases=0;
    for(int item=-1;item<7;++item)for(int box=-1;box<5;++box)
    for(bool deco:{false,true})for(bool other:{false,true})for(bool metadata:{false,true}) {
        auto type=pikmin::gift_dispatch_type(metadata,item,deco,other,box);
        if(type==K::ordinary_white)assert(metadata&&item==1&&deco&&!other&&box==1);
        if(type==K::rare)assert(metadata&&item==4&&!deco&&other);
        g={true,true,type,u"<b>長名字\n😀ABCDEF</b>",u"裝飾\t很長😀花苗"};
        auto label=gift_card(source,g);auto line=label.find(u'\n');
        assert(line!=std::u16string::npos&&line<=6&&label.size()-line-1<=7);
        assert(label.find(u'\t')==std::u16string::npos&&label.find(u'<')==std::u16string::npos);
        if(line)assert(!(label[line-1]>=0xd800&&label[line-1]<=0xdbff));
        g.current=false;assert(gift_card(source,g)==source);
        g.current=true;g.gift=false;assert(gift_card(source,g)==source);
        ++cases;
    }
    assert(gift_short(u"123😀678",5)==u"123…");
    std::printf("PASS D05 gift policy %d classification/current/missing/rare/title cases; no game operation\n",cases);
}
