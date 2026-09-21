// Object data built from the generated ZIL model (docs/ZIL_FIDELITY_TODO.md D2).
//
// The cases below are the object-level findings of
// docs/audit/world_data_audit.md: objects that were missing or invented,
// wrong initial locations, wrong flags, sizes, capacities and strengths,
// renamed descriptions, and first descriptions that had been stored as
// readable text.  Every expectation cites the line of zil/1dungeon.zil or
// zil/gglobals.zil it comes from.
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/world/generated/world_data.h"
#include "../src/world/objects.h"
#include "../src/world/rooms.h"
#include "../src/world/world.h"
#include "../src/world/zil_registry.h"
#include "test_framework.h"

namespace {

ZObject *obj(ObjectId id) { return Globals::instance().getObject(id); }

void setup() {
  Globals::instance().reset();
  initializeWorld();
  goSetup();
}

} // namespace

// =============================================================================
// Coverage
// =============================================================================

TEST(ZilObjects_AllObjectsLoaded) {
  setup();
  ASSERT_EQ(zork::zil::kObjects.size(), static_cast<std::size_t>(140));
  for (const auto &def : zork::zil::kObjects) {
    ObjectId id = ZilRegistry::idFor(def.name);
    ASSERT_TRUE(id != 0);
    ZObject *o = obj(id);
    ASSERT_TRUE(o != nullptr);
    ASSERT_EQ(o->getDesc(), std::string(def.desc));
  }
}

TEST(ZilObjects_PreviouslyMissingObjectsExist) {
  setup();
  // These six had no C++ counterpart at all; actions_group_a.cpp looked
  // ATTIC-TABLE up and got null.  Source: zil/1dungeon.zil:302 (KITCHEN-TABLE),
  // 310 (ATTIC-TABLE), 640 (LEAK), 695 (MACHINE-SWITCH), 704 (CONTROL-PANEL),
  // 1145 (BAT)
  ASSERT_TRUE(obj(ObjectIds::KITCHEN_TABLE) != nullptr);
  ASSERT_TRUE(obj(ObjectIds::ATTIC_TABLE) != nullptr);
  ASSERT_TRUE(obj(ObjectIds::LEAK) != nullptr);
  ASSERT_TRUE(obj(ObjectIds::MACHINE_SWITCH) != nullptr);
  ASSERT_TRUE(obj(ObjectIds::CONTROL_PANEL) != nullptr);
  ASSERT_TRUE(obj(ObjectIds::BAT) != nullptr);
}

TEST(ZilObjects_InventedObjectsGone) {
  setup();
  // A phantom 10-point "trophy" in the Treasure Room, a second bag of coins
  // and a bare "window" had no ZIL counterpart, so nothing creates them now.
  ASSERT_TRUE(obj(ObjectIds::TROPHY) == nullptr);
  ASSERT_TRUE(obj(ObjectIds::COINS) == nullptr);
  ASSERT_TRUE(obj(ObjectIds::WINDOW) == nullptr);
}

// =============================================================================
// Initial locations
// =============================================================================

TEST(ZilObjects_InitialLocations) {
  setup();
  // ZIL: (IN MAILBOX) / (IN WEST-OF-HOUSE).  Source: 1dungeon.zil:566, 606
  ASSERT_EQ(obj(ObjectIds::ADVERTISEMENT)->getLocation(), obj(ObjectIds::MAILBOX));
  ASSERT_EQ(obj(ObjectIds::MAILBOX)->getLocation(), obj(RoomIds::WEST_OF_HOUSE));

  // The port had left these with a "TODO: Set proper location".
  // Source: 1dungeon.zil:206 (ALTAR), 213 (BOOK, on the altar), 420 (FRONT-DOOR),
  // 928 (MAP), 1102 (OWNERS-MANUAL), 1016 (GUIDE), 314 (TOOL-CHEST)
  ASSERT_EQ(obj(ObjectIds::ALTAR)->getLocation(), obj(RoomIds::SOUTH_TEMPLE));
  ASSERT_EQ(obj(ObjectIds::BOOK)->getLocation(), obj(ObjectIds::ALTAR));
  ASSERT_EQ(obj(ObjectIds::FRONT_DOOR)->getLocation(), obj(RoomIds::WEST_OF_HOUSE));
  ASSERT_EQ(obj(ObjectIds::MAP)->getLocation(), obj(ObjectIds::TROPHY_CASE));
  ASSERT_EQ(obj(ObjectIds::OWNERS_MANUAL)->getLocation(), obj(RoomIds::STUDIO));
  ASSERT_EQ(obj(ObjectIds::GUIDE)->getLocation(), obj(RoomIds::DAM_LOBBY));
  ASSERT_EQ(obj(ObjectIds::TOOL_CHEST)->getLocation(), obj(RoomIds::MAINTENANCE_ROOM));

  // Objects the port had put in the wrong room.
  // Source: 1dungeon.zil:658 (BOLT in DAM-ROOM), 665-693 (the four buttons in
  // MAINTENANCE-ROOM), 1053 (AXE held by the troll), 2898 (EGG in the NEST)
  ASSERT_EQ(obj(ObjectIds::BOLT)->getLocation(), obj(RoomIds::DAM_ROOM));
  ASSERT_EQ(obj(ObjectIds::YELLOW_BUTTON)->getLocation(),
            obj(RoomIds::MAINTENANCE_ROOM));
  ASSERT_EQ(obj(ObjectIds::BLUE_BUTTON)->getLocation(),
            obj(RoomIds::MAINTENANCE_ROOM));
  ASSERT_EQ(obj(ObjectIds::AXE)->getLocation(), obj(ObjectIds::TROLL));
  ASSERT_EQ(obj(ObjectIds::EGG)->getLocation(), obj(ObjectIds::NEST));

  // ZIL: (IN KITCHEN-TABLE) - not loose on the kitchen floor.
  // Source: 1dungeon.zil:287 (SANDWICH-BAG), 296 (BOTTLE)
  ASSERT_EQ(obj(ObjectIds::SANDWICH_BAG)->getLocation(),
            obj(ObjectIds::KITCHEN_TABLE));
  ASSERT_EQ(obj(ObjectIds::BOTTLE)->getLocation(), obj(ObjectIds::KITCHEN_TABLE));

  // Objects with no (IN ...) clause start nowhere and appear during play.
  // Source: 1dungeon.zil:132 (BROKEN-EGG), 184 (DIAMOND), 613 (GUNK)
  ASSERT_TRUE(obj(ObjectIds::BROKEN_EGG)->getLocation() == nullptr);
  ASSERT_TRUE(obj(ObjectIds::DIAMOND)->getLocation() == nullptr);
  ASSERT_TRUE(obj(ObjectIds::GUNK)->getLocation() == nullptr);

  // The globals live in the two parent objects, not in a room.
  // Source: zil/gglobals.zil:7-24, 1dungeon.zil:13 (BOARD), 28 (WATER)
  ASSERT_EQ(obj(ObjectIds::BOARD)->getLocation(), obj(ObjectIds::LOCAL_GLOBALS));
  ASSERT_EQ(obj(ObjectIds::GRUE)->getLocation(), obj(ObjectIds::GLOBAL_OBJECTS));
}

// =============================================================================
// Flags, properties and vocabulary
// =============================================================================

TEST(ZilObjects_Flags) {
  setup();
  // ZIL: (FLAGS TAKEBIT TRYTAKEBIT ...) - both, so the parser does no
  // implicit take but the object is still takeable.
  // Source: 1dungeon.zil:361 (CHALICE), 921 (SWORD)
  ASSERT_TRUE(obj(ObjectIds::CHALICE)->hasFlag(ObjectFlag::TAKEBIT));
  ASSERT_TRUE(obj(ObjectIds::CHALICE)->hasFlag(ObjectFlag::TRYTAKEBIT));
  ASSERT_TRUE(obj(ObjectIds::SWORD)->hasFlag(ObjectFlag::TRYTAKEBIT));

  // ZIL: the trophy case starts closed; the port had set OPENBIT.
  // Source: 1dungeon.zil:343
  ASSERT_FALSE(obj(ObjectIds::TROPHY_CASE)->hasFlag(ObjectFlag::OPENBIT));
  ASSERT_TRUE(obj(ObjectIds::TROPHY_CASE)->hasFlag(ObjectFlag::TRANSBIT));

  // ZIL: the villains carry no FIGHTBIT until a fight starts.
  // Source: 1dungeon.zil:391, 972, 1041
  ASSERT_FALSE(obj(ObjectIds::TROLL)->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_FALSE(obj(ObjectIds::THIEF)->hasFlag(ObjectFlag::FIGHTBIT));
  ASSERT_TRUE(obj(ObjectIds::THIEF)->hasFlag(ObjectFlag::INVISIBLE));

  // Flags the port had dropped.  Source: 1dungeon.zil:266 (BOOK CONTBIT),
  // 932 (MAP INVISIBLE), 30 (WATER DRINKBIT), 429 (BARROW-DOOR OPENBIT)
  ASSERT_TRUE(obj(ObjectIds::BOOK)->hasFlag(ObjectFlag::CONTBIT));
  ASSERT_TRUE(obj(ObjectIds::MAP)->hasFlag(ObjectFlag::INVISIBLE));
  ASSERT_TRUE(obj(ObjectIds::WATER)->hasFlag(ObjectFlag::DRINKBIT));
  ASSERT_TRUE(obj(ObjectIds::BARROW_DOOR)->hasFlag(ObjectFlag::OPENBIT));
}

TEST(ZilObjects_Properties) {
  setup();
  // ZIL: <PROPDEF SIZE 5> in zork1.zil, so an object with no (SIZE n) weighs 5.
  ASSERT_EQ(obj(ObjectIds::SWORD)->getProperty(P_SIZE), 30);   // 1dungeon.zil:925
  ASSERT_EQ(obj(ObjectIds::GARLIC)->getProperty(P_SIZE), 4);   // 1dungeon.zil:374
  ASSERT_EQ(obj(ObjectIds::ADVERTISEMENT)->getProperty(P_SIZE), 2);
  ASSERT_EQ(obj(ObjectIds::KNIFE)->getProperty(P_SIZE), 5);    // no (SIZE n)

  // ZIL: (CAPACITY n).  Source: 1dungeon.zil:345 (TROPHY-CASE 10000),
  // 294 (SANDWICH-BAG 9), 611 (MAILBOX 10)
  ASSERT_EQ(obj(ObjectIds::TROPHY_CASE)->getProperty(P_CAPACITY), 10000);
  ASSERT_EQ(obj(ObjectIds::SANDWICH_BAG)->getProperty(P_CAPACITY), 9);
  ASSERT_EQ(obj(ObjectIds::MAILBOX)->getProperty(P_CAPACITY), 10);
  // The lamp holds nothing: its fuel is I-LANTERN, not a capacity.
  ASSERT_EQ(obj(ObjectIds::LAMP)->getProperty(P_CAPACITY), 0);

  // ZIL: (STRENGTH n) belongs to the fighters only.
  // Source: 1dungeon.zil:393, 977, 1046, gglobals.zil:271
  ASSERT_EQ(obj(ObjectIds::TROLL)->getProperty(P_STRENGTH), 2);
  ASSERT_EQ(obj(ObjectIds::THIEF)->getProperty(P_STRENGTH), 5);
  ASSERT_EQ(obj(ObjectIds::CYCLOPS)->getProperty(P_STRENGTH), 10000);
  ASSERT_EQ(obj(ObjectIds::ADVENTURER)->getProperty(P_STRENGTH), 0);
  // STRENGTH is not a weapon's damage or a light source's fuel.
  ASSERT_EQ(obj(ObjectIds::AXE)->getProperty(P_STRENGTH), 0);
  ASSERT_EQ(obj(ObjectIds::LAMP)->getProperty(P_STRENGTH), 0);

  // ZIL: (VTYPE NONLANDBIT) - the boat may enter water rooms.
  // Source: 1dungeon.zil:736 and GO (1dungeon.zil:2643)
  auto vtype = obj(ObjectIds::BOAT_INFLATED)->getVehicleType();
  ASSERT_TRUE(vtype.has_value());
  ASSERT_TRUE(*vtype == ObjectFlag::NONLANDBIT);
}

TEST(ZilObjects_TreasureValues) {
  setup();
  // The 350 points: object VALUE 143 + TVALUE 132 + room VALUE 65, less the
  // mutually exclusive broken egg (2) and broken canary (1), plus the
  // 13-point lit Drafty Room bonus (1actions.zil:2578).
  int value = 0, tvalue = 0;
  for (const auto &def : zork::zil::kObjects) {
    ZObject *o = obj(ZilRegistry::idFor(def.name));
    value += o->getProperty(P_VALUE);
    tvalue += o->getProperty(P_TVALUE);
  }
  ASSERT_EQ(value, 143);
  ASSERT_EQ(tvalue, 132);

  int roomValue = 0;
  for (const auto &def : zork::zil::kRooms) {
    roomValue += obj(ZilRegistry::idFor(def.name))->getProperty(P_VALUE);
  }
  ASSERT_EQ(roomValue, 65);
  ASSERT_EQ(value + tvalue + roomValue - 2 - 1 + 13, 350);
}

TEST(ZilObjects_DescriptionsAndText) {
  setup();
  // Descriptions the port had renamed.  Source: gglobals.zil:269 (cretin),
  // 1dungeon.zil:359 (chalice), 947 (torch), 931 (ancient map), 423 (door),
  // 349 (carpet), 607 (leather bag of coins), gglobals.zil:188 (lurking grue)
  ASSERT_EQ(obj(ObjectIds::ADVENTURER)->getDesc(), "cretin");
  ASSERT_EQ(obj(ObjectIds::CHALICE)->getDesc(), "chalice");
  ASSERT_EQ(obj(ObjectIds::TORCH)->getDesc(), "torch");
  ASSERT_EQ(obj(ObjectIds::MAP)->getDesc(), "ancient map");
  ASSERT_EQ(obj(ObjectIds::FRONT_DOOR)->getDesc(), "door");
  ASSERT_EQ(obj(ObjectIds::RUG)->getDesc(), "carpet");
  ASSERT_EQ(obj(ObjectIds::GRUE)->getDesc(), "lurking grue");

  // ZIL: (FDESC ...) is the description used until the object is touched; it
  // is not the READ text.  Source: 1dungeon.zil:788 (BUOY), 2892 (EGG)
  ASSERT_EQ(obj(ObjectIds::BUOY)->getFirstDesc(),
            "There is a red buoy here (probably a warning).");
  ASSERT_TRUE(obj(ObjectIds::BUOY)->getText().empty());

  // ZIL: (TEXT ...) belongs to what can be read.
  // Source: 1dungeon.zil:572 (ADVERTISEMENT), 1104 (OWNERS-MANUAL)
  ASSERT_TRUE(obj(ObjectIds::ADVERTISEMENT)->getText().find(
                  "WELCOME TO ZORK!") != std::string::npos);
  ASSERT_TRUE(obj(ObjectIds::OWNERS_MANUAL)->getText().find(
                  "ZORK I: The Great Underground Empire") != std::string::npos);
  // A ZIL string's line breaks are spaces; only "|" is a newline.
  ASSERT_TRUE(obj(ObjectIds::OWNERS_MANUAL)->getText().find(
                  "self-contained and self-maintaining universe") !=
              std::string::npos);
}

TEST(ZilObjects_Vocabulary) {
  setup();
  // ZIL: (SYNONYM DAM GATE GATES FCD#3) - the parser sees the dictionary
  // form of the last one, cut at six z-characters.
  // Source: zil/1dungeon.zil:399
  ZObject *dam = obj(ObjectIds::DAM);
  ASSERT_TRUE(dam->hasSynonym("dam"));
  ASSERT_TRUE(dam->hasSynonym("gate"));
  ASSERT_TRUE(dam->hasSynonym("gates"));
  ASSERT_TRUE(dam->hasSynonym("fcd#"));

  // ZIL: (ADJECTIVE BEAUTIFUL) is stored truncated in the story file; both
  // spellings resolve.  Source: 1dungeon.zil:50 (WHITE-HOUSE BEAUTI/COLONI)
  ZObject *house = obj(ObjectIds::WHITE_HOUSE);
  ASSERT_TRUE(house->hasAdjective("beauti"));
  ASSERT_TRUE(house->hasAdjective("coloni"));

  // ZIL: (SYNONYM ... OVERBOARD) on the teeth, so "throw x overboard" on
  // land reaches the teeth.  Source: 1dungeon.zil:21
  ASSERT_TRUE(obj(ObjectIds::TEETH)->hasSynonym("overbo"));

  // ZIL: ME carries the first-person words, not the ADVENTURER object.
  // Source: zil/gglobals.zil:216
  ASSERT_TRUE(obj(ObjectIds::ME)->hasSynonym("me"));
  ASSERT_TRUE(obj(ObjectIds::ME)->hasSynonym("myself"));
  ASSERT_FALSE(obj(ObjectIds::ADVENTURER)->hasSynonym("me"));
}

TEST(ZilObjects_ChildOrderFollowsTheStoryFile) {
  setup();
  // The story file links a parent's children in reverse definition order, so
  // the kitchen table holds the bottle before the sack: SANDWICH-BAG is
  // defined first (1dungeon.zil:285) and BOTTLE second (1dungeon.zil:293).
  auto contents = obj(ObjectIds::KITCHEN_TABLE)->getContents();
  ASSERT_EQ(contents.size(), static_cast<std::size_t>(2));
  ASSERT_EQ(contents[0]->getId(), ObjectIds::BOTTLE);
  ASSERT_EQ(contents[1]->getId(), ObjectIds::SANDWICH_BAG);
}

TEST(ZilObjects_ActionsBound) {
  setup();
  // A sample of the (ACTION ...) clauses.  Source: 1dungeon.zil:612
  // (MAILBOX-F), 344 (TROPHY-CASE-FCN), 1042 (TROLL-FCN), 791 (TREASURE-INSIDE)
  ASSERT_TRUE(obj(ObjectIds::MAILBOX)->hasAction());
  ASSERT_TRUE(obj(ObjectIds::TROPHY_CASE)->hasAction());
  ASSERT_TRUE(obj(ObjectIds::TROLL)->hasAction());
  ASSERT_TRUE(obj(ObjectIds::BUOY)->hasAction());
  // An object with no (ACTION ...) has none.  Source: 1dungeon.zil:26 (WALL)
  ASSERT_FALSE(obj(ObjectIds::WALL)->hasAction());
}

int main() {
  std::cout << "Running ZIL Object Data Tests" << std::endl;
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
