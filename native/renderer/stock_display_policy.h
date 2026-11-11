#pragma once
#include <string>
namespace display_insights {
struct StockDisplay { int nectar{-1},petals{-1},capacity{-1}; };
inline std::u16string stock_label(StockDisplay s) {
    auto quantity=[](int n){if(n<0)return std::u16string(u"？");
        auto text=std::to_string(n);return std::u16string(text.begin(),text.end());};
    if(s.capacity<=0)s.capacity=-1;
    // The game keeps its original nectar count below the ball. Only add the
    // confirmed petal comparison here so four-column cards stay readable.
    auto text=u"瓣"+quantity(s.petals)+u"/"+quantity(s.capacity);
    if(s.petals>=0&&s.capacity>0)text+=s.petals>=s.capacity?u"\n滿倉":u"\n餘"+quantity(s.capacity-s.petals);
    else text+=u"\n餘？";
    return text;
}
}
