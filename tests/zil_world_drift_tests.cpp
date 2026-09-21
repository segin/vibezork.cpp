// The live world must be exactly what the ZIL says (docs/ZIL_FIDELITY_TODO.md D4).
//
// tools/worldgen parses zil/1dungeon.zil and zil/gglobals.zil into
// tools/worldgen/zil_world.json and emits it as src/world/generated; that
// output was checked field by field against zil/COMPILED/zork1.z3.  This test
// walks the built world and compares every field against the model, so a hand
// edit to the loader, to the generated data or to an ObjectId constant fails
// the build instead of quietly changing the game.
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/world/dungeon.h"
#include "../src/world/generated/world_data.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/world/zil_registry.h"
#include "test_framework.h"

#include <algorithm>
#include <string>

namespace {

namespace zil = zork::zil;

std::string lower(std::string_view s) {
  std::string out(s);
  std::ranges::transform(out, out.begin(),
                         [](unsigned char c) { return std::tolower(c); });
  return out;
}

void setup() {
  Globals::instance().reset();
  initializeWorld();
  goSetup();
}

ZObject *live(std::string_view zilName) {
  return Globals::instance().getObject(ZilRegistry::idFor(zilName));
}

/// Flags the engine sets while the world is being built rather than reading
/// them out of the source: TOUCHBIT is bookkeeping, and the player object is
/// moved into the first room by GO.
bool runtimeFlag(ObjectFlag f) { return f == ObjectFlag::TOUCHBIT; }

} // namespace

TEST(ZilDrift_EveryRoomMatchesTheModel) {
  setup();
  for (const auto &def : zil::kRooms) {
    ZRoom *r = dynamic_cast<ZRoom *>(live(def.name));
    ASSERT_TRUE(r != nullptr);
    ASSERT_EQ(r->getDesc(), std::string(def.desc));
    ASSERT_EQ(r->getLongDesc(), std::string(def.ldesc));
    ASSERT_EQ(r->getProperty(P_VALUE), def.value ? *def.value : 0);
    ASSERT_EQ(r->hasRoomAction(), !def.action.empty() &&
                                      ZilRegistry::roomActionFor(def.action) !=
                                          nullptr);

    // Flags: exactly the source's set, give or take the runtime ones.
    for (std::string_view name : def.flags) {
      auto bit = ZilRegistry::flagFor(name);
      ASSERT_TRUE(bit.has_value());
      ASSERT_TRUE(r->hasFlag(*bit));
    }
    for (std::string_view name : zil::kFlagNames) {
      auto bit = ZilRegistry::flagFor(name);
      if (!bit || runtimeFlag(*bit)) {
        continue;
      }
      bool inSource = std::ranges::find(def.flags, name) != def.flags.end();
      if (!inSource) {
        ASSERT_FALSE(r->hasFlag(*bit));
      }
    }

    // (GLOBAL ...): same objects, same count.
    ASSERT_EQ(r->getGlobals().size(), def.globals.size());
    for (std::string_view name : def.globals) {
      ASSERT_TRUE(r->hasGlobal(ZilRegistry::idFor(name)));
    }

    // (PSEUDO "WORD" ROUTINE ...): same words, same order.
    ASSERT_EQ(r->getPseudos().size(), def.pseudo.size());
    for (std::size_t i = 0; i < def.pseudo.size(); ++i) {
      ASSERT_EQ(r->getPseudos()[i].word, std::string(def.pseudo[i].word));
    }
  }
}

TEST(ZilDrift_EveryExitMatchesTheModel) {
  setup();
  int total = 0;
  for (const auto &def : zil::kRooms) {
    ZRoom *r = dynamic_cast<ZRoom *>(live(def.name));
    for (const auto &e : def.exits) {
      auto dir = ZilRegistry::directionFor(e.dir);
      ASSERT_TRUE(dir.has_value());
      const RoomExit *x = r->getExit(*dir);
      ASSERT_TRUE(x != nullptr);
      ++total;

      switch (e.kind) {
      case zil::ExitKind::UEXIT:
        ASSERT_TRUE(x->type == ExitType::NORMAL);
        ASSERT_EQ(x->targetRoom, ZilRegistry::idFor(e.to));
        break;
      case zil::ExitKind::NEXIT:
        ASSERT_EQ(x->message, std::string(e.text));
        ASSERT_EQ(x->targetRoom, ObjectId{0});
        break;
      case zil::ExitKind::CEXIT:
        ASSERT_TRUE(x->type == ExitType::CONDITIONAL);
        ASSERT_EQ(x->targetRoom, ZilRegistry::idFor(e.to));
        ASSERT_EQ(x->message, std::string(e.text));
        ASSERT_TRUE(x->condition != nullptr);
        break;
      case zil::ExitKind::DEXIT:
        ASSERT_TRUE(x->type == ExitType::DOOR);
        ASSERT_EQ(x->targetRoom, ZilRegistry::idFor(e.to));
        ASSERT_EQ(x->doorObject, ZilRegistry::idFor(e.door));
        ASSERT_EQ(x->message, std::string(e.text));
        break;
      case zil::ExitKind::FEXIT:
        ASSERT_TRUE(x->type == ExitType::PROCEDURAL);
        ASSERT_TRUE(x->procedural != nullptr);
        break;
      }
    }

    // No exit the source does not define.
    for (std::string_view name : zil::kDirections) {
      auto dir = ZilRegistry::directionFor(name);
      bool inSource =
          std::ranges::any_of(def.exits, [&](const zil::ExitDef &e) {
            return e.dir == name;
          });
      if (!inSource) {
        ASSERT_TRUE(r->getExit(*dir) == nullptr);
      }
    }
  }
  ASSERT_EQ(total, 352);
}

TEST(ZilDrift_EveryObjectMatchesTheModel) {
  setup();
  for (const auto &def : zil::kObjects) {
    ZObject *o = live(def.name);
    ASSERT_TRUE(o != nullptr);
    ASSERT_EQ(o->getDesc(), std::string(def.desc));
    ASSERT_EQ(o->getFirstDesc(), std::string(def.fdesc));
    ASSERT_EQ(o->getLongDesc(), std::string(def.ldesc));
    ASSERT_EQ(o->getText(), std::string(def.text));

    // PROPDEF defaults from zork1.zil apply where the source is silent.
    ASSERT_EQ(o->getProperty(P_SIZE), def.size ? *def.size : 5);
    ASSERT_EQ(o->getProperty(P_CAPACITY), def.capacity ? *def.capacity : 0);
    ASSERT_EQ(o->getProperty(P_VALUE), def.value ? *def.value : 0);
    ASSERT_EQ(o->getProperty(P_TVALUE), def.tvalue ? *def.tvalue : 0);
    ASSERT_EQ(o->getProperty(P_STRENGTH), def.strength ? *def.strength : 0);

    for (std::string_view name : def.flags) {
      auto bit = ZilRegistry::flagFor(name);
      ASSERT_TRUE(bit.has_value());
      ASSERT_TRUE(o->hasFlag(*bit));
    }
    for (std::string_view name : zil::kFlagNames) {
      auto bit = ZilRegistry::flagFor(name);
      if (!bit || runtimeFlag(*bit)) {
        continue;
      }
      bool inSource = std::ranges::find(def.flags, name) != def.flags.end();
      if (!inSource) {
        ASSERT_FALSE(o->hasFlag(*bit));
      }
    }

    for (const auto &w : def.synonyms) {
      ASSERT_TRUE(o->hasSynonym(lower(w.text)));
      ASSERT_TRUE(o->hasSynonym(lower(w.key)));
    }
    for (const auto &w : def.adjectives) {
      ASSERT_TRUE(o->hasAdjective(lower(w.text)));
      ASSERT_TRUE(o->hasAdjective(lower(w.key)));
    }

    ASSERT_EQ(o->hasAction(), !def.action.empty() &&
                                  ZilRegistry::objectActionFor(def.action) !=
                                      nullptr);
  }
}

TEST(ZilDrift_EveryObjectStartsWhereTheModelSays) {
  setup();
  for (const auto &def : zil::kObjects) {
    ZObject *o = live(def.name);
    if (def.in.empty()) {
      // The player object is the one exception: GO moves WINNER to HERE.
      if (o == Globals::instance().winner) {
        continue;
      }
      ASSERT_TRUE(o->getLocation() == nullptr);
    } else {
      ASSERT_EQ(o->getLocation(), live(def.in));
    }
  }
}

TEST(ZilDrift_GlobalsAndTables) {
  setup();
  auto &g = Globals::instance();

  // ZIL: <GLOBAL name value> of 1dungeon.zil.  The conditional-exit flags all
  // start false, SCORE-MAX is 350 and both load limits are 100.
  // Source: zil/1dungeon.zil:7, 9, 1225-1237, zil/gglobals.zil:92-94
  for (const auto &gl : zil::kGlobals) {
    if (gl.name == "SCORE-MAX") {
      ASSERT_EQ(Dungeon::scoreMax(), gl.value);
      ASSERT_EQ(Dungeon::SCORE_MAX, gl.value);
    } else if (gl.name == "LOAD-MAX") {
      ASSERT_EQ(g.loadMax, gl.value);
    } else if (gl.name == "LOAD-ALLOWED") {
      ASSERT_EQ(g.loadAllowed, gl.value);
    } else {
      // Every other one is a flag that starts clear.
      ASSERT_TRUE(ZilRegistry::flagTestFor(gl.name)() == (gl.value != 0));
    }
  }

  // ZIL: <DIRECTIONS NORTH EAST WEST SOUTH NE NW SE SW UP DOWN IN OUT LAND>
  // Source: zil/1dungeon.zil:5
  ASSERT_EQ(zil::kDirections.size(), static_cast<std::size_t>(13));
  for (std::string_view name : zil::kDirections) {
    ASSERT_TRUE(ZilRegistry::directionFor(name).has_value());
  }
  // LAUNCH is not one of them: the port used to hang pseudo-exits off it, but
  // LAUNCH is a verb (gsyntax.zil:279), not a direction.
  bool hasLaunch = std::ranges::find(zil::kDirections, "LAUNCH") !=
                   zil::kDirections.end();
  ASSERT_FALSE(hasLaunch);

  // ZIL: the four walk-around LTABLEs.  Source: zil/1dungeon.zil:2620-2633
  ASSERT_EQ(Dungeon::houseAround().size(), static_cast<std::size_t>(5));
  ASSERT_EQ(Dungeon::forestAround().size(), static_cast<std::size_t>(6));
  ASSERT_EQ(Dungeon::inHouseAround().size(), static_cast<std::size_t>(4));
  ASSERT_EQ(Dungeon::aboveGround().size(), static_cast<std::size_t>(11));
  for (const auto &t : zil::kWalkTables) {
    std::span<const ObjectId> live_table =
        t.name == "HOUSE-AROUND"      ? Dungeon::houseAround()
        : t.name == "FOREST-AROUND"   ? Dungeon::forestAround()
        : t.name == "IN-HOUSE-AROUND" ? Dungeon::inHouseAround()
                                      : Dungeon::aboveGround();
    ASSERT_EQ(live_table.size(), t.rooms.size());
    for (std::size_t i = 0; i < t.rooms.size(); ++i) {
      ASSERT_EQ(live_table[i], ZilRegistry::idFor(t.rooms[i]));
    }
  }
}

TEST(ZilDrift_NoObjectOutsideTheModel) {
  setup();
  auto &g = Globals::instance();
  // Every id the world registered has to be one the model names.  This is
  // what catches an invented object being added back.
  std::vector<ObjectId> fromModel;
  for (const auto &def : zil::kRooms) {
    fromModel.push_back(ZilRegistry::idFor(def.name));
  }
  for (const auto &def : zil::kObjects) {
    fromModel.push_back(ZilRegistry::idFor(def.name));
  }
  std::ranges::sort(fromModel);
  ASSERT_EQ(fromModel.size(), static_cast<std::size_t>(250));
  ASSERT_TRUE(std::ranges::adjacent_find(fromModel) == fromModel.end());

  for (ObjectId id : fromModel) {
    ASSERT_TRUE(g.getObject(id) != nullptr);
  }
}

int main() {
  std::cout << "Running ZIL World Drift Tests" << std::endl;
  std::cout << "=============================" << std::endl;

  auto results = TestFramework::instance().runAll();

  int passed = 0, failed = 0;
  for (const auto &r : results) {
    if (r.passed)
      passed++;
    else
      failed++;
  }

  std::cout << "\nResults: " << passed << " passed, " << failed << " failed"
            << std::endl;
  return failed > 0 ? 1 : 0;
}
