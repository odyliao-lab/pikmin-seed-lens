#pragma once
#include <string_view>
namespace seed_probe_api32 {
constexpr bool platform(int api,std::string_view sdk,std::string_view bridge){return api==32&&sdk=="32"&&bridge=="libnb.so";}
}
