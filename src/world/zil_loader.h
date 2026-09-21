// Builds the game world from the data generated out of the ZIL sources.
//
// tools/worldgen/zilgen.py parses zil/1dungeon.zil and zil/gglobals.zil and
// tools/worldgen/emit_cpp.py writes it to src/world/generated/world_data.cpp.
// The loader walks that data and creates the ZRoom and ZObject instances, so
// the room and object definitions in play are the ZIL definitions rather than
// a hand transcription of them.
#pragma once

#include "core/types.h"

#include <string_view>
#include <vector>

namespace ZilLoader {

/// Creates every <ROOM> of 1dungeon.zil with its description, flags, VALUE,
/// ACTION, GLOBAL list, PSEUDO clauses and exits, and registers them with
/// Globals.  Objects are loaded separately by loadObjects().
void loadRooms();

/// Names of routines the data refers to that have no implementation yet.
/// Source: ZilRegistry::unresolvedRoutines()
std::vector<std::string_view> unresolvedRoutines();

} // namespace ZilLoader
