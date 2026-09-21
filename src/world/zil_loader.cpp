#include "world/zil_loader.h"

#include "core/globals.h"
#include "world/objects.h"
#include "world/generated/world_data.h"
#include "world/rooms.h"
#include "world/zil_registry.h"

#include <memory>

namespace ZilLoader {
namespace {

namespace zil = zork::zil;

/// ZIL exit clause -> RoomExit.
///
/// UEXIT (DIR TO room)                               a plain exit
/// NEXIT (DIR "text")                                refusal message
/// CEXIT (DIR TO room IF flag [ELSE "text"])         passable while flag is set
/// DEXIT (DIR TO room IF door IS OPEN [ELSE "text"]) passable while door is open
/// FEXIT (DIR PER routine)                           routine names the room
/// Source: zil/1dungeon.zil, the exit clause of every <ROOM>
RoomExit buildExit(const zil::ExitDef &e) {
  switch (e.kind) {
  case zil::ExitKind::UEXIT:
    return RoomExit(ZilRegistry::idFor(e.to));

  case zil::ExitKind::NEXIT:
    return RoomExit(e.text);

  case zil::ExitKind::CEXIT:
    return RoomExit::createConditional(ZilRegistry::idFor(e.to),
                                       ZilRegistry::flagTestFor(e.flag), e.text);

  case zil::ExitKind::DEXIT: {
    RoomExit exit =
        RoomExit::createDoor(ZilRegistry::idFor(e.to), ZilRegistry::idFor(e.door));
    exit.message = e.text; // the ELSE string, printed while the door is shut
    return exit;
  }

  case zil::ExitKind::FEXIT:
    return RoomExit::createProcedural(ZilRegistry::exitRoutineFor(e.routine));
  }
  return RoomExit(ObjectId{0});
}

} // namespace

void loadRooms() {
  auto &g = Globals::instance();

  for (const auto &def : zil::kRooms) {
    const ObjectId id = ZilRegistry::idFor(def.name);
    if (id == 0) {
      continue; // emit_ids.py refuses to generate an unmapped name
    }

    auto room = std::make_unique<ZRoom>(id, def.desc, def.ldesc);

    for (std::string_view flag : def.flags) {
      if (auto bit = ZilRegistry::flagFor(flag)) {
        room->setFlag(*bit);
      }
    }

    // ZIL: (VALUE n), the points for first entering the room (gverbs.zil:2118
    // awards it through SCORE-OBJ).
    if (def.value) {
      room->setProperty(P_VALUE, *def.value);
    }

    if (auto action = ZilRegistry::roomActionFor(def.action)) {
      room->setRoomAction(std::move(action));
    }

    // ZIL: (GLOBAL ...), the objects reachable from this room even though they
    // live in GLOBAL-OBJECTS or LOCAL-GLOBALS (gparser.zil:1169-1200).
    for (std::string_view name : def.globals) {
      if (ObjectId globalId = ZilRegistry::idFor(name)) {
        room->addGlobal(globalId);
      }
    }

    // ZIL: (PSEUDO "WORD" ROUTINE ...), nouns that exist only in this room
    // (gparser.zil:1181-1194).
    for (const auto &p : def.pseudo) {
      if (auto action = ZilRegistry::objectActionFor(p.routine)) {
        room->addPseudo(p.word, std::move(action));
      }
    }

    for (const auto &e : def.exits) {
      if (auto dir = ZilRegistry::directionFor(e.dir)) {
        room->setExit(*dir, buildExit(e));
      }
    }

    g.registerObject(id, std::move(room));
  }
}

std::vector<std::string_view> unresolvedRoutines() {
  return ZilRegistry::unresolvedRoutines();
}

} // namespace ZilLoader
