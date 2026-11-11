#pragma once
#include <string>

namespace feeding_catalogue {
// Keep string identities exact and distinct from legacy numeric IDs. Never
// invent a mapping between formats, nor use these catalogue keys as RPC ItemId.
inline std::string family(int legacy, const std::string &text) {
    if (legacy < 0) return {};
    // v152 FlowerProtoExtensions.DefaultToCommon (0x6D3DAF4) maps 0 to COMMON=5.
    // Only a successfully read empty string takes this path; failed getters
    // remain rejected by the caller. Nonempty unknown names never become common.
    if (text.empty()) return "flower-" + std::to_string(legacy == 0 ? 5 : legacy);
    if (text.size() > 80) return {};
    for (unsigned char c : text)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.')) return {};
    return "flower-s-" + text;
}
}
