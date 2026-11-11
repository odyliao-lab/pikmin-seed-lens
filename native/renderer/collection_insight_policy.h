#pragma once
#include <cstdint>
#include <string>

// Display-only decisions. Adapters must resolve the exact future decoration AND
// colour, and bind every observation to the current account/session. Nothing in
// this policy authorises hatching, discarding, feeding or a game RPC.
namespace display_insights {
enum class CollectionStatus { unknown, unseen, seen };
enum class CollectionReason { waiting, metadata, identity, changed, book_missing, book_incomplete, book_ambiguous };
inline const char16_t *collection_reason(CollectionReason reason) {
    switch(reason) {
    case CollectionReason::metadata:return u"版本不符";
    case CollectionReason::identity:return u"種類不明";
    case CollectionReason::changed:return u"資料變動";
    case CollectionReason::book_missing:return u"圖鑑缺項";
    case CollectionReason::book_incomplete:return u"圖鑑未完整";
    case CollectionReason::book_ambiguous:return u"圖鑑重複";
    default:return u"等待資料";
    }
}
struct CollectionEvidence {
    bool current_session{};
    bool exact_entry{};
    bool seen_known{};
    bool seen{};
    // These are independently enumerated CURRENT inventories, not NumPlucked
    // or PictureBookTracker.GetCountForType (whose semantics are unproven).
    bool owned_complete{};
    int owned{-1};
    bool growing_complete{};
    int growing{-1};
    bool waiting_complete{};
    int waiting{-1};
    CollectionReason reason{CollectionReason::waiting};
};
struct CollectionInsight {
    CollectionStatus status{CollectionStatus::unknown};
    int owned{-1}, growing{-1};
    int waiting{-1};
    CollectionReason reason{CollectionReason::waiting};
};
struct DecorationIdentity {int asset{-1},texture{-1};};
// Mirror the reviewed game catalog rule: an already decorated type uses its
// explicit current decoration. A plain type (1) uses its explicit future one.
// Enum 0 can require a legacy string conversion; leave it unknown here.
inline DecorationIdentity seed_decoration(int current_asset,int current_texture,int future_asset,int future_texture) {
    if(current_asset>=2)return current_texture>=0?DecorationIdentity{current_asset,current_texture}:DecorationIdentity{};
    if(current_asset==1&&future_asset>=2&&future_texture>=0)return {future_asset,future_texture};
    return {};
}
// Match the confirmed future identity, never the seed's current plain appearance.
// Missing, duplicate or incomplete book rows cannot establish an unseen result.
struct ExactBookLookup {
    int colour,asset,texture;
    int matches{},plucked{-1};
    void observe(int row_colour,int row_asset,int row_texture,int row_plucked) {
        if(row_colour==colour&&row_asset==asset&&row_texture==texture) {
            ++matches;plucked=row_plucked;
        }
    }
    bool known(bool complete) const {return complete&&matches==1&&plucked>=0;}
    bool seen() const {return plucked>0;}
};
inline CollectionInsight collection(const CollectionEvidence &e) {
    CollectionInsight out;
    out.reason=e.reason;
    if (!e.current_session) return out;
    if (!e.exact_entry) {out.reason=CollectionReason::identity;return out;}
    // Null/missing entries never mean unseen. Counters never imply Seen either:
    // an undecorated owned Pikmin may not have registered the future decoration.
    if (e.seen_known) out.status=e.seen?CollectionStatus::seen:CollectionStatus::unseen;
    if (e.owned_complete && e.owned>=0) out.owned=e.owned;
    if (e.growing_complete && e.growing>=0) out.growing=e.growing;
    if (e.waiting_complete && e.waiting>=0) out.waiting=e.waiting;
    return out;
}
inline std::u16string collection_label(const CollectionInsight &value) {
    auto count=[](int n){if(n<0)return std::u16string(u"未知");
        auto ascii=std::to_string(n);return std::u16string(ascii.begin(),ascii.end());};
    const auto status=value.status==CollectionStatus::seen?u"已收集":
        value.status==CollectionStatus::unseen?u"未收集":u"收集未知";
    auto text=std::u16string(status)+u" · 持有 "+count(value.owned)+u" · 育苗 "+count(value.growing)+u" · 待種 "+count(value.waiting);
    if(value.status==CollectionStatus::unknown)text+=u"（"+std::u16string(collection_reason(value.reason))+u"）";
    if(value.owned<0||value.growing<0||value.waiting<0)text+=u" · 數量資料未完整";
    return text;
}
inline std::u16string collection_card_label(const CollectionInsight &value) {
    auto count=[](int n){if(n<0)return std::u16string(u"？");
        auto ascii=std::to_string(n);return std::u16string(ascii.begin(),ascii.end());};
    const auto status=value.status==CollectionStatus::seen?u"已收":
        value.status==CollectionStatus::unseen?u"未收":u"未知";
    return std::u16string(value.status==CollectionStatus::unknown?collection_reason(value.reason):status)+u"·持"+count(value.owned)+u"·育"+count(value.growing)+u"·待"+count(value.waiting);
}
} // namespace display_insights
