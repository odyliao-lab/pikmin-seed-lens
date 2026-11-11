#pragma once

namespace pikmin {
enum class GiftDispatchType { unknown, ordinary_white, rare };

// Game156 PikminGiftProto.ItemCase: Deco=1, RareDeco=4.
// Only explicit DEFAULT_BOX=1 is the ordinary white/red-ribbon box.
inline GiftDispatchType gift_dispatch_type(bool metadata_ready, int item_case,
        bool deco, bool rare_deco, int box_type) {
    if (!metadata_ready) return GiftDispatchType::unknown;
    if (item_case == 4 && rare_deco && !deco) return GiftDispatchType::rare;
    if (item_case == 1 && deco && !rare_deco && box_type == 1)
        return GiftDispatchType::ordinary_white;
    return GiftDispatchType::unknown;
}
}
