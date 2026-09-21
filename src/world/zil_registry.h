// Resolution of the names that appear in the generated ZIL world data.
//
// tools/worldgen emits the contents of 1dungeon.zil and gglobals.zil keyed by
// ZIL name: rooms and objects, the flags they set, the globals their
// conditional exits test, and the routines named by ACTION, DESCFCN, CONTFCN,
// (DIR PER ...) and PSEUDO.  This header turns those names into the port's
// own ids, flag bits, global variables and function objects so that
// zil_loader.cpp can build the world straight from the data.
#pragma once

#include "core/flags.h"
#include "core/object.h"
#include "core/types.h"
#include "world/rooms.h"

#include <functional>
#include <string_view>
#include <vector>

namespace ZilRegistry {

/// ObjectId for a ZIL room/object name, or 0 when the name is unknown.
/// Source: src/world/generated/zil_ids.cpp
ObjectId idFor(std::string_view zilName);

/// Flag bit for a ZIL flag name ("RLANDBIT"), nullopt when unknown.
std::optional<ObjectFlag> flagFor(std::string_view zilFlagName);

/// Direction for a ZIL direction name ("NORTH", "NE", "LAND"), nullopt
/// when unknown.  ZIL: <DIRECTIONS ...> (1dungeon.zil:5)
std::optional<Direction> directionFor(std::string_view zilDirName);

/// Reads the game global that a CEXIT tests, e.g. "WON-FLAG" -> Globals::wonFlag.
/// Returns a predicate that is always false for an unknown name, which is also
/// the right answer for FALSE-FLAG (1dungeon.zil:9).
std::function<bool()> flagTestFor(std::string_view zilGlobalName);

/// ACTION of an object, or an empty function when the routine is not ported.
ZObject::ActionFunc objectActionFor(std::string_view routine);

/// ACTION of a room (called with M-BEG, M-LOOK, M-END, ...), or empty.
ZRoom::RoomActionFunc roomActionFor(std::string_view routine);

/// (DIR PER routine) exit, or empty.  ZIL FEXITs return the room to move to,
/// or 0 when the routine handled the move itself.
std::function<ObjectId()> exitRoutineFor(std::string_view routine);

/// Routine names that appear in the data but have no C++ implementation yet.
/// The loader leaves those handlers unset; Phase G of docs/ZIL_FIDELITY_TODO.md
/// fills them in.  Sorted, no duplicates.
std::vector<std::string_view> unresolvedRoutines();

} // namespace ZilRegistry
