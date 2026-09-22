#include "world/zil_registry.h"

#include "core/globals.h"
#include "world/dungeon.h"
#include "world/generated/zil_ids.h"
#include "world/objects.h"
#include "core/gglobals.h"
#include "world/pseudo_actions.h"
#include "world/villains.h"

#include <algorithm>
#include <string>
#include <functional>
#include <unordered_map>

// Action routines ported from zil/1actions.zil and zil/gglobals.zil.  They are
// scattered over several translation units and most are not declared in a
// header, so they are declared here with the signatures their definitions use.
// A bool-returning routine is a ZIL RFALSE/RTRUE handler; an int-returning one
// can also answer M-FATAL.
// clang-format off
bool axeAction(); bool bagOfCoinsAction(); bool barrowAction(); bool barrowDoorAction();
bool basketAction(); bool batAction(); bool bellAction(); bool boardAction();
bool boardedWindowAction(); bool bodyAction(); bool boltAction(); bool bottleAction();
bool bubbleAction(); bool buttonAction(); bool canaryAction(); int candlesAction();
bool canyonViewAction(); bool chaliceAction(); bool chimneyAction(); bool crackAction();
bool damAction(); bool eggAction();
bool forestAction(); bool frontDoorAction(); bool garlicAction(); bool ghostsAction();
bool graniteWallAction(); bool grateAction();
bool gunkAction(); bool hotBellAction(); bool iboatFunction(); bool inflatedBoatAction();
bool kitchenWindowAction(); bool knifeAction(); int lampAction(); bool largeBagAction();
bool leakFunction(); bool machineAction(); bool machineSwitchAction(); bool mailboxAction();
bool matchesAction(); bool mirrorAction(); bool mountainRangeAction();
bool paintingAction(); bool puncturedBoatAction(); bool puttyAction();
bool rainbowAction(); bool riverAction(); bool robberAction(); bool ropeAction();
bool rugAction(); bool rustyKnifeAction(); bool sandAction();
bool sandwichBagAction(); bool sceptreAction(); bool slideAction(); bool songbirdAction();
bool stilettoAction(); bool swordAction(); bool teethAction();
bool toolChestAction(); bool torchAction(); bool trapDoorAction();
bool trophyCaseAction(); bool trunkAction(); bool tubeAction(); bool waterAction();
bool whiteHouseAction();

int batsRoom(int); int behindHouseAction(int); int boomRoom(int); int cave2Room(int);
int cellarAction(int); int clearingAction(int); int cyclopsRoomAction(int);
int damRoomAction(int); int deepCanyonRoomAction(int); int domeRoomAction(int);
int fallsRoom(int); int forestRoom(int); int kitchenAction(int); int livingRoomAction(int);
int lldRoom(int); int loudRoomAction(int); int machineRoomAction(int); int maze11Action(int);
int mirrorRoom(int); int reservoirAction(int); int reservoirNorthAction(int);
int reservoirSouthAction(int); int rivr4Room(int); int southTempleAction(int);
int stoneBarrowAction(int); int torchRoomAction(int); int treasureRoomAction(int);
int treeRoom(int); int trollRoomAction(int); int westHouseAction(int);
// clang-format on

namespace ZilRegistry {
namespace {

/// Routine names present in the data with no C++ implementation yet.  Kept so
/// the loader can report them and the D4 test can assert the list only shrinks.
std::vector<std::string_view> &unresolvedList() {
  static std::vector<std::string_view> v;
  return v;
}

void noteUnresolved(std::string_view routine) {
  auto &v = unresolvedList();
  if (std::find(v.begin(), v.end(), routine) == v.end()) {
    v.push_back(routine);
  }
}

// An object ACTION routine.  Most return the ZIL truth value as bool, but a
// routine whose ZIL body can RFATAL (e.g. CANDLES-FCN, 1actions.zil:2360)
// returns the tri-state directly, so the slot holds a callable yielding int
// and a bool-returning function converts into it.
using ObjFn = std::function<int()>;
/// ZIL object ACTIONs that take the routine's optional RARG: the villains,
/// whose routines the melee engine calls with F-BUSY?, F-DEAD and friends.
using ObjArgFn = int (*)(int);
using RoomFn = int (*)(int);
using ExitFn = ObjectId (*)();

/// ZIL object ACTION / DESCFCN routines.  Where two C++ functions carry the
/// same ZIL name in a comment, the one named after the ZIL routine wins.
const std::unordered_map<std::string_view, ObjFn> &objectActions() {
  static const std::unordered_map<std::string_view, ObjFn> m = {
      {"AXE-F", Villains::axeF},
      {"BAG-OF-COINS-F", bagOfCoinsAction},
      {"BARROW-DOOR-FCN", barrowDoorAction},
      {"BARROW-FCN", barrowAction},
      {"BASKET-F", basketAction},
      {"BAT-F", batAction},
      {"BELL-F", bellAction},
      {"BOARD-F", boardAction},
      {"BOARDED-WINDOW-FCN", boardedWindowAction},
      {"BODY-FUNCTION", bodyAction},
      {"BOLT-F", boltAction},
      {"BOTTLE-FUNCTION", bottleAction},
      {"BUBBLE-F", bubbleAction},
      {"BUTTON-F", buttonAction},
      {"CANARY-OBJECT", canaryAction},
      {"CANDLES-FCN", candlesAction},
      {"CHALICE-FCN", Villains::chaliceFcn},
      {"CHIMNEY-F", chimneyAction},
      {"CRACK-FCN", crackAction},
      {"CRETIN-FCN", GGlobals::cretinFcn},
      
      {"DAM-FUNCTION", damAction},
      {"DBOAT-FUNCTION", puncturedBoatAction},
      {"EGG-OBJECT", eggAction},
      {"FOREST-F", forestAction},
      {"FRONT-DOOR-FCN", frontDoorAction},
      {"GARLIC-F", garlicAction},
      {"GHOSTS-F", ghostsAction},
      {"GRANITE-WALL-F", graniteWallAction},
      {"GRATE-FUNCTION", grateAction},
      {"GROUND-FUNCTION", GGlobals::groundFunction},
      {"GRUE-FUNCTION", GGlobals::grueFunction},
      {"GUNK-FUNCTION", gunkAction},
      {"HOT-BELL-F", hotBellAction},
      {"IBOAT-FUNCTION", iboatFunction},
      {"KITCHEN-WINDOW-F", kitchenWindowAction},
      {"KNIFE-F", knifeAction},
      {"LANTERN", lampAction},
      {"LARGE-BAG-F", Villains::largeBagF},
      {"LEAK-FUNCTION", leakFunction},
      {"MACHINE-F", machineAction},
      {"MAILBOX-F", mailboxAction},
      {"MATCH-FUNCTION", matchesAction},
      {"MIRROR-MIRROR", mirrorAction},
      {"MOUNTAIN-RANGE-F", mountainRangeAction},
      {"MSWITCH-FUNCTION", machineSwitchAction},
      {"NOT-HERE-OBJECT-F", GGlobals::notHereObjectF},
      {"PAINTING-FCN", paintingAction},
      {"PATH-OBJECT", GGlobals::pathObject},
      {"PUTTY-FCN", puttyAction},
      {"RAINBOW-FCN", rainbowAction},
      {"RBOAT-FUNCTION", inflatedBoatAction},
      {"RIVER-FUNCTION", riverAction},
      
      {"ROPE-FUNCTION", ropeAction},
      {"RUG-FCN", rugAction},
      {"RUSTY-KNIFE-FCN", rustyKnifeAction},
      {"SAILOR-FCN", GGlobals::sailorFcn},
      {"SAND-FUNCTION", sandAction},
      {"SANDWICH-BAG-FCN", sandwichBagAction},
      {"SCEPTRE-FUNCTION", sceptreAction},
      {"SLIDE-FUNCTION", slideAction},
      {"SONGBIRD-F", songbirdAction},
      {"STAIRS-F", GGlobals::stairsF},
      {"STILETTO-FUNCTION", Villains::stilettoFunction},
      {"SWORD-FCN", swordAction},
      {"TEETH-F", teethAction},
      {"TOOL-CHEST-FCN", toolChestAction},
      {"TORCH-OBJECT", torchAction},
      {"TRAP-DOOR-FCN", trapDoorAction},
      {"TREASURE-INSIDE", Dungeon::treasureInsideAction},
      
      {"TROPHY-CASE-FCN", trophyCaseAction},
      {"TRUNK-F", trunkAction},
      {"TUBE-FUNCTION", tubeAction},
      {"WATER-F", waterAction},
      {"WHITE-HOUSE-F", whiteHouseAction},
      {"ZORKMID-FUNCTION", GGlobals::zorkmidFunction},
      // Room (PSEUDO "word" ROUTINE) handlers, 1actions.zil:3177-3231, 4167.
      {"CHAIN-PSEUDO", chainPseudo},
      {"CHASM-PSEUDO", chasmPseudo},
      {"DOME-PSEUDO", domePseudo},
      {"DOOR-PSEUDO", doorPseudo},
      {"GAS-PSEUDO", gasPseudo},
      {"GATE-PSEUDO", gatePseudo},
      {"LAKE-PSEUDO", lakePseudo},
      {"NAILS-PSEUDO", nailsPseudo},
      {"PAINT-PSEUDO", paintPseudo},
      {"STREAM-PSEUDO", streamPseudo},
  };
  return m;
}

/// ZIL room ACTION routines (1dungeon.zil room (ACTION ...) clauses).
const std::unordered_map<std::string_view, RoomFn> &roomActions() {
  static const std::unordered_map<std::string_view, RoomFn> m = {
      {"BATS-ROOM", batsRoom},
      {"BOOM-ROOM", boomRoom},
      {"CANYON-VIEW-F", Dungeon::canyonViewRoomAction},
      {"CAVE2-ROOM", cave2Room},
      {"CELLAR-FCN", cellarAction},
      {"CLEARING-FCN", clearingAction},
      
      {"DAM-ROOM-FCN", damRoomAction},
      {"DEEP-CANYON-F", deepCanyonRoomAction},
      {"DOME-ROOM-FCN", domeRoomAction},
      {"EAST-HOUSE", behindHouseAction},
      {"FALLS-ROOM", fallsRoom},
      {"FOREST-ROOM", forestRoom},
      {"KITCHEN-FCN", kitchenAction},
      {"LIVING-ROOM-FCN", livingRoomAction},
      {"LLD-ROOM", lldRoom},
      {"LOUD-ROOM-FCN", loudRoomAction},
      {"MACHINE-ROOM-FCN", machineRoomAction},
      {"MAZE-11-FCN", maze11Action},
      {"MIRROR-ROOM", mirrorRoom},
      {"RESERVOIR-FCN", reservoirAction},
      {"RESERVOIR-NORTH-FCN", reservoirNorthAction},
      {"RESERVOIR-SOUTH-FCN", reservoirSouthAction},
      {"RIVR4-ROOM", rivr4Room},
      {"SOUTH-TEMPLE-FCN", southTempleAction},
      {"STONE-BARROW-FCN", stoneBarrowAction},
      {"TORCH-ROOM-FCN", torchRoomAction},
      
      {"TREE-ROOM", treeRoom},
      {"TROLL-ROOM-F", trollRoomAction},
      {"TREASURE-ROOM-FCN", Villains::treasureRoomFcn},
      {"CYCLOPS-ROOM-FCN", Villains::cyclopsRoomFcn},
      {"WEST-HOUSE", westHouseAction},
  };
  return m;
}

/// (DIR PER routine) exits, 1dungeon.zil.
const std::unordered_map<std::string_view, ExitFn> &exitRoutines() {
  static const std::unordered_map<std::string_view, ExitFn> m = {
      {"GRATING-EXIT", Dungeon::gratingExit},
      {"MAZE-DIODES", Dungeon::mazeDiodes},
      {"TRAP-DOOR-EXIT", Dungeon::trapDoorExit},
      {"UP-CHIMNEY-FUNCTION", Dungeon::upChimneyFunction},
  };
  return m;
}

} // namespace

ObjectId idFor(std::string_view zilName) {
  static const std::unordered_map<std::string_view, ObjectId> m = [] {
    std::unordered_map<std::string_view, ObjectId> t;
    for (const auto &e : zork::zil::kNameIds) {
      t.emplace(e.name, e.id);
    }
    return t;
  }();
  auto it = m.find(zilName);
  return it == m.end() ? 0 : it->second;
}

std::optional<ObjectFlag> flagFor(std::string_view n) {
  static const std::unordered_map<std::string_view, ObjectFlag> m = {
      {"RMUNGBIT", ObjectFlag::RMUNGBIT},     {"INVISIBLE", ObjectFlag::INVISIBLE},
      {"TOUCHBIT", ObjectFlag::TOUCHBIT},     {"SURFACEBIT", ObjectFlag::SURFACEBIT},
      {"TRYTAKEBIT", ObjectFlag::TRYTAKEBIT}, {"OPENBIT", ObjectFlag::OPENBIT},
      {"SEARCHBIT", ObjectFlag::SEARCHBIT},   {"TRANSBIT", ObjectFlag::TRANSBIT},
      {"ONBIT", ObjectFlag::ONBIT},           {"RLANDBIT", ObjectFlag::RLANDBIT},
      {"FIGHTBIT", ObjectFlag::FIGHTBIT},     {"STAGGERED", ObjectFlag::STAGGERED},
      {"WEARBIT", ObjectFlag::WEARBIT},       {"NDESCBIT", ObjectFlag::NDESCBIT},
      {"TAKEBIT", ObjectFlag::TAKEBIT},       {"DOORBIT", ObjectFlag::DOORBIT},
      {"CONTBIT", ObjectFlag::CONTBIT},       {"LIGHTBIT", ObjectFlag::LIGHTBIT},
      {"ACTORBIT", ObjectFlag::ACTORBIT},     {"WEAPONBIT", ObjectFlag::WEAPONBIT},
      {"TOOLBIT", ObjectFlag::TOOLBIT},       {"BURNBIT", ObjectFlag::BURNBIT},
      {"FLAMEBIT", ObjectFlag::FLAMEBIT},     {"VEHBIT", ObjectFlag::VEHBIT},
      {"CLIMBBIT", ObjectFlag::CLIMBBIT},     {"DRINKBIT", ObjectFlag::DRINKBIT},
      {"FOODBIT", ObjectFlag::FOODBIT},       {"READBIT", ObjectFlag::READBIT},
      {"TURNBIT", ObjectFlag::TURNBIT},       {"SACREDBIT", ObjectFlag::SACREDBIT},
      {"MAZEBIT", ObjectFlag::MAZEBIT},       {"NONLANDBIT", ObjectFlag::NONLANDBIT},
  };
  auto it = m.find(n);
  return it == m.end() ? std::nullopt : std::optional<ObjectFlag>(it->second);
}

std::optional<Direction> directionFor(std::string_view n) {
  static const std::unordered_map<std::string_view, Direction> m = {
      {"NORTH", Direction::NORTH}, {"SOUTH", Direction::SOUTH},
      {"EAST", Direction::EAST},   {"WEST", Direction::WEST},
      {"NE", Direction::NE},       {"NW", Direction::NW},
      {"SE", Direction::SE},       {"SW", Direction::SW},
      {"UP", Direction::UP},       {"DOWN", Direction::DOWN},
      {"IN", Direction::IN},       {"OUT", Direction::OUT},
      {"LAND", Direction::LAND},
  };
  auto it = m.find(n);
  return it == m.end() ? std::nullopt : std::optional<Direction>(it->second);
}

std::function<bool()> flagTestFor(std::string_view n) {
  // ZIL: the conditional-exit globals of 1dungeon.zil:1225-1236 plus
  // FALSE-FLAG (line 9), which is never set and only exists to make an exit
  // print its ELSE string.
  using Member = bool Globals::*;
  static const std::unordered_map<std::string_view, Member> m = {
      {"COFFIN-CURE", &Globals::coffinCure},
      {"CYCLOPS-FLAG", &Globals::cyclopsFlag},
      {"DEFLATE", &Globals::deflate},
      {"DOME-FLAG", &Globals::domeFlag},
      {"EMPTY-HANDED", &Globals::emptyHanded},
      {"FALSE-FLAG", &Globals::falseFlag},
      {"LLD-FLAG", &Globals::lldFlag},
      {"LOW-TIDE", &Globals::lowTide},
      {"MAGIC-FLAG", &Globals::magicFlag},
      {"RAINBOW-FLAG", &Globals::rainbowFlag},
      {"TROLL-FLAG", &Globals::trollFlag},
      {"WON-FLAG", &Globals::wonFlag},
  };
  auto it = m.find(n);
  if (it == m.end()) {
    return [] { return false; };
  }
  Member member = it->second;
  return [member] { return Globals::instance().*member; };
}

/// ZIL object ACTIONs that take the routine's optional MODE argument.
const std::unordered_map<std::string_view, ObjArgFn> &objectArgActions() {
  static const std::unordered_map<std::string_view, ObjArgFn> m = {
      {"TROLL-FCN", Villains::trollFcn},
      {"ROBBER-FUNCTION", Villains::robberFunction},
      {"CYCLOPS-FCN", Villains::cyclopsFcn},
  };
  return m;
}

ZObject::ActionFunc objectActionFor(std::string_view routine) {
  if (routine.empty()) {
    return {};
  }
  const auto &args = objectArgActions();
  if (auto ait = args.find(routine); ait != args.end()) {
    ObjArgFn fn = ait->second;
    return [fn](int rarg) { return fn(rarg); };
  }
  const auto &m = objectActions();
  auto it = m.find(routine);
  if (it == m.end()) {
    noteUnresolved(routine);
    return {};
  }
  const ObjFn &fn = it->second;
  // A bool-returning routine yields 0/1, which are M-NOT-HANDLED/M-HANDLED;
  // a tri-state routine's M-FATAL (2) passes through unchanged.
  return [fn](int) { return fn(); };
}

ZRoom::RoomActionFunc roomActionFor(std::string_view routine) {
  if (routine.empty()) {
    return {};
  }
  const auto &m = roomActions();
  auto it = m.find(routine);
  if (it == m.end()) {
    noteUnresolved(routine);
    return {};
  }
  RoomFn fn = it->second;
  return [fn](int rarg) { return fn(rarg); };
}

std::function<ObjectId()> exitRoutineFor(std::string_view routine) {
  if (routine.empty()) {
    return {};
  }
  const auto &m = exitRoutines();
  auto it = m.find(routine);
  if (it == m.end()) {
    noteUnresolved(routine);
    return {};
  }
  ExitFn fn = it->second;
  return [fn] { return fn(); };
}

std::vector<std::string_view> unresolvedRoutines() {
  auto v = unresolvedList();
  std::sort(v.begin(), v.end());
  return v;
}

} // namespace ZilRegistry
