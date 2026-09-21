// GENERATED FILE - DO NOT EDIT.
// Produced by tools/worldgen/emit_ids.py from zil/1dungeon.zil, zil/gglobals.zil, src/world/rooms.h, src/world/objects.h.
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.

#pragma once
#include "core/types.h"
#include <span>
#include <string_view>

namespace zork::zil {

/// One ZIL name and the port ObjectId constant it denotes.
struct NameId { std::string_view name; ObjectId id; };

/// Every ROOM and OBJECT of 1dungeon.zil and gglobals.zil, definition order.
extern const std::span<const NameId> kNameIds;

}  // namespace zork::zil
