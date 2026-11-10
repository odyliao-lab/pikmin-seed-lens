#pragma once
#include <string_view>
// Research-only exact tuple. Does not participate in the product dispatcher.
namespace seed_probe_api34 {
constexpr bool platform(int api,std::string_view sdk,std::string_view bridge){
    return api==34&&sdk=="34"&&bridge=="libndk_translation.so";
}
}
