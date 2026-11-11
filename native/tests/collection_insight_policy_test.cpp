#include "../renderer/collection_insight_policy.h"
#undef NDEBUG
#include <cassert>
#include <initializer_list>
int main() {
    using namespace display_insights;
    // Exhaustive independent gates, including zero vs unavailable counts.
    for (bool session:{false,true}) for (bool exact:{false,true})
    for (bool known:{false,true}) for (bool seen:{false,true})
    for (bool owned:{false,true}) for (bool growing:{false,true})
    for (int count:{-1,0,2}) {
        CollectionEvidence e{session,exact,known,seen,owned,count,growing,count};
        auto a=collection(e);
        assert(a.status==(!session||!exact||!known?CollectionStatus::unknown:
                         seen?CollectionStatus::seen:CollectionStatus::unseen));
        assert(a.owned==(session&&exact&&owned&&count>=0?count:-1));
        assert(a.growing==(session&&exact&&growing&&count>=0?count:-1));
    }
    // Plain current appearance misses a decorated category's exact future row.
    ExactBookLookup plain{1,1,0},future{1,37,4};
    for(auto *lookup:{&plain,&future}) {
        lookup->observe(2,37,4,9); // wrong colour
        lookup->observe(1,37,3,8); // wrong texture
        lookup->observe(1,38,4,7); // wrong asset
        lookup->observe(1,37,4,2);
    }
    assert(!plain.known(true)&&future.known(true)&&future.seen());
    assert(!future.known(false));
    future.observe(1,37,4,2);assert(!future.known(true));
    ExactBookLookup unseen{2,37,5};unseen.observe(2,37,5,0);
    assert(unseen.known(true)&&!unseen.seen());
    ExactBookLookup missing{2,37,5};assert(!missing.known(true));
    missing.observe(2,37,5,-1);assert(!missing.known(true));
    CollectionEvidence e{true,true,true,true,true,0,true,1};
    auto a=collection(e);assert(a.status==CollectionStatus::seen&&a.owned==0&&a.growing==1);
    e.seen_known=false;e.owned=3;
    assert(collection(e).status==CollectionStatus::unknown); // not inferred from stock
    e.current_session=false;
    assert(collection(e).owned==-1); // logout invalidates all display evidence
    assert(collection_label(collection(e))==u"收集未知 · 持有 未知 · 育苗 未知 · 待種 未知（等待資料） · 數量資料未完整");
    assert(collection_label({CollectionStatus::unseen,0,0,0})==u"未收集 · 持有 0 · 育苗 0 · 待種 0");
    assert(collection_label({CollectionStatus::seen,3,-1}).find(u"數量資料未完整")!=std::u16string::npos);
    assert(collection_card_label({CollectionStatus::unknown,-1,-1})==u"等待資料·持？·育？·待？");
    assert(collection_card_label({CollectionStatus::unseen,0,0,0})==u"未收·持0·育0·待0");
    assert(collection_card_label({CollectionStatus::seen,3,-1})==u"已收·持3·育？·待？");
    for(bool session:{false,true})for(bool exact:{false,true})
    for(bool complete:{false,true})for(int count:{-1,0,6}) {
        CollectionEvidence sample{session,exact,false,false,true,2,true,1,complete,count,CollectionReason::book_missing};
        auto result=collection(sample);
        assert(result.waiting==(session&&exact&&complete&&count>=0?count:-1));
        if(session&&exact)assert(result.status==CollectionStatus::unknown&&result.owned==2&&result.growing==1);
    }
    for(auto reason:{CollectionReason::waiting,CollectionReason::metadata,CollectionReason::identity,
            CollectionReason::changed,CollectionReason::book_missing,CollectionReason::book_incomplete,CollectionReason::book_ambiguous}) {
        CollectionEvidence sample{true,true,false,false,true,2,true,1,true,6,reason};
        const auto result=collection(sample);
        assert(result.waiting==6&&result.reason==reason);
        assert(collection_card_label(result).find(collection_reason(reason))==0);
    }
    assert(seed_decoration(1,0,42,3).asset==42);
    assert(seed_decoration(42,3,99,8).asset==42); // explicit assigned decoration, no rare prediction
    assert(seed_decoration(42,3,0,0).texture==3); // gold seed current decoration
    assert(seed_decoration(0,0,42,3).asset==-1); // legacy string unresolved
    assert(seed_decoration(1,0,0,0).asset==-1);
    assert(seed_decoration(42,-1,99,0).asset==-1); // no silent future fallback
}
