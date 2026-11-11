#pragma once
#include "gift_dispatch_policy.h"
#include "seed_preview_policy.h"
#include <string>

namespace display_insights {
enum class GiftReason { none, recipient_id_missing, recipient_missing, name_missing, restriction_unknown, recipient_conflict };
struct GiftRecipientIdentity {
    std::u16string id;
    GiftReason reason=GiftReason::recipient_id_missing;
};
// Game157 AllowsPikminWithId reads the explicit expedition restriction. A
// legacy decoration ID may be absent, but two conflicting IDs are not guessed.
inline GiftRecipientIdentity gift_recipient_identity(const std::u16string &decoration_id,
        int condition,const std::u16string &allowed_id) {
    if(condition<0||condition>1)return {{},GiftReason::restriction_unknown};
    if((condition==0&&!allowed_id.empty())||(condition==1&&allowed_id.empty()))
        return {{},GiftReason::restriction_unknown};
    if(condition==1&&!decoration_id.empty()&&decoration_id!=allowed_id)
        return {{},GiftReason::recipient_conflict};
    auto id=condition==1?allowed_id:decoration_id;
    return {id,id.empty()?GiftReason::recipient_id_missing:GiftReason::none};
}
// A confirmed task's explicit gift only. No future/rare reward inference.
struct GiftPreview {
    bool current{},gift{};
    pikmin::GiftDispatchType kind=pikmin::GiftDispatchType::unknown;
    std::u16string recipient,decoration;
    GiftReason reason=GiftReason::none;
};
inline std::u16string gift_short(std::u16string s,size_t limit) {
    s=seed_preview::plain_name(s);
    for(auto &c:s)if(c==u'\n'||c==u'\r'||c==u'\t')c=u' ';
    if(s.size()<=limit)return s;
    auto n=limit-1;if(n&&s[n-1]>=0xd800&&s[n-1]<=0xdbff)--n;
    return s.substr(0,n)+u"…";
}
inline std::u16string gift_card(const std::u16string &source,const GiftPreview &g) {
    if(!g.current||!g.gift)return source;
    const auto title=gift_short(source,6);
    if(g.kind==pikmin::GiftDispatchType::rare)return title+u"\n稀有內容未知";
    if(g.kind!=pikmin::GiftDispatchType::ordinary_white)return title+u"\n禮盒資訊未知";
    if(seed_preview::plain_name(g.recipient).empty()) {
        const auto why=g.reason==GiftReason::recipient_id_missing?u"缺少對象編號":
            g.reason==GiftReason::recipient_missing?u"對象尚未同步":
            g.reason==GiftReason::name_missing?u"對象名稱未知":
            g.reason==GiftReason::restriction_unknown?u"對象條件未知":
            g.reason==GiftReason::recipient_conflict?u"對象編號衝突":u"對象資料不足";
        return title+u"\n"+why;
    }
    const auto name=u"給"+gift_short(g.recipient,5);
    return name+u"\n"+(seed_preview::plain_name(g.decoration).empty()?u"一般·裝飾未知":u"一般·"+gift_short(g.decoration,4));
}
}
