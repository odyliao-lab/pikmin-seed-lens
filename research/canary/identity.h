#pragma once
#include <string>
inline bool canary_command(const std::string& value){
    const std::string expected="dev.ody.seedlensprobe";
    return value.size()>expected.size()&&value.starts_with(expected)
      &&value.find_first_not_of('\0',expected.size())==std::string::npos;
}
