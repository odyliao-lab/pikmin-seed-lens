#pragma once
#include <string_view>
namespace seed_root {
inline bool game_command(std::string_view value) {
    constexpr std::string_view name="com.nianticlabs.pikmin";
    if(value.size()<=name.size()||value.substr(0,name.size())!=name)return false;
    for(size_t i=name.size();i<value.size();++i)if(value[i]!='\0')return false;
    return true;
}
}
