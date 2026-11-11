#pragma once
#include <string_view>

// Research admission only. API31 is an experimental test target, NOT a product
// qualification. No default, ranges, coercion, or caller-supplied actual API.
namespace seed_probe {
enum class Abi { Arm64, X64 };
constexpr int requestedApi(std::string_view value) {
    if (value == "31") return 31;
    if (value == "34") return 34;
    if (value == "35") return 35;
    return -1;
}
constexpr bool admit(std::string_view expected, int actual, Abi abi) {
    if (requestedApi(expected) != actual || actual < 0) return false;
    if (abi == Abi::Arm64) return actual == 31 || actual == 34;
    if (abi == Abi::X64) return actual == 31 || actual == 35;
    return false;
}
}
