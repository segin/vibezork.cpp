#include "world/zil_loader.h"

#include "core/globals.h"
#include "world/objects.h"
#include "world/generated/world_data.h"
#include "world/rooms.h"
#include "world/zil_registry.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>

namespace ZilLoader {
namespace {

namespace zil = zork::zil;

std::string lower(std::string_view s) {
  std::string out(s);
  std::ranges::transform(out, out.begin(),
                         [](unsigned char c) { return std::tolower(c); });
  return out;
}

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

void loadObjects() {
  auto &g = Globals::instance();

  // Pass 1: create every object.  Properties absent from the source are left
  // unset so ZObject::getProperty answers the PROPDEF default from zork1.zil
  // (SIZE 5, CAPACITY/VALUE/TVALUE 0).
  for (const auto &def : zil::kObjects) {
    const ObjectId id = ZilRegistry::idFor(def.name);
    if (id == 0) {
      continue;
    }

    auto obj = std::make_unique<ZObject>(id, def.desc);

    // ZIL atoms are upper case and the Z-machine dictionary is lower case;
    // ZObject matches lower case, so both the word as written and its
    // six-z-character dictionary form are registered folded down.
    for (const auto &w : def.synonyms) {
      obj->addSynonym(lower(w.text));
      if (w.key != w.text) {
        obj->addSynonym(lower(w.key));
      }
    }
    for (const auto &w : def.adjectives) {
      obj->addAdjective(lower(w.text));
      if (w.key != w.text) {
        obj->addAdjective(lower(w.key));
      }
    }

    for (std::string_view flag : def.flags) {
      if (auto bit = ZilRegistry::flagFor(flag)) {
        obj->setFlag(*bit);
      }
    }

    if (!def.fdesc.empty()) {
      obj->setFirstDesc(def.fdesc);
    }
    if (!def.ldesc.empty()) {
      obj->setLongDesc(def.ldesc);
    }
    if (!def.text.empty()) {
      obj->setText(def.text);
    }

    if (def.size) {
      obj->setProperty(P_SIZE, *def.size);
    }
    if (def.capacity) {
      obj->setProperty(P_CAPACITY, *def.capacity);
    }
    if (def.value) {
      obj->setProperty(P_VALUE, *def.value);
    }
    if (def.tvalue) {
      obj->setProperty(P_TVALUE, *def.tvalue);
    }
    if (def.strength) {
      obj->setProperty(P_STRENGTH, *def.strength);
    }
    // ZIL: (VTYPE NONLANDBIT) names the room flag a vehicle may enter
    // (gverbs.zil:2050-2062).  LOCAL-GLOBALS writes the literal 1 instead,
    // which is a compiler artifact and not a flag name.
    if (auto vtype = ZilRegistry::flagFor(def.vtype)) {
      obj->setVehicleType(*vtype);
    }

    if (auto action = ZilRegistry::objectActionFor(def.action)) {
      obj->setAction(std::move(action));
    }
    // ZIL: (DESCFCN routine) lets an object describe itself in a room listing
    // (gverbs.zil:1693-1728, M-OBJDESC).  ZObject has no DESCFCN slot yet, so
    // the two objects that use one - PATHOBJ and the BAT's BAT-D - keep their
    // ordinary description until DESCRIBE-OBJECT is ported in phase C4.
    // Source: zil/1dungeon.zil:1148, zil/gglobals.zil:15
    if (auto contfcn = ZilRegistry::objectActionFor(def.contfcn)) {
      obj->setContainerAction(std::move(contfcn));
    }

    g.registerObject(id, std::move(obj));
  }

  // Pass 2: place each object where its (IN ...) clause says.  The story file
  // links every parent's children in reverse definition order, so walking the
  // definitions backwards and appending reproduces the order FIRST?/NEXT?
  // walks see - which is the order "take all" and the room description use.
  for (auto it = zil::kObjects.rbegin(); it != zil::kObjects.rend(); ++it) {
    const auto &def = *it;
    if (def.in.empty()) {
      continue; // no (IN ...): the object starts nowhere, e.g. BROKEN-EGG
    }
    ZObject *obj = g.getObject(ZilRegistry::idFor(def.name));
    ZObject *parent = g.getObject(ZilRegistry::idFor(def.in));
    if (obj && parent) {
      obj->moveTo(parent);
    }
  }
}

std::vector<std::string_view> unresolvedRoutines() {
  return ZilRegistry::unresolvedRoutines();
}

} // namespace ZilLoader
