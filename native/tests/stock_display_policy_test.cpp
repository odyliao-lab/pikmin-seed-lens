#include "../renderer/stock_display_policy.h"
#undef NDEBUG
#include <cassert>
int main(){using namespace display_insights;
    assert(stock_label({})==u"瓣？/？\n餘？");
    assert(stock_label({5,0,300})==u"瓣0/300\n餘300");
    assert(stock_label({0,300,300})==u"瓣300/300\n滿倉");
    assert(stock_label({4,-1,300})==u"瓣？/300\n餘？");
    assert(stock_label({4,318,300})==u"瓣318/300\n滿倉"); // confirmed game overflow is not unknown
    assert(stock_label({-1,0,-1})==u"瓣0/？\n餘？");
    assert(stock_label({4,0,0})==u"瓣0/？\n餘？");
}
