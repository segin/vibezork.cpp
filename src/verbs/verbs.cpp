#include "verbs.h"
#include "verb_tables.h"
#include "../systems/melee.h"
#include "../systems/death.h"
#include "../systems/save.h"
#include "../systems/score.h"
#include "core/globals.h"
#include "core/gmacros.h"
#include "core/gmain.h"
#include "core/io.h"
#include "parser/gparser.h"
#include "parser/parser.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"
#include <fstream>
#include <cctype>
#include <sstream>

// External state from actions.cpp
extern bool damGatesOpen;

namespace Verbs {

// ZIL: <GLOBAL FUMBLE-NUMBER 7> <GLOBAL FUMBLE-PROB 8> (gverbs.zil:1896-1898)
constexpr int FUMBLE_NUMBER = 7;
constexpr int FUMBLE_PROB = 8;

// The release and serial V-VERSION reads out of the story header
// (gverbs.zil:110-119); these are the values in zil/COMPILED/zork1.z3.
constexpr int RELEASE_NUMBER = 119;
constexpr std::string_view SERIAL_NUMBER = "880429";

// The interpreter owns the save file in the original, so SAVE and RESTORE
// take no filename (gverbs.zil:71-83).
constexpr std::string_view SAVE_FILE = "zork1.sav";

// Helper function to calculate total weight (size) of an object and all its
// contents recursively This matches the WEIGHT function from ZIL
// (Requirement 64.2)
static int calculateWeight(const ZObject *obj) {
  if (!obj)
    return 0;

  int weight = obj->getProperty(P_SIZE);

  // Add weight of all contents recursively
  for (const auto *content : obj->getContents()) {
    weight += calculateWeight(content);
  }

  return weight;
}

// Helper function to check if an object is accessible to the player
// An object is accessible if it's in the current room, player inventory,
// or in an open container in either location
static bool isObjectAccessible(const ZObject *obj) {
  if (!obj)
    return false;

  auto &g = Globals::instance();
  ZObject *location = obj->getLocation();

  // In inventory
  if (location == g.winner)
    return true;

  // In current room
  if (location == g.here)
    return true;

  // In an open container in room or inventory
  if (location && location->hasFlag(ObjectFlag::CONTBIT) &&
      location->hasFlag(ObjectFlag::OPENBIT)) {
    ZObject *containerLocation = location->getLocation();
    if (containerLocation == g.here || containerLocation == g.winner) {
      return true;
    }
  }

  return false;
}

// Helper function to find and auto-select an implied object when verb has only
// one valid target Returns the object if exactly one is applicable, nullptr
// otherwise Prints "(object name)" if an object is auto-selected
static ZObject *tryImpliedObject(VerbId verb) {
  auto &g = Globals::instance();
  std::vector<ZObject *> applicable;

  // Different verb types need different object pools
  if (verb == V_TAKE) {
    // For TAKE: objects in room that can be taken
    for (const auto *obj : g.here->getContents()) {
      if (obj->hasFlag(ObjectFlag::TAKEBIT) &&
          !obj->hasFlag(ObjectFlag::TRYTAKEBIT) &&
          !obj->hasFlag(ObjectFlag::NDESCBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    // Also check inside open containers in the room
    for (const auto *container : g.here->getContents()) {
      if (container->hasFlag(ObjectFlag::CONTBIT) &&
          container->hasFlag(ObjectFlag::OPENBIT)) {
        for (const auto *obj : container->getContents()) {
          if (obj->hasFlag(ObjectFlag::TAKEBIT) &&
              !obj->hasFlag(ObjectFlag::TRYTAKEBIT)) {
            applicable.push_back(const_cast<ZObject *>(obj));
          }
        }
      }
    }
  } else if (verb == V_DROP) {
    // For DROP: objects in inventory
    for (const auto *obj : g.winner->getContents()) {
      applicable.push_back(const_cast<ZObject *>(obj));
    }
  } else if (verb == V_READ) {
    // For READ: readable objects visible (READBIT)
    for (const auto *obj : g.here->getContents()) {
      if (obj->hasFlag(ObjectFlag::READBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    for (const auto *obj : g.winner->getContents()) {
      if (obj->hasFlag(ObjectFlag::READBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    // Check open containers
    for (const auto *container : g.here->getContents()) {
      if (container->hasFlag(ObjectFlag::CONTBIT) &&
          container->hasFlag(ObjectFlag::OPENBIT)) {
        for (const auto *obj : container->getContents()) {
          if (obj->hasFlag(ObjectFlag::READBIT)) {
            applicable.push_back(const_cast<ZObject *>(obj));
          }
        }
      }
    }
  } else if (verb == V_OPEN) {
    // For OPEN: openable objects not already open
    for (const auto *obj : g.here->getContents()) {
      if ((obj->hasFlag(ObjectFlag::CONTBIT) ||
           obj->hasFlag(ObjectFlag::DOORBIT)) &&
          !obj->hasFlag(ObjectFlag::OPENBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    for (const auto *obj : g.winner->getContents()) {
      if ((obj->hasFlag(ObjectFlag::CONTBIT) ||
           obj->hasFlag(ObjectFlag::DOORBIT)) &&
          !obj->hasFlag(ObjectFlag::OPENBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
  } else if (verb == V_CLOSE) {
    // For CLOSE: closeable objects that are open
    for (const auto *obj : g.here->getContents()) {
      if ((obj->hasFlag(ObjectFlag::CONTBIT) ||
           obj->hasFlag(ObjectFlag::DOORBIT)) &&
          obj->hasFlag(ObjectFlag::OPENBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    for (const auto *obj : g.winner->getContents()) {
      if ((obj->hasFlag(ObjectFlag::CONTBIT) ||
           obj->hasFlag(ObjectFlag::DOORBIT)) &&
          obj->hasFlag(ObjectFlag::OPENBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
  } else if (verb == V_EXAMINE) {
    // For EXAMINE: all visible objects (very permissive)
    for (const auto *obj : g.here->getContents()) {
      if (!obj->hasFlag(ObjectFlag::INVISIBLE)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    for (const auto *obj : g.winner->getContents()) {
      applicable.push_back(const_cast<ZObject *>(obj));
    }
  } else {
    // Generic: visible objects in room and inventory
    for (const auto *obj : g.here->getContents()) {
      if (!obj->hasFlag(ObjectFlag::INVISIBLE) &&
          !obj->hasFlag(ObjectFlag::NDESCBIT)) {
        applicable.push_back(const_cast<ZObject *>(obj));
      }
    }
    for (const auto *obj : g.winner->getContents()) {
      applicable.push_back(const_cast<ZObject *>(obj));
    }
  }

  // If exactly one object, auto-select it
  if (applicable.size() == 1) {
    ZObject *obj = applicable[0];
    print("(");
    print(obj->getDesc());
    printLine(")");
    return obj;
  }

  return nullptr;
}

// ZIL: <GLOBAL INDENTS <TABLE (PURE) "" "  " ... >>
// Source: zil/gverbs.zil:2015-2022
static constexpr std::string_view INDENTS[] = {
    "", "  ", "    ", "      ", "        ", "          ",
};

// ZIL: <GLOBAL DESC-OBJECT <>> (gverbs.zil:1691). Set by DESCRIBE-OBJECT and
// never read; kept because the source keeps it.
static ZObject *descObject = nullptr;

// ZIL: <ROUTINE FIRSTER (OBJ LEVEL) ...>
// Source: zil/gverbs.zil:1818-1835
bool firster(const ZObject *obj, int level) {
  auto &g = Globals::instance();
  if (!obj) return RFALSE;
  if (obj->getId() == ObjectIds::TROPHY_CASE) {
    tell("Your collection of treasures consists of:", CR);
    return RTRUE;
  }
  if (obj == g.winner) {
    tell("You are carrying:", CR);
    return RTRUE;
  }
  if (!dynamic_cast<const ZRoom *>(obj)) {
    if (level > 0) {
      tell(INDENTS[level < 6 ? level : 5]);
    }
    if (obj->hasFlag(ObjectFlag::SURFACEBIT)) {
      tell("Sitting on the ", obj, " is: ", CR);
    } else if (obj->hasFlag(ObjectFlag::ACTORBIT)) {
      tell("The ", obj, " is holding: ", CR);
    } else {
      tell("The ", obj, " contains:", CR);
    }
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE DESCRIBE-OBJECT (OBJ V? LEVEL "AUX" (STR <>) AV) ...>
// Source: zil/gverbs.zil:1693-1728
void describeObject(ZObject *obj, bool v, int level) {
  auto &g = Globals::instance();
  if (!obj) return;
  descObject = obj;
  if (level == 0 && obj->performDescFcn(M_OBJDESC)) {
    return;
  }
  std::string_view str;
  if (level == 0 &&
      ((!obj->hasFlag(ObjectFlag::TOUCHBIT) && !obj->getFirstDesc().empty() &&
        (str = obj->getFirstDesc(), true)) ||
       (!obj->getLongDesc().empty() && (str = obj->getLongDesc(), true)))) {
    tell(str);
  } else if (level == 0) {
    tell("There is a ", obj, " here");
    if (obj->hasFlag(ObjectFlag::ONBIT)) {
      tell(" (providing light)");
    }
    tell(".");
  } else {
    tell(INDENTS[level < 6 ? level : 5]);
    tell("A ", obj);
    if (obj->hasFlag(ObjectFlag::ONBIT)) {
      tell(" (providing light)");
    } else if (obj->hasFlag(ObjectFlag::WEARBIT) &&
               obj->getLocation() == g.winner) {
      tell(" (being worn)");
    }
  }
  if (level == 0 && g.winner) {
    ZObject *av = g.winner->getLocation();
    if (av && av->hasFlag(ObjectFlag::VEHBIT)) {
      tell(" (outside the ", av, ")");
    }
  }
  crlf();
  if (seeInside(obj) && !obj->getContents().empty()) {
    printCont(obj, v, level);
  }
}

// ZIL: <ROUTINE PRINT-CONT (OBJ "OPTIONAL" (V? <>) (LEVEL 0) ...) ...>
// Source: zil/gverbs.zil:1750-1816. Two passes: first the untouched objects
// with an FDESC, then everything else through DESCRIBE-OBJECT. The AUX
// variable the source calls SHIT tracks whether an FDESC was printed.
bool printCont(const ZObject *obj, bool v, int level) {
  auto &g = Globals::instance();
  if (!obj) return RTRUE;
  auto contents = obj->getContents();
  if (contents.empty()) return RTRUE;

  ZObject *av = g.winner ? g.winner->getLocation() : nullptr;
  if (!(av && av->hasFlag(ObjectFlag::VEHBIT))) {
    av = nullptr;
  }
  bool first = true;
  bool shit = true;
  bool pv = false;
  bool inv = (g.winner == obj) || (g.winner == obj->getLocation());

  if (!inv) {
    for (ZObject *y : contents) {
      if (y == av) {
        pv = true;
        continue;
      }
      if (y == g.winner) {
        continue;
      }
      if (!y->hasFlag(ObjectFlag::INVISIBLE) &&
          !y->hasFlag(ObjectFlag::TOUCHBIT) && !y->getFirstDesc().empty()) {
        if (!y->hasFlag(ObjectFlag::NDESCBIT)) {
          tell(y->getFirstDesc(), CR);
          shit = false;
        }
        if (seeInside(y) && !(y->getLocation() && y->getLocation()->hasDescFcn()) &&
            !y->getContents().empty()) {
          if (printCont(y, v, 0)) {
            first = false;
          }
        }
      }
    }
  }

  for (ZObject *y : obj->getContents()) {
    if (y == av || y == g.player) {
      continue;
    }
    if (!y->hasFlag(ObjectFlag::INVISIBLE) &&
        (inv || y->hasFlag(ObjectFlag::TOUCHBIT) || y->getFirstDesc().empty())) {
      if (!y->hasFlag(ObjectFlag::NDESCBIT)) {
        if (first) {
          if (firster(obj, level) && level < 0) {
            level = 0;
          }
          level = level + 1;
          first = false;
        }
        if (level < 0) {
          level = 0;
        }
        describeObject(y, v, level);
      } else if (!y->getContents().empty() && seeInside(y)) {
        level = level + 1;
        printCont(y, v, level);
        level = level - 1;
      }
    }
  }
  if (pv && av && !av->getContents().empty()) {
    printCont(av, v, level + 1);
  }
  return !(first && shit);
}

// ZIL: <ROUTINE DESCRIBE-OBJECTS ("OPTIONAL" (V? <>)) ...>
// Source: zil/gverbs.zil:1681-1687
void describeObjects(bool v) {
  auto &g = Globals::instance();
  if (g.lit) {
    if (g.here && !g.here->getContents().empty()) {
      printCont(g.here, v || g.verboseMode, -1);
    }
  } else {
    tell("Only bats can see in the dark. And you're not one.", CR);
  }
}

// ZIL: <ROUTINE DESCRIBE-ROOM ("OPTIONAL" (LOOK? <>) "AUX" V? STR AV) ...>
// Source: zil/gverbs.zil:1635-1679
bool describeRoom(bool look) {
  auto &g = Globals::instance();
  bool v = look || g.verboseMode;
  if (!g.lit) {
    tell("It is pitch black.");
    if (!g.sprayed) {
      tell(" You are likely to be eaten by a grue.");
    }
    crlf();
    return RFALSE;
  }
  if (!g.here) return RFALSE;
  if (!g.here->hasFlag(ObjectFlag::TOUCHBIT)) {
    g.here->setFlag(ObjectFlag::TOUCHBIT);
    v = true;
  }
  // Maze rooms never stay "seen", so they are always described in full.
  if (g.here->hasFlag(ObjectFlag::MAZEBIT)) {
    g.here->clearFlag(ObjectFlag::TOUCHBIT);
  }
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  ZObject *av = g.winner ? g.winner->getLocation() : nullptr;
  if (room) {
    tell(g.here->getDesc());
    if (av && av->hasFlag(ObjectFlag::VEHBIT)) {
      tell(", in the ", av);
    }
    crlf();
  }
  if (look || !g.superbriefMode) {
    if (v && room && room->performRoomAction(M_LOOK)) {
      return RTRUE;
    }
    if (v && !g.here->getLongDesc().empty()) {
      tell(g.here->getLongDesc(), CR);
    } else if (room) {
      room->performRoomAction(M_FLASH);
    }
    if (av && av != g.here && av->hasFlag(ObjectFlag::VEHBIT)) {
      av->performAction();
    }
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-LOOK () <COND (<DESCRIBE-ROOM T> <DESCRIBE-OBJECTS T>)>>
// Source: zil/gverbs.zil:858-860
bool vLook() {
  if (describeRoom(true)) {
    describeObjects(true);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-FIRST-LOOK () ...>
// Source: zil/gverbs.zil:1630-1633
bool vFirstLook() {
  auto &g = Globals::instance();
  if (describeRoom()) {
    if (!g.superbriefMode) {
      describeObjects();
    }
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-INVENTORY () ...>
// Source: zil/gverbs.zil:29-31
bool vInventory() {
  auto &g = Globals::instance();
  if (g.winner && !g.winner->getContents().empty()) {
    printCont(g.winner);
  } else {
    tell("You are empty-handed.", CR);
  }
  return RTRUE;
}



// ZIL: <ROUTINE V-QUIT ("AUX" SCOR) ...>
// Source: zil/gverbs.zil:55-61
bool vQuit() {
  vScore();
  tell("Do you wish to leave the game? (Y is affirmative): ");
  if (yes()) {
    std::exit(0);
  }
  tell("Ok.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-TAKE () ...>
// Source: zil/gverbs.zil:1382-1388. Only an exact T from ITAKE prints;
// RFATAL (the load message) and RFALSE fall through silently.
bool vTake() {
  auto &g = Globals::instance();
  if (iTake() == RTRUE) {
    if (g.prso->hasFlag(ObjectFlag::WEARBIT)) {
      tell("You are now wearing the ", g.prso, ".", CR);
    } else {
      tell("Taken.", CR);
    }
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE V-DROP () ...>
// Source: zil/gverbs.zil:479-481
bool vDrop() {
  if (iDrop()) {
    tell("Dropped.", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE V-EXAMINE () ...>
// Source: zil/gverbs.zil:623-630
bool vExamine() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasText()) {
    tell(g.prso->getText(), CR);
    return RTRUE;
  }
  if (g.prso && (g.prso->hasFlag(ObjectFlag::CONTBIT) ||
                 g.prso->hasFlag(ObjectFlag::DOORBIT))) {
    return vLookInside();
  }
  tell("There's nothing special about the ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-OPEN ("AUX" F STR) ...>
// Source: zil/gverbs.zil:966-994. A container with <CAPACITY 0> is not
// openable at all, and a container holding exactly one untouched object with
// an FDESC prints that description instead of the usual reveal.
bool vOpen() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  if (g.prso->hasFlag(ObjectFlag::CONTBIT) &&
      g.prso->getProperty(P_CAPACITY) != 0) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      tell("It is already open.", CR);
      return RTRUE;
    }
    g.prso->setFlag(ObjectFlag::OPENBIT);
    g.prso->setFlag(ObjectFlag::TOUCHBIT);
    auto contents = g.prso->getContents();
    const ZObject *f = contents.empty() ? nullptr : contents.front();
    if (!f || g.prso->hasFlag(ObjectFlag::TRANSBIT)) {
      tell("Opened.", CR);
    } else if (contents.size() == 1 && !f->hasFlag(ObjectFlag::TOUCHBIT) &&
               !f->getFirstDesc().empty()) {
      tell("The ", g.prso, " opens.", CR);
      tell(f->getFirstDesc(), CR);
    } else {
      tell("Opening the ", g.prso, " reveals ");
      printContents(g.prso);
      tell(".", CR);
    }
    return RTRUE;
  }
  if (g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      tell("It is already open.", CR);
    } else {
      tell("The ", g.prso, " opens.", CR);
      g.prso->setFlag(ObjectFlag::OPENBIT);
    }
    return RTRUE;
  }
  tell("You must tell me how to do that to a ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-CLOSE () ...>
// Source: zil/gverbs.zil:336-357
bool vClose() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  if (!g.prso->hasFlag(ObjectFlag::CONTBIT) &&
      !g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    tell("You must tell me how to do that to a ", g.prso, ".", CR);
    return RTRUE;
  }
  if (!g.prso->hasFlag(ObjectFlag::SURFACEBIT) &&
      g.prso->getProperty(P_CAPACITY) != 0) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      g.prso->clearFlag(ObjectFlag::OPENBIT);
      tell("Closed.", CR);
      if (g.lit) {
        g.lit = GParser::isLit(g.here);
        if (!g.lit) {
          tell("It is now pitch black.", CR);
        }
      }
      return RTRUE;
    }
    tell("It is already closed.", CR);
    return RTRUE;
  }
  if (g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      g.prso->clearFlag(ObjectFlag::OPENBIT);
      tell("The ", g.prso, " is now closed.", CR);
    } else {
      tell("It is already closed.", CR);
    }
    return RTRUE;
  }
  tell("You cannot close that.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LOCK () <TELL "It doesn't seem to work." CR>>
// Source: zil/gverbs.zil:855-856
bool vLock() {
  tell("It doesn't seem to work.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-UNLOCK () <TELL "It doesn't seem to work." CR>>
// Source: zil/gverbs.zil:1487 (V-UNLOCK shares V-LOCK's message)
bool vUnlock() {
  tell("It doesn't seem to work.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-WALK ("AUX" PT PTS STR OBJ RM)
//        <COND (<NOT ,P-WALK-DIR> <PERFORM ,V?WALK-TO ,PRSO> <RTRUE>)
//              ... direction handling ...
// Source: gverbs.zil:1521-1580
int vWalk() {
  auto &g = Globals::instance();
  if (!g.pWalkDir) {
    perform(V_WALK_TO, g.prso);
    return M_HANDLED;
  }
  return vWalkDir(*g.pWalkDir);
}

// ZIL: <ROUTINE NO-GO-TELL (AV WLOC) ...>
// Source: zil/gverbs.zil:2038-2043
void noGoTell(ZObject *av, ZObject *wloc) {
  if (av) {
    tell("You can't go there in a ", wloc, ".");
  } else {
    tell("You can't go there without a vehicle.");
  }
  crlf();
}

// ZIL: <ROUTINE GOTO (RM "OPTIONAL" (V? T) ...) ...>
// Source: zil/gverbs.zil:2045-2137
bool goTo(ZObject *rm, bool v) {
  auto &g = Globals::instance();
  if (!rm) return RFALSE;
  bool lb = rm->hasFlag(ObjectFlag::RLANDBIT);
  ZObject *wloc = g.winner ? g.winner->getLocation() : nullptr;
  std::optional<ObjectFlag> av;
  bool olit = g.lit;
  ZObject *ohere = g.here;
  if (wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    av = wloc->getVehicleType();
  }
  if (!lb && !av) {
    noGoTell(nullptr, wloc);
    return RFALSE;
  }
  if (!lb && !rm->hasFlag(*av)) {
    noGoTell(wloc, wloc);
    return RFALSE;
  }
  if (g.here && g.here->hasFlag(ObjectFlag::RLANDBIT) && lb && av &&
      *av != ObjectFlag::RLANDBIT && !rm->hasFlag(*av)) {
    noGoTell(wloc, wloc);
    return RFALSE;
  }
  if (rm->hasFlag(ObjectFlag::RMUNGBIT)) {
    tell(rm->getLongDesc(), CR);
    return RFALSE;
  }
  if (lb && g.here && !g.here->hasFlag(ObjectFlag::RLANDBIT) &&
      !DeathSystem::isDead() && wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    tell("The ", wloc, " comes to a rest on the shore.", CR, CR);
  }
  if (av) {
    wloc->moveTo(rm);
  } else if (g.winner) {
    g.winner->moveTo(rm);
  }
  g.here = rm;
  g.lit = GParser::isLit(g.here);
  if (!olit && !g.lit && GMacros::prob(80)) {
    if (g.sprayed) {
      tell("There are sinister gurgling noises in the darkness all around you!",
           CR);
    } else {
      tell("Oh, no! A lurking grue slithered into the ");
      ZObject *wl = g.winner ? g.winner->getLocation() : nullptr;
      if (wl && wl->hasFlag(ObjectFlag::VEHBIT)) {
        tell(wl);
      } else {
        tell("room");
      }
      DeathSystem::jigsUp(" and devoured you!");
      return RTRUE;
    }
  }
  if (!g.lit && g.winner == g.player) {
    tell("You have moved into a dark place.", CR);
    g.pCont = 0;
  }
  if (ZRoom *room = dynamic_cast<ZRoom *>(g.here)) {
    room->performRoomAction(M_ENTER);
  }
  scoreObj(rm);
  if (g.here != rm) {
    return RTRUE;
  }
  if (g.winner != g.player && g.player && g.player->getLocation() == ohere) {
    tell("The ", g.winner, " leaves the room.", CR);
    return RTRUE;
  }
  // ZIL: no double description when re-entering Hades from itself.
  if (g.here == ohere && g.here->getId() == RoomIds::ENTRANCE_TO_HADES) {
    return RTRUE;
  }
  if (v && g.winner == g.player) {
    vFirstLook();
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-WALK ("AUX" PT PTS STR OBJ RM) ...>
// Source: zil/gverbs.zil:1521-1580
int vWalkDir(Direction dir) {
  auto &g = Globals::instance();
  ZRoom *currentRoom = dynamic_cast<ZRoom *>(g.here);
  RoomExit *exit = currentRoom ? currentRoom->getExit(dir) : nullptr;

  if (exit) {
    switch (exit->type) {
    case ExitType::DOOR: {
      // ZIL DEXIT (gverbs.zil:1550-1558)
      ZObject *obj = g.getObject(exit->doorObject);
      if (obj && obj->hasFlag(ObjectFlag::OPENBIT)) {
        return goTo(g.getObject(exit->targetRoom)) ? M_HANDLED : GMacros::rfatal();
      }
      if (!exit->message.empty()) {
        tell(exit->message, CR);
        return GMacros::rfatal();
      }
      tell("The ", obj, " is closed.", CR);
      thisIsIt(obj);
      return GMacros::rfatal();
    }
    case ExitType::CONDITIONAL: {
      // ZIL CEXIT (gverbs.zil:1541-1549)
      if (!exit->condition || exit->condition()) {
        return goTo(g.getObject(exit->targetRoom)) ? M_HANDLED : GMacros::rfatal();
      }
      if (!exit->message.empty()) {
        tell(exit->message, CR);
        return GMacros::rfatal();
      }
      tell("You can't go that way.", CR);
      return GMacros::rfatal();
    }
    case ExitType::PROCEDURAL: {
      // ZIL FEXIT (gverbs.zil:1531-1540)
      ObjectId target = exit->procedural ? exit->procedural() : 0;
      if (target != 0) {
        return goTo(g.getObject(target)) ? M_HANDLED : GMacros::rfatal();
      }
      return GMacros::rfatal();
    }
    case ExitType::SPECIAL:
    case ExitType::ONE_WAY:
    case ExitType::NORMAL:
    default:
      // ZIL NEXIT: an exit that is only a string (gverbs.zil:1528-1530)
      if (!exit->message.empty()) {
        tell(exit->message, CR);
        return GMacros::rfatal();
      }
      if (!exit->specialMessage.empty()) {
        tell(exit->specialMessage, CR);
        return GMacros::rfatal();
      }
      // ZIL UEXIT
      return goTo(g.getObject(exit->targetRoom)) ? M_HANDLED : GMacros::rfatal();
    }
  }

  // No exit that way. In the dark this is usually fatal (gverbs.zil:1559-1577).
  if (!g.lit && GMacros::prob(80) && g.winner == g.player && g.here &&
      !g.here->hasFlag(ObjectFlag::NONLANDBIT)) {
    if (g.sprayed) {
      tell("There are odd noises in the darkness, and there is no exit in that "
           "direction.",
           CR);
      return GMacros::rfatal();
    }
    DeathSystem::jigsUp(
        "Oh, no! You have walked into the slavering fangs of a lurking grue!");
    return GMacros::rfatal();
  }
  tell("You can't go that way.", CR);
  return GMacros::rfatal();
}

// ZIL: <ROUTINE V-WALK-AROUND () <TELL "Use compass directions for movement." CR>>
// Source: zil/gverbs.zil:1582-1583
bool vWalkAround() {
  tell("Use compass directions for movement.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-WALK-TO () ...>
// Source: zil/gverbs.zil:1585-1591
bool vWalkTo() {
  auto &g = Globals::instance();
  if (g.prso && (g.prso->getLocation() == g.here ||
                 globalIn(g.prso->getId(), g.here))) {
    tell("It's here!", CR);
  } else {
    tell("You should supply a direction!", CR);
  }
  return RTRUE;
}


bool trySpecialMovement(int verbId, Direction dir) {
  auto &g = Globals::instance();
  ZRoom *currentRoom = dynamic_cast<ZRoom *>(g.here);

  if (!currentRoom) {
    return false;
  }

  RoomExit *exit = currentRoom->getExit(dir);
  if (!exit || exit->type != ExitType::SPECIAL) {
    return false;
  }

  // Check if this is the required verb for this exit
  if (exit->requiredVerb != verbId) {
    if (!exit->specialMessage.empty()) {
      printLine(exit->specialMessage);
    } else {
      printLine("You can't do that here.");
    }
    return true;
  }

  // Check if door is involved
  if (exit->doorObject != 0) {
    ZObject *door = g.getObject(exit->doorObject);
    if (door) {
      if (!door->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("The door is closed.");
        return true;
      }
    }
  }

  // Check condition if present
  if (exit->condition && !exit->condition()) {
    if (!exit->message.empty()) {
      printLine(exit->message);
    } else {
      printLine("You can't do that.");
    }
    return true;
  }

  // Move to new room
  ZObject *newRoom = g.getObject(exit->targetRoom);
  if (newRoom) {
    g.here = newRoom;
    g.winner->moveTo(newRoom);
  }

  return true;
}

// ZIL: <ROUTINE V-ENTER () <DO-WALK ,P?IN>>
// Source: zil/gverbs.zil:620-621
bool vEnter() {
  doWalk(Direction::IN);
  return RTRUE;
}

// ZIL: <ROUTINE V-EXIT () ...>
// Source: zil/gverbs.zil:632-641
bool vExit() {
  auto &g = Globals::instance();
  ZObject *wloc = g.winner ? g.winner->getLocation() : nullptr;
  if (!g.prso && wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_DISEMBARK, wloc);
    return RTRUE;
  }
  if (g.prso && wloc == g.prso) {
    perform(V_DISEMBARK, g.prso);
    return RTRUE;
  }
  doWalk(Direction::OUT);
  return RTRUE;
}

// ZIL: <ROUTINE V-CLIMB-UP ("OPTIONAL" (DIR ,P?UP) (OBJ <>) "AUX" X TX) ...>
// Source: zil/gverbs.zil:300-334
bool climbUp(Direction dir, bool haveObj) {
  auto &g = Globals::instance();
  ZObject *obj = nullptr;
  if (haveObj && g.prso && g.prso->getId() != ObjectIds::ROOMS) {
    obj = g.prso;
  }
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  RoomExit *tx = room ? room->getExit(dir) : nullptr;
  if (tx) {
    if (obj) {
      bool blocked = (tx->targetRoom == 0 && !tx->message.empty());
      if (!blocked && !globalIn(obj->getId(), g.here) &&
          obj->getLocation() != g.here) {
        blocked = true;
      }
      if (blocked) {
        tell("The ", obj, " do");
        if (obj->getId() != ObjectIds::STAIRS) {
          tell("es");
        }
        tell("n't lead ");
        tell(dir == Direction::UP ? "up" : "down");
        tell("ward.", CR);
        return RTRUE;
      }
    }
    doWalk(dir);
    return RTRUE;
  }
  if (obj && obj->hasSynonym("wall")) {
    tell("Climbing the walls is to no avail.", CR);
    return RTRUE;
  }
  if (g.here && g.here->getId() != RoomIds::FOREST_PATH &&
      (!obj || obj->getId() == ObjectIds::TREE) &&
      globalIn(ObjectIds::TREE, g.here)) {
    tell("There are no climbable trees here.", CR);
    return RTRUE;
  }
  if (!obj || obj->getId() == ObjectIds::ROOMS) {
    tell("You can't go that way.", CR);
    return RTRUE;
  }
  tell("You can't do that!", CR);
  return RTRUE;
}

bool vClimbUp() { return climbUp(Direction::UP, true); }

// ZIL: <ROUTINE V-CLIMB-DOWN () <V-CLIMB-UP ,P?DOWN ,PRSO>>
// Source: zil/gverbs.zil:279
bool vClimbDown() { return climbUp(Direction::DOWN, true); }

// ZIL: <ROUTINE V-BOARD ("AUX" AV) ...>
// Source: zil/gverbs.zil:224-228
bool vBoard() {
  auto &g = Globals::instance();
  tell("You are now in the ", g.prso, ".", CR);
  if (g.winner && g.prso) {
    g.winner->moveTo(g.prso);
    g.prso->performAction();
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-DISEMBARK () ...>
// Source: zil/gverbs.zil:418-432
bool vDisembark() {
  auto &g = Globals::instance();
  ZObject *wloc = g.winner ? g.winner->getLocation() : nullptr;
  if (!g.prso && wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_DISEMBARK, wloc);
    return RTRUE;
  }
  if (wloc != g.prso) {
    tell("You're not in that!", CR);
    return GMacros::rfatal();
  }
  if (g.here && g.here->hasFlag(ObjectFlag::RLANDBIT)) {
    tell("You are on your own feet again.", CR);
    g.winner->moveTo(g.here);
    return RTRUE;
  }
  tell("You realize that getting out here would be fatal.", CR);
  return GMacros::rfatal();
}

// ZIL: <ROUTINE V-READ () ...>
// Source: zil/gverbs.zil:1143-1147
bool vRead() {
  auto &g = Globals::instance();
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::READBIT)) {
    tell("How does one read a ", g.prso, "?", CR);
  } else {
    tell(g.prso->getText(), CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-LOOK-INSIDE () ...>
// Source: zil/gverbs.zil:866-890
bool vLookInside() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  if (g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      tell("The ", g.prso, " is open, but I can't tell what's beyond it.");
    } else {
      tell("The ", g.prso, " is closed.");
    }
    crlf();
    return RTRUE;
  }
  if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
      tell("There is nothing special to be seen.", CR);
    } else if (seeInside(g.prso)) {
      if (!g.prso->getContents().empty() && printCont(g.prso)) {
        return RTRUE;
      }
      tell("The ", g.prso, " is empty.", CR);
    } else {
      tell("The ", g.prso, " is closed.", CR);
    }
    return RTRUE;
  }
  tell("You can't look inside a ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SEARCH () <TELL "You find nothing unusual." CR>>
// Source: zil/gverbs.zil:1197-1198
bool vSearch() {
  tell("You find nothing unusual.", CR);
  return RTRUE;
}

bool vSearchOld() {
  // Keeping old implementation for reference - can be removed
  auto &g = Globals::instance();

  if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {

    const auto &contents = g.prso->getContents();
    if (contents.empty()) {
      printLine("You find nothing of interest.");
    } else {
      printLine("You find:");
      for (const auto *obj : contents) {
        print("  ");
        printLine(obj->getDesc());
      }
    }
  } else {
    // For non-containers, just give a generic message
    printLine("You find nothing unusual.");
  }

  return RTRUE;
}

// ZIL: <ROUTINE V-PUT () ...>
// Source: zil/gverbs.zil:1085-1113
// Note the <NOT <ITAKE>> test: ITAKE returns RFATAL (2) for an overload,
// which is truthy, so the object is still moved after "Your load is too
// heavy." This is the original's behaviour and is reproduced deliberately.
bool vPut() {
  auto &g = Globals::instance();
  if (!(g.prsi && (g.prsi->hasFlag(ObjectFlag::OPENBIT) ||
                   GMacros::isOpenable(g.prsi) ||
                   g.prsi->hasFlag(ObjectFlag::VEHBIT)))) {
    tell("You can't do that.", CR);
    return RTRUE;
  }
  if (!g.prsi->hasFlag(ObjectFlag::OPENBIT)) {
    tell("The ", g.prsi, " isn't open.", CR);
    thisIsIt(g.prsi);
    return RTRUE;
  }
  if (g.prsi == g.prso) {
    tell("How can you do that?", CR);
    return RTRUE;
  }
  if (g.prso->getLocation() == g.prsi) {
    tell("The ", g.prso, " is already in the ", g.prsi, ".", CR);
    return RTRUE;
  }
  if (weight(g.prsi) + weight(g.prso) - g.prsi->getProperty(P_SIZE) >
      g.prsi->getProperty(P_CAPACITY)) {
    tell("There's no room.", CR);
    return RTRUE;
  }
  if (!isHeld(g.prso) && g.prso->hasFlag(ObjectFlag::TRYTAKEBIT)) {
    tell("You don't have the ", g.prso, ".", CR);
    return RTRUE;
  }
  if (!isHeld(g.prso) && !iTake()) {
    return RTRUE;
  }
  g.prso->moveTo(g.prsi);
  g.prso->setFlag(ObjectFlag::TOUCHBIT);
  scoreObj(g.prso);
  tell("Done.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUT-ON () ...>
// Source: zil/gverbs.zil:1118-1126
bool vPutOn() {
  auto &g = Globals::instance();
  if (g.prsi == g.getObject(ObjectIds::GROUND)) {
    perform(V_DROP, g.prso);
    return RTRUE;
  }
  if (g.prsi && g.prsi->hasFlag(ObjectFlag::SURFACEBIT)) {
    return vPut();
  }
  tell("There's no good surface on the ", g.prsi, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUT-UNDER () <TELL "You can't do that." CR>>
// Source: zil/gverbs.zil:1128-1129
bool vPutUnder() {
  tell("You can't do that.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUT-BEHIND () ...>
// Source: zil/gverbs.zil:1115-1116
bool vPutBehind() {
  tell("That hiding place is too obvious.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-GIVE () ...>
// Source: zil/gverbs.zil:714-718
bool vGive() {
  auto &g = Globals::instance();
  if (!g.prsi || !g.prsi->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("You can't give a ", g.prso, " to a ", g.prsi, "!", CR);
  } else {
    tell("The ", g.prsi, " refuses it politely.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-SGIVE () <TELL "Foo!" CR>>
// Source: zil/gverbs.zil:1210-1211. Unreachable in play: PRE-SGIVE always
// re-performs the command as GIVE.
bool vSgive() {
  tell("Foo!", CR);
  return RTRUE;
}

// Manipulation Verbs (Requirement 26)

// ZIL: <ROUTINE V-TURN () <TELL "This has no effect." CR>>
// Source: zil/gverbs.zil:1505-1506
bool vTurn() {
  tell("This has no effect.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUSH () <HACK-HACK "Pushing the ">>
// Source: zil/gverbs.zil:1070
bool vPush() {
  hackHack("Pushing the ");
  return RTRUE;
}

bool vPull() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to pull?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // V-PULL redirects to V-MOVE in ZIL
  return vMove();
}

// ZIL: <ROUTINE V-MOVE () ...>
// Source: zil/gverbs.zil:914-918
bool vMove() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
    tell("Moving the ", g.prso, " reveals nothing.", CR);
  } else {
    tell("You can't move the ", g.prso, ".", CR);
  }
  return RTRUE;
}

// Interaction Verbs (Requirement 27)

// ZIL: <ROUTINE V-TIE () ...>
// Source: zil/gverbs.zil:1465-1469
bool vTie() {
  auto &g = Globals::instance();
  if (g.prsi == g.winner) {
    tell("You can't tie anything to yourself.", CR);
  } else {
    tell("You can't tie the ", g.prso, " to that.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-UNTIE () ...>
// Source: zil/gverbs.zil:1511-1512
bool vUntie() {
  tell("This cannot be tied, so it cannot be untied!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LISTEN () <TELL "The " D ,PRSO " makes no sound." CR>>
// Source: zil/gverbs.zil:852-853
bool vListen() {
  auto &g = Globals::instance();
  tell("The ", g.prso, " makes no sound.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SMELL () <TELL "It smells like a " D ,PRSO "." CR>>
// Source: zil/gverbs.zil:1278-1279
bool vSmell() {
  auto &g = Globals::instance();
  tell("It smells like a ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-YELL () <TELL "Aaaarrrrgggghhhh!" CR>>
// Source: zil/gverbs.zil:1616
bool vYell() {
  tell("Aaaarrrrgggghhhh!", CR);
  return RTRUE;
}

bool vTouch() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("Touch what?");
    return RTRUE;
  }

  // Call object action handler
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // ZIL default
  if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    print("The ");
    print(g.prso->getDesc());
    printLine(" dislikes being touched.");
    return RTRUE;
  }

  print("You feel nothing unexpected as you touch the ");
  print(g.prso->getDesc());
  printLine(".");
  return RTRUE;
}


// Consumption Verbs (Requirement 28)

// ZIL: <ROUTINE V-EAT ("AUX" (EAT? <>) (DRINK? <>) (NOBJ <>)) ...>
// Source: zil/gverbs.zil:483-517
bool vEat() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  bool eat = g.prso->hasFlag(ObjectFlag::FOODBIT);
  bool drink = false;
  if (eat) {
    ZObject *loc = g.prso->getLocation();
    if (loc != g.winner && !(loc && loc->getLocation() == g.winner)) {
      tell("You're not holding that.");
    } else if (g.prsa == V_DRINK) {
      tell("How can you drink that?");
    } else {
      tell("Thank you very much. It really hit the spot.");
      removeCarefully(g.prso);
    }
    crlf();
    return RTRUE;
  }
  if (g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
    drink = true;
    ZObject *nobj = g.prso->getLocation();
    ZObject *globals = g.getObject(ObjectIds::GLOBAL_OBJECTS);
    if ((globals && nobj == globals) ||
        globalIn(ObjectIds::GLOBAL_WATER, g.here) ||
        g.prso->getId() == ObjectIds::PSEUDO_OBJECT) {
      hitSpot();
    } else if (!nobj || !GParser::isAccessible(nobj)) {
      tell("There isn't any water here.", CR);
    } else if (nobj->getLocation() != g.winner) {
      tell("You have to be holding the ", nobj, " first.", CR);
    } else if (!nobj->hasFlag(ObjectFlag::OPENBIT)) {
      tell("You'll have to open the ", nobj, " first.", CR);
    } else {
      hitSpot();
    }
    return RTRUE;
  }
  if (!eat && !drink) {
    tell("I don't think that the ", g.prso, " would agree with you.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-DRINK () <V-EAT>>
// Source: zil/gverbs.zil:468-469
bool vDrink() {
  return vEat();
}

// Light Source Verbs (Requirement 30)

// ZIL: <ROUTINE V-LAMP-ON () ...>
// Source: zil/gverbs.zil:786-801
bool vLampOn() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
    if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
      tell("It is already on.", CR);
    } else {
      g.prso->setFlag(ObjectFlag::ONBIT);
      tell("The ", g.prso, " is now on.", CR);
      if (!g.lit) {
        g.lit = GParser::isLit(g.here);
        crlf();
        vLook();
      }
    }
  } else if (g.prso && g.prso->hasFlag(ObjectFlag::BURNBIT)) {
    tell("If you wish to burn the ", g.prso, ", you should say so.", CR);
  } else {
    tell("You can't turn that on.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-LAMP-OFF () ...>
// Source: zil/gverbs.zil:771-784
bool vLampOff() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
    if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
      tell("It is already off.", CR);
    } else {
      g.prso->clearFlag(ObjectFlag::ONBIT);
      if (g.lit) {
        g.lit = GParser::isLit(g.here);
      }
      tell("The ", g.prso, " is now off.", CR);
      if (!g.lit) {
        tell("It is now pitch black.", CR);
      }
    }
  } else {
    tell("You can't turn that off.", CR);
  }
  return RTRUE;
}

// Special Action Verbs (Requirement 31)

// ZIL: <ROUTINE V-INFLATE () <TELL "How can you inflate that?" CR>>
// Source: zil/gverbs.zil:757-758
bool vInflate() {
  tell("How can you inflate that?", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-DEFLATE () <TELL "Come on, now!" CR>>
// Source: zil/gverbs.zil:402-403
bool vDeflate() {
  tell("Come on, now!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PRAY () ...>
// Source: zil/gverbs.zil:1046-1054
bool vPray() {
  auto &g = Globals::instance();
  if (g.here && g.here->getId() == RoomIds::SOUTH_TEMPLE) {
    goTo(g.getObject(RoomIds::FOREST_1));
    return RTRUE;
  }
  tell("If you pray enough, your prayers may be answered.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-EXORCISE () <TELL "What a bizarre concept!" CR>>
// Source: zil/gverbs.zil:643-644
bool vExorcise() {
  tell("What a bizarre concept!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-WAVE () <HACK-HACK "Waving the ">>
// Source: zil/gverbs.zil:1594-1595
bool vWave() {
  hackHack("Waving the ");
  return RTRUE;
}

// ZIL: <ROUTINE V-RUB () <HACK-HACK "Fiddling with the ">>
// Source: zil/gverbs.zil:1165
bool vRub() {
  hackHack("Fiddling with the ");
  return RTRUE;
}

// ZIL: <ROUTINE V-RING () ...>
// Source: zil/gverbs.zil:1162-1163
bool vRing() {
  tell("How, exactly, can you ring that?", CR);
  return RTRUE;
}

// Combat Verbs (Requirement 29)

// ZIL: <ROUTINE FIND-WEAPON (O "AUX" W) ...>
// Source: zil/1actions.zil:3401-3409
ZObject *findWeapon(const ZObject *o) {
  if (!o) return nullptr;
  for (ZObject *w : o->getContents()) {
    ObjectId id = w->getId();
    if (id == ObjectIds::STILETTO || id == ObjectIds::AXE ||
        id == ObjectIds::SWORD || id == ObjectIds::KNIFE ||
        id == ObjectIds::RUSTY_KNIFE) {
      return w;
    }
  }
  return nullptr;
}

// ZIL: <ROUTINE V-ATTACK () ...>
// Source: zil/gverbs.zil:176-193
bool vAttack() {
  auto &g = Globals::instance();
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("I've known strange people, but fighting a ", g.prso, "?", CR);
    return RTRUE;
  }
  if (!g.prsi || g.prsi == g.getObject(ObjectIds::HANDS)) {
    tell("Trying to attack a ", g.prso, " with your bare hands is suicidal.",
         CR);
    return RTRUE;
  }
  if (g.prsi->getLocation() != g.winner) {
    tell("You aren't even holding the ", g.prsi, ".", CR);
    return RTRUE;
  }
  if (!g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    tell("Trying to attack the ", g.prso, " with a ", g.prsi, " is suicidal.",
         CR);
    return RTRUE;
  }
  // ZIL: <HERO-BLOW>
  Melee::heroBlow();
  return RTRUE;
}

// ZIL: <ROUTINE V-THROW () ...>
// Source: zil/gverbs.zil:1445-1460
bool vThrow() {
  auto &g = Globals::instance();
  if (!iDrop()) {
    tell("Huh?", CR);
    return RTRUE;
  }
  if (g.prsi && g.prsi->getId() == ObjectIds::ME) {
    tell("A terrific throw! The ", g.prso);
    g.winner = g.player;
    DeathSystem::jigsUp(
        " hits you squarely in the head. Normally, this wouldn't do much "
        "damage, but by incredible mischance, you fall over backwards trying "
        "to duck, and break your neck, justice being swift and merciful in "
        "the Great Underground Empire.");
    return RTRUE;
  }
  if (g.prsi && g.prsi->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("The ", g.prsi, " ducks as the ", g.prso,
         " flies by and crashes to the ground.", CR);
    return RTRUE;
  }
  tell("Thrown.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SWING () ...>
// Source: zil/gverbs.zil:1347-1351
bool vSwing() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    tell("Whoosh!", CR);
    return RTRUE;
  }
  perform(V_ATTACK, g.prsi, g.prso);
  return RTRUE;
}

// Meta-Game Verbs (Requirement 32, 65, 66, 67, 68)

// ZIL: <ROUTINE V-SCORE ("OPTIONAL" (ASK? T)) ...>
// Source: zil/1actions.zil:4026-4045. The thresholds are <G? ,SCORE n>, so a
// score of exactly 330 is "Master", not "Wizard".
bool vScore() {
  auto &g = Globals::instance();
  tell("Your score is ");
  tell(g.score);
  tell(" (total of 350 points), in ");
  tell(g.moves);
  tell(g.moves == 1 ? " move." : " moves.");
  crlf();
  tell("This gives you the rank of ");
  if (g.score == 350) {
    tell("Master Adventurer");
  } else if (g.score > 330) {
    tell("Wizard");
  } else if (g.score > 300) {
    tell("Master");
  } else if (g.score > 200) {
    tell("Adventurer");
  } else if (g.score > 100) {
    tell("Junior Adventurer");
  } else if (g.score > 50) {
    tell("Novice Adventurer");
  } else if (g.score > 25) {
    tell("Amateur Adventurer");
  } else {
    tell("Beginner");
  }
  tell(".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-DIAGNOSE ("AUX" (MS <FIGHT-STRENGTH <>>) ...) ...>
// Source: zil/1actions.zil:3993-4025
bool vDiagnose() {
  auto &g = Globals::instance();
  const int ms = Melee::fightStrength(false);
  int wd = g.winner ? g.winner->getProperty(P_STRENGTH) : 0;
  const int rs = ms + wd;

  // ZIL: the wound count only counts while I-CURE is running.
  const TimerSystem::Interrupt *cure =
      TimerSystem::TimerManager::instance().find("I-CURE");
  if (!cure || !cure->enabled) {
    wd = 0;
  } else {
    wd = -wd;
  }

  if (wd == 0) {
    tell("You are in perfect health.");
  } else {
    tell("You have ");
    if (wd == 1) {
      tell("a light wound,");
    } else if (wd == 2) {
      tell("a serious wound,");
    } else if (wd == 3) {
      tell("several wounds,");
    } else if (wd > 3) {
      tell("serious wounds,");
    }
  }
  if (wd != 0) {
    tell(" which will be cured after ");
    tell(Melee::CURE_WAIT * (wd - 1) + (cure ? cure->tick : 0));
    tell(" moves.");
  }
  crlf();

  tell("You can ");
  if (rs == 0) {
    tell("expect death soon");
  } else if (rs == 1) {
    tell("be killed by one more light wound");
  } else if (rs == 2) {
    tell("be killed by a serious wound");
  } else if (rs == 3) {
    tell("survive one serious wound");
  } else if (rs > 3) {
    tell("survive several wounds");
  }
  tell(".", CR);

  if (DeathSystem::getDeathCount() != 0) {
    tell("You have been killed ");
    tell(DeathSystem::getDeathCount() == 1 ? "once" : "twice");
    tell(".", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-VERBOSE () ...>
// Source: zil/gverbs.zil:13-16
bool vVerbose() {
  auto &g = Globals::instance();
  g.verboseMode = true;
  g.superbriefMode = false;
  g.briefMode = false;
  tell("Maximum verbosity.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-BRIEF () ...>
// Source: zil/gverbs.zil:18-21
bool vBrief() {
  auto &g = Globals::instance();
  g.verboseMode = false;
  g.superbriefMode = false;
  g.briefMode = true;
  tell("Brief descriptions.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SUPER-BRIEF () ...>
// Source: zil/gverbs.zil:23-25. Note it sets SUPER-BRIEF without clearing
// VERBOSE, exactly as the source does.
bool vSuperbrief() {
  auto &g = Globals::instance();
  g.superbriefMode = true;
  g.briefMode = false;
  tell("Superbrief descriptions.", CR);
  return RTRUE;
}

// Game Control Verbs (Requirement 33, 60, 61, 62, 69, 70)

// ZIL: <ROUTINE V-SAVE () ...>
// Source: zil/gverbs.zil:78-83
bool vSave() {
  // TODO(H1): SaveSystem does not yet round-trip the whole state.
  if (SaveSystem::save(SAVE_FILE) == SaveSystem::SaveError::SUCCESS) {
    tell("Ok.", CR);
  } else {
    tell("Failed.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-RESTORE () ...>
// Source: zil/gverbs.zil:71-76. The interpreter owns the file, so there is
// no filename prompt.
bool vRestore() {
  // TODO(H1): SaveSystem does not yet round-trip the whole state.
  if (SaveSystem::restore(SAVE_FILE) == SaveSystem::SaveError::SUCCESS) {
    tell("Ok.", CR);
    vFirstLook();
  } else {
    tell("Failed.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-RESTART () ...>
// Source: zil/gverbs.zil:63-69
bool vRestart() {
  vScore();
  tell("Do you wish to restart? (Y is affirmative): ");
  if (yes()) {
    tell("Restarting.", CR);
    // TODO(H1): a real RESTART re-initialises the whole machine; until the
    // full state reset lands this reports the ZIL failure line.
    tell("Failed.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-VERSION ("AUX" (CNT 17)) ...>
// Source: zil/gverbs.zil:98-121. The release and serial come from the story
// header in the original; this port carries the same Release 119 /
// Serial number 880429 as zil/COMPILED/zork1.z3.
bool vVersion() {
  tell("ZORK I: The Great Underground Empire", CR);
  tell("Infocom interactive fiction - a fantasy story", CR);
  tell("Copyright (c) 1981, 1982, 1983, 1984, 1985, 1986");
  tell(" Infocom, Inc. All rights reserved.", CR);
  tell("ZORK is a registered trademark of Infocom, Inc.", CR);
  tell("Release ");
  tell(RELEASE_NUMBER);
  tell(" / Serial number ");
  tell(SERIAL_NUMBER);
  crlf();
  return RTRUE;
}

// ZIL: <ROUTINE V-SCRIPT () ...>
// Source: zil/gverbs.zil:86-90
bool vScript() {
  auto &g = Globals::instance();
  g.scripting = true;
  tell("Here begins a transcript of interaction with", CR);
  vVersion();
  return RTRUE;
}

// ZIL: <ROUTINE V-UNSCRIPT () ...>
// Source: zil/gverbs.zil:92-96
bool vUnscript() {
  auto &g = Globals::instance();
  tell("Here ends a transcript of interaction with", CR);
  vVersion();
  g.scripting = false;
  return RTRUE;
}

// Communication Verbs



// ZIL: <ROUTINE V-TELL () ...>
// Source: zil/gverbs.zil:1389-1402. With a quoted command following, the
// actor becomes WINNER for the rest of the turn.
bool vTell() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    if (g.pCont) {
      g.winner = g.prso;
      g.here = g.winner->getLocation();
    } else {
      tell("The ", g.prso,
           " pauses for a moment, perhaps thinking that you should reread the "
           "manual.",
           CR);
    }
    return RTRUE;
  }
  tell("You can't talk to the ", g.prso, "!", CR);
  g.quoteFlag = false;
  g.pCont = 0;
  return GMacros::rfatal();
}

// ZIL: <ROUTINE V-ODYSSEUS () ...>
// Source: zil/gverbs.zil:945-960
bool vOdysseus() {
  auto &g = Globals::instance();
  ZObject *cyclops = g.getObject(ObjectIds::CYCLOPS);
  if (g.here == g.getObject(RoomIds::CYCLOPS_ROOM) && cyclops &&
      cyclops->getLocation() == g.here && !g.cyclopsFlag) {
    TimerSystem::disable("I-CYCLOPS");
    g.cyclopsFlag = true;
    tell("The cyclops, hearing the name of his father's deadly nemesis, flees "
         "the room by knocking down the wall on the east of the room.",
         CR);
    g.magicFlag = true;
    cyclops->clearFlag(ObjectFlag::FIGHTBIT);
    removeCarefully(cyclops);
    return RTRUE;
  }
  tell("Wasn't he a sailor?", CR);
  return RTRUE;
}

// Easter eggs / special words - authentic ZIL responses

// ZIL: <ROUTINE V-HELLO () ...>
// Source: zil/gverbs.zil:723-734
bool vHello() {
  auto &g = Globals::instance();
  if (g.prso) {
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
      tell("The ", g.prso, " bows his head to you in greeting.", CR);
    } else {
      tell("It's a well known fact that only schizophrenics say \"Hello\" to a ",
           g.prso, ".", CR);
    }
  } else {
    tell(VerbTables::hellos().pickOne(), CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-ZORK () <TELL "At your service!" CR>>
// Source: zil/gverbs.zil:1618
bool vZork() {
  tell("At your service!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-ADVENT () <TELL "A hollow voice says \"Fool.\"" CR>>
// Source: zil/gverbs.zil:153-154
bool vPlugh() {
  tell("A hollow voice says \"Fool.\"", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-FROBOZZ () ...>
// Source: zil/gverbs.zil:704-706
bool vFrobozz() {
  tell("The FROBOZZ Corporation created, owns, and operates this dungeon.", CR);
  return RTRUE;
}

// Additional common verbs - authentic ZIL responses

// ZIL: <ROUTINE V-WAIT ("OPTIONAL" (NUM 3)) ...>
// Source: zil/gverbs.zil:1514-1519. Three clock ticks, stopping early if any
// interrupt reports something, then CLOCK-WAIT so MAIN-LOOP-1 does not tick
// again for this command.
bool vWait() {
  auto &g = Globals::instance();
  tell("Time passes...", CR);
  int num = 3;
  while (true) {
    if (--num < 0) break;
    if (TimerSystem::clocker()) break;
  }
  g.clockWait = true;
  return RTRUE;
}

// ZIL: <ROUTINE V-SWIM () ...>
// Source: zil/gverbs.zil:1324-1345
bool vSwim() {
  auto &g = Globals::instance();
  if (globalIn(ObjectIds::GLOBAL_WATER, g.here)) {
    tell("Swimming isn't usually allowed in the ");
    if (g.prso && g.prso->getId() != ObjectIds::WATER &&
        g.prso->getId() != ObjectIds::GLOBAL_WATER) {
      tell(g.prso, ".");
    } else {
      tell("dungeon.");
    }
    crlf();
    return RTRUE;
  }
  tell("Go jump in a lake!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-BACK () ...>
// Source: zil/gverbs.zil:195-196
bool vBack() {
  tell("Sorry, my memory is poor. Please give a direction.", CR);
  return RTRUE;
}

// ZIL: JUMP is a SYNONYM of LEAP (gsyntax.zil:249-255).
bool vJump() {
  return vLeap();
}

// ZIL: <ROUTINE V-CURSES () ...>
// Source: zil/gverbs.zil:374-382
bool vCurse() {
  auto &g = Globals::instance();
  if (g.prso) {
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
      tell("Insults of this nature won't help you.", CR);
    } else {
      tell("What a loony!", CR);
    }
  } else {
    tell("Such language in a high-class establishment like this!", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-OVERBOARD ("AUX" LOCN) ...>
// Source: zil/gverbs.zil:996-1009
bool vOverboard() {
  auto &g = Globals::instance();
  if (g.prsi && g.prsi->getId() == ObjectIds::TEETH) {
    ZObject *locn = g.winner ? g.winner->getLocation() : nullptr;
    if (locn && locn->hasFlag(ObjectFlag::VEHBIT)) {
      g.prso->moveTo(locn->getLocation());
      tell("Ahoy -- ", g.prso, " overboard!", CR);
    } else {
      tell("You're not in anything!", CR);
    }
    return RTRUE;
  }
  ZObject *wloc = g.winner ? g.winner->getLocation() : nullptr;
  if (wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_THROW, g.prso);
    return RTRUE;
  }
  tell("Huh?", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-MUNG () ...>
// Source: zil/gverbs.zil:938-943
bool vMung() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    perform(V_ATTACK, g.prso);
    return RTRUE;
  }
  tell("Nice try.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-WEAR () ...>
// Source: zil/gverbs.zil:1597-1602
bool vWear() {
  auto &g = Globals::instance();
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::WEARBIT)) {
    tell("You can't wear the ", g.prso, ".", CR);
    return RTRUE;
  }
  perform(V_TAKE, g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE V-FIND ("AUX" (L <LOC ,PRSO>)) ...>
// Source: zil/gverbs.zil:677-699
bool vFind() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  ZObject *l = g.prso->getLocation();
  ObjectId id = g.prso->getId();
  if (id == ObjectIds::HANDS || id == ObjectIds::LUNGS) {
    tell("Within six feet of your head, assuming you haven't left that "
         "somewhere.",
         CR);
  } else if (id == ObjectIds::ME) {
    tell("You're around here somewhere...", CR);
  } else if (l && l->getId() == ObjectIds::GLOBAL_OBJECTS) {
    tell("You find it.", CR);
  } else if (l == g.winner) {
    tell("You have it.", CR);
  } else if (l == g.here || globalIn(id, g.here) ||
             id == ObjectIds::PSEUDO_OBJECT) {
    tell("It's right here.", CR);
  } else if (l && l->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("The ", l, " has it.", CR);
  } else if (l && l->hasFlag(ObjectFlag::SURFACEBIT)) {
    tell("It's on the ", l, ".", CR);
  } else if (l && l->hasFlag(ObjectFlag::CONTBIT)) {
    tell("It's in the ", l, ".", CR);
  } else {
    tell("Beats me.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-LEAP ("AUX" TX S) ...>
// Source: zil/gverbs.zil:813-842
bool vLeap() {
  auto &g = Globals::instance();
  if (g.prso) {
    if (g.prso->getLocation() == g.here) {
      if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
        tell("The ", g.prso, " is too big to jump over.", CR);
      } else {
        return vSkip();
      }
    } else {
      tell("That would be a good trick.", CR);
    }
    return RTRUE;
  }
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  RoomExit *down = room ? room->getExit(Direction::DOWN) : nullptr;
  if (down) {
    bool fatal = false;
    if (down->type == ExitType::CONDITIONAL) {
      fatal = down->condition && !down->condition();
    } else if (down->targetRoom == 0 && !down->message.empty()) {
      fatal = true;
    }
    if (fatal) {
      tell("This was not a very safe place to try jumping.", CR);
      DeathSystem::jigsUp(VerbTables::jumploss().pickOne());
      return RTRUE;
    }
    if (g.here && g.here->getId() == RoomIds::UP_A_TREE) {
      tell("In a feat of unaccustomed daring, you manage to land on your feet "
           "without killing yourself.",
           CR, CR);
      doWalk(Direction::DOWN);
      return RTRUE;
    }
  }
  return vSkip();
}

// ZIL: <ROUTINE V-SAY ("AUX" V) ...>
// Source: zil/gverbs.zil:1167-1194
bool vSay() {
  auto &g = Globals::instance();
  if (!g.pCont) {
    tell("Say what?", CR);
    return RTRUE;
  }
  g.quoteFlag = false;
  if (ZObject *v = findIn(g.here, ObjectFlag::ACTORBIT)) {
    tell("You must address the ", v, " directly.", CR);
    g.pCont = 0;
  } else {
    g.pCont = 0;
    tell("Talking to yourself is a sign of impending mental collapse.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-KICK () <HACK-HACK "Kicking the ">>
// Source: zil/gverbs.zil:760
bool vKick() {
  hackHack("Kicking the ");
  return RTRUE;
}

// ZIL: <ROUTINE V-BREATHE () <PERFORM ,V?INFLATE ,PRSO ,LUNGS>>
// Source: zil/gverbs.zil:230-231
bool vBreathe() {
  auto &g = Globals::instance();
  perform(V_INFLATE, g.prso, g.getObject(ObjectIds::LUNGS));
  return RTRUE;
}

// ZIL: <ROUTINE V-RAPE () <TELL "What a (ahem!) strange idea." CR>>
// Source: zil/gverbs.zil:1134-1135
bool vRape() {
  tell("What a (ahem!) strange idea.", CR);
  return RTRUE;
}

// Phase 10.3 Batch 1: Movement & Positioning

// ZIL: <ROUTINE V-CLIMB-FOO () <V-CLIMB-UP ,P?UP ,PRSO>>
// Source: zil/gverbs.zil:281-288
bool vClimbFoo() { return climbUp(Direction::UP, true); }

// ZIL: <ROUTINE V-CLIMB-ON () ...>
// Source: zil/gverbs.zil:290-298
bool vClimbOn() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_BOARD, g.prso);
    return RTRUE;
  }
  tell("You can't climb onto the ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-DIG () ...>
// Source: zil/gverbs.zil:405-416
bool vDig() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    g.prsi = g.getObject(ObjectIds::HANDS);
  }
  if (g.prsi && g.prsi->getId() == ObjectIds::SHOVEL) {
    tell("There's no reason to be digging here.", CR);
    return RTRUE;
  }
  if (g.prsi && g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
    tell("Digging with the ", g.prsi, " is slow and tedious.", CR);
  } else {
    tell("Digging with a ", g.prsi, " is silly.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-FILL () ...>
// Source: zil/gverbs.zil:664-676
bool vFill() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    if (globalIn(ObjectIds::GLOBAL_WATER, g.here)) {
      perform(V_FILL, g.prso, g.getObject(ObjectIds::GLOBAL_WATER));
      return RTRUE;
    }
    ZObject *water = g.getObject(ObjectIds::WATER);
    if (water && g.winner && water->getLocation() == g.winner->getLocation()) {
      perform(V_FILL, g.prso, water);
      return RTRUE;
    }
    tell("There's nothing to fill it with.", CR);
    return RTRUE;
  }
  tell("You may know how to do that, but I don't.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-THROUGH ("OPTIONAL" (OBJ <>) "AUX" M) ...>
// Source: zil/gverbs.zil:1404-1443
bool vThrough() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    if (auto dir = otherSideDir(g.prso)) {
      doWalk(*dir);
      return RTRUE;
    }
  }
  if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_BOARD, g.prso);
    return RTRUE;
  }
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
    tell("You hit your head against the ", g.prso,
         " as you attempt this feat.", CR);
    return RTRUE;
  }
  if (g.prso->getLocation() == g.winner) {
    tell("That would involve quite a contortion!", CR);
    return RTRUE;
  }
  tell(VerbTables::yuks().pickOne(), CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-STAND () ...>
// Source: zil/gverbs.zil:1305-1310
bool vStand() {
  auto &g = Globals::instance();
  ZObject *wloc = g.winner ? g.winner->getLocation() : nullptr;
  if (wloc && wloc->hasFlag(ObjectFlag::VEHBIT)) {
    perform(V_DISEMBARK, wloc);
    return RTRUE;
  }
  tell("You are already standing, I think.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-ALARM () ...>
// Source: zil/gverbs.zil:156-168. A negative STRENGTH means unconscious.
bool vAlarm() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    if (g.prso->getProperty(P_STRENGTH) < 0) {
      tell("The ", g.prso, " is rudely awakened.", CR);
      // TODO(E2): ZIL calls AWAKEN, which restores the villain's strength.
    } else {
      tell("He's wide awake, or haven't you noticed...", CR);
    }
    return RTRUE;
  }
  tell("The ", g.prso, " isn't sleeping.", CR);
  return RTRUE;
}



// ZIL: <ROUTINE V-LAUNCH () ...>
// Source: zil/gverbs.zil:804-808
bool vLaunch() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    tell("You can't launch that by saying \"launch\"!", CR);
  } else {
    tell("That's pretty weird.", CR);
  }
  return RTRUE;
}

// Phase 10.3 Batch 2: Manipulation

// ZIL: <ROUTINE V-CUT () ...>
// Source: zil/gverbs.zil:384-400. The "smanship" suffix is concatenated onto
// the weapon's name, so a knife gives "knifesmanship".
bool vCut() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    perform(V_ATTACK, g.prso, g.prsi);
    return RTRUE;
  }
  if (g.prso && g.prso->hasFlag(ObjectFlag::BURNBIT) && g.prsi &&
      g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    if (g.winner && g.winner->getLocation() == g.prso) {
      tell("Not a bright idea, especially since you're in it.", CR);
      return RTRUE;
    }
    removeCarefully(g.prso);
    tell("Your skillful ", g.prsi, "smanship slices the ", g.prso,
         " into innumerable slivers which blow away.", CR);
    return RTRUE;
  }
  if (!g.prsi || !g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    tell("The \"cutting edge\" of a ", g.prsi, " is hardly adequate.", CR);
    return RTRUE;
  }
  tell("Strange concept, cutting the ", g.prso, "....", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LOWER () <HACK-HACK "Playing in this way with the ">>
// Source: zil/gverbs.zil:902
bool vLower() {
  hackHack("Playing in this way with the ");
  return RTRUE;
}

// ZIL: <ROUTINE V-RAISE () <V-LOWER>>
// Source: zil/gverbs.zil:1131-1132
bool vRaise() {
  return vLower();
}

// ZIL: <ROUTINE V-WIND () <TELL "You cannot wind up a " D ,PRSO "." CR>>
// Source: zil/gverbs.zil:1607-1608
bool vWind() {
  auto &g = Globals::instance();
  tell("You cannot wind up a ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-MAKE () <TELL "You can't do that." CR>>
// Source: zil/gverbs.zil:904-905
bool vMake() {
  tell("You can't do that.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-MELT () ...>
// Source: zil/gverbs.zil:907-908
bool vMelt() {
  auto &g = Globals::instance();
  tell("It's not clear that a ", g.prso, " can be melted.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PLAY () ...>
// Source: zil/gverbs.zil:1013-1019
bool vPlay() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("You become so engrossed in the role of the ", g.prso,
         " that you kill yourself, just as he might have done!", CR);
    DeathSystem::jigsUp("");
    return RTRUE;
  }
  tell("That's silly!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PLUG () <TELL "This has no effect." CR>>
// Source: zil/gverbs.zil:1021-1022
bool vPlug() {
  tell("This has no effect.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-POUR-ON () ...>
// Source: zil/gverbs.zil:1024-1046
bool vPourOn() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->getId() == ObjectIds::WATER) {
    removeCarefully(g.prso);
    if (GMacros::isFlaming(g.prsi)) {
      tell("The ", g.prsi, " is extinguished.", CR);
      g.prsi->clearFlag(ObjectFlag::ONBIT);
      g.prsi->clearFlag(ObjectFlag::FLAMEBIT);
    } else {
      tell("The water spills over the ", g.prsi,
           ", to the floor, and evaporates.", CR);
    }
    return RTRUE;
  }
  if (g.prso && g.prso->getId() == ObjectIds::PUTTY) {
    perform(V_PUT, g.prso, g.prsi);
    return RTRUE;
  }
  tell("You can't pour that.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUSH-TO () ...>
// Source: zil/gverbs.zil:1072-1073
bool vPushTo() {
  tell("You can't push things to that.", CR);
  return RTRUE;
}



// ZIL: <ROUTINE V-SHAKE () ...>
// Source: zil/gverbs.zil:1213-1240
bool vShake() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("This seems to have no effect.", CR);
    return RTRUE;
  }
  if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
    tell("You can't take it; thus, you can't shake it!", CR);
    return RTRUE;
  }
  if (g.prso->hasFlag(ObjectFlag::CONTBIT)) {
    if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
      if (!g.prso->getContents().empty()) {
        shakeLoop(g.prso);
        tell("The contents of the ", g.prso, " spill ");
        if (g.here && !g.here->hasFlag(ObjectFlag::RLANDBIT)) {
          tell("out and disappears");
        } else {
          tell("to the ground");
        }
        tell(".", CR);
      } else {
        tell("Shaken.", CR);
      }
    } else if (!g.prso->getContents().empty()) {
      tell("It sounds like there is something inside the ", g.prso, ".", CR);
    } else {
      tell("The ", g.prso, " sounds empty.", CR);
    }
    return RTRUE;
  }
  tell("Shaken.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SPIN () <TELL "You can't spin that!" CR>>
// Source: zil/gverbs.zil:1281-1282
bool vSpin() {
  tell("You can't spin that!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SQUEEZE () ...>
// Source: zil/gverbs.zil:1287-1292
bool vSqueeze() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("The ", g.prso, " does not understand this.");
  } else {
    tell("How singularly useless.");
  }
  crlf();
  return RTRUE;
}

// ZIL: <ROUTINE V-TIE-UP () ...>
// Source: zil/gverbs.zil:1471-1472
bool vTieUp() {
  tell("You could certainly never tie it with that!", CR);
  return RTRUE;
}

// Phase 10.3 Batch 3: Interactions

// ZIL: <ROUTINE V-ANSWER () ...>
// Source: zil/gverbs.zil:170-174
bool vAnswer() {
  auto &g = Globals::instance();
  tell("Nobody seems to be awaiting your answer.", CR);
  g.pCont = 0;
  g.quoteFlag = false;
  return RTRUE;
}

// ZIL: <ROUTINE V-REPLY () ...>
// Source: zil/gverbs.zil:1156-1160
bool vReply() {
  auto &g = Globals::instance();
  tell("It is hardly likely that the ", g.prso, " is interested.", CR);
  g.pCont = 0;
  g.quoteFlag = false;
  return RTRUE;
}

// ZIL: <ROUTINE V-COMMAND () ...>
// Source: zil/gverbs.zil:359-363
bool vCommand() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("The ", g.prso, " pays no attention.", CR);
  } else {
    tell("You cannot talk to that!", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-ECHO ("AUX" LST MAX (ECH 0) CNT) ...>
// Source: zil/gverbs.zil:526-548. With no trailing words it prints the
// fallback; the Loud Room's own routine handles the puzzle (TODO(G5)).
bool vEcho() {
  tell("echo echo ...", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-FOLLOW () <TELL "You're nuts!" CR>>
// Source: zil/gverbs.zil:701-702
bool vFollow() {
  tell("You're nuts!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-KISS () <TELL "I'd sooner kiss a pig." CR>>
// Source: zil/gverbs.zil:762-763
bool vKiss() {
  tell("I'd sooner kiss a pig.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-MUMBLE () ...>
// Source: zil/gverbs.zil:920-921
bool vMumble() {
  tell("You'll have to speak up if you expect me to hear you!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-REPENT () <TELL "It could very well be too late!" CR>>
// Source: zil/gverbs.zil:1153-1154
bool vRepent() {
  tell("It could very well be too late!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SEND () ...>
// Source: zil/gverbs.zil:1200-1204
bool vSend() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("Why would you send for the ", g.prso, "?", CR);
  } else {
    tell("That doesn't make sends.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-WISH () ...>
// Source: zil/gverbs.zil:1610-1614
bool vWish() {
  tell("With luck, your wish will come true.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SPRAY () <V-SQUEEZE>>
// Source: zil/gverbs.zil:1284-1285
bool vSpray() {
  return vSqueeze();
}

// Phase 10.3 Batch 4: Magic/Misc

// ZIL: <ROUTINE V-BLAST () ...>
// Source: zil/gverbs.zil:198-199
bool vBlast() {
  tell("You can't blast anything by using words.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-BURN () ...>
// Source: zil/gverbs.zil:251-273
bool vBurn() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::BURNBIT)) {
    bool held = g.prso->getLocation() == g.winner;
    bool inside = g.winner && g.winner->getLocation() == g.prso;
    if (held || inside) {
      removeCarefully(g.prso);
      tell("The ", g.prso);
      tell(" catches fire. Unfortunately, you were ");
      tell(inside ? "in" : "holding");
      DeathSystem::jigsUp(" it at the time.");
      return RTRUE;
    }
    removeCarefully(g.prso);
    tell("The ", g.prso, " catches fire and is consumed.", CR);
    return RTRUE;
  }
  tell("You can't burn a ", g.prso, ".", CR);
  return RTRUE;
}

// ZIL: CHANT is a SYNONYM of INCANT (gsyntax.zil:243-244).
bool vChant() {
  return vIncant();
}

// ZIL: <ROUTINE V-DISENCHANT () ...>
// Source: zil/gverbs.zil:434-466; the Zork I branch is just "Nothing happens."
bool vDisenchant() {
  tell("Nothing happens.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-ENCHANT () ...>
// Source: zil/gverbs.zil:550-618; Zork I falls through to V-DISENCHANT.
bool vEnchant() {
  return vDisenchant();
}

// ZIL: <ROUTINE V-INCANT () ...>
// Source: zil/gverbs.zil:736-755
bool vIncant() {
  auto &g = Globals::instance();
  tell("The incantation echoes back faintly, but nothing else happens.", CR);
  g.quoteFlag = false;
  g.pCont = 0;
  return RTRUE;
}

// ZIL: <ROUTINE V-WIN () <TELL "Naturally!" CR>>
// Source: zil/gverbs.zil:1604-1605
bool vWin() {
  tell("Naturally!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-TREASURE () ...>
// Source: zil/gverbs.zil:1474-1486
bool vTreasure() {
  auto &g = Globals::instance();
  if (g.here && g.here->getId() == RoomIds::NORTH_TEMPLE) {
    goTo(g.getObject(RoomIds::TREASURE_ROOM));
    return RTRUE;
  }
  if (g.here && g.here->getId() == RoomIds::TREASURE_ROOM) {
    goTo(g.getObject(RoomIds::NORTH_TEMPLE));
    return RTRUE;
  }
  tell("Nothing happens.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-STAY () <TELL "You will be lost without me!" CR>>
// Source: zil/gverbs.zil:1312-1313
bool vStay() {
  tell("You will be lost without me!", CR);
  return RTRUE;
}

// Phase 10.3 Batch 5: Cleanup & Edge Cases

// ZIL: <ROUTINE V-BRUSH () ...>
// Source: zil/gverbs.zil:233-234
bool vBrush() {
  tell("If you wish, but heaven only knows why.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-BUG () ...>
// Source: zil/gverbs.zil:236-238
bool vBug() {
  tell("Bug? Not in a flawless program like this! (Cough, cough).", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-CHOMP () <TELL "Preposterous!" CR>>
// Source: zil/gverbs.zil:276-277
bool vChomp() {
  tell("Preposterous!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-COUNT () ...>
// Source: zil/gverbs.zil:365-369
bool vCount() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->getId() == ObjectIds::BLESSINGS) {
    tell("Well, for one, you are playing Zork...", CR);
  } else {
    tell("You have lost your mind.", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-CROSS () <TELL "You can't cross that!" CR>>
// Source: zil/gverbs.zil:371-372
bool vCross() {
  tell("You can't cross that!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-HATCH () <TELL "Bizarre!" CR>>
// Source: zil/gverbs.zil:719-720
bool vHatch() {
  tell("Bizarre!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-KNOCK () ...>
// Source: zil/gverbs.zil:765-769
bool vKnock() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::DOORBIT)) {
    tell("Nobody's home.", CR);
  } else {
    tell("Why knock on a ", g.prso, "?", CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE V-LEAVE () <DO-WALK ,P?OUT>>
// Source: zil/gverbs.zil:850
bool vLeave() {
  doWalk(Direction::OUT);
  return RTRUE;
}

// ZIL: <ROUTINE V-LEAN-ON () <TELL "Getting tired?" CR>>
// Source: zil/gverbs.zil:810-811
bool vLeanOn() {
  tell("Getting tired?", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-PUMP () ...>
// Source: zil/gverbs.zil:1056-1068
bool vPump() {
  auto &g = Globals::instance();
  if (g.prsi && g.prsi->getId() != ObjectIds::PUMP) {
    tell("Pump it up with a ", g.prsi, "?", CR);
    return RTRUE;
  }
  ZObject *pump = g.getObject(ObjectIds::PUMP);
  if (pump && pump->getLocation() == g.winner) {
    perform(V_INFLATE, g.prso, pump);
    return RTRUE;
  }
  tell("It's really not clear how.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-STRIKE () ...>
// Source: zil/gverbs.zil:1315-1322
bool vStrike() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    tell("Since you aren't versed in hand-to-hand combat, you'd better attack "
         "the ",
         g.prso, " with a weapon.", CR);
    return RTRUE;
  }
  perform(V_LAMP_ON, g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE V-READ-PAGE () <PERFORM ,V?READ ,PRSO> <RTRUE>>
// Source: zil/gverbs.zil:1149-1151
bool vReadPage() {
  auto &g = Globals::instance();
  perform(V_READ, g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE V-PICK () <TELL "You can't pick that." CR>>
// Source: zil/gverbs.zil:1011
bool vPick() {
  tell("You can't pick that.", CR);
  return RTRUE;
}

bool vApply() {
  printLine("Apply what?");
  return RTRUE;
}

// Phase 12 Batch 2: Missing Verbs

// ZIL: <ROUTINE V-OIL () ...>
// Source: zil/gverbs.zil:963-964
bool vOil() {
  tell("You probably put spinach in your gas tank, too.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-STAB ("AUX" W) ...>
// Source: zil/gverbs.zil:1297-1303
bool vStab() {
  auto &g = Globals::instance();
  if (ZObject *w = findWeapon(g.winner)) {
    perform(V_ATTACK, g.prso, w);
    return RTRUE;
  }
  tell("No doubt you propose to stab the ", g.prso, " with your pinky?", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-DRINK-FROM () <TELL "How peculiar!" CR>>
// Source: zil/gverbs.zil:471-472
bool vDrinkFrom() {
  tell("How peculiar!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LOOK-UNDER () ...>
// Source: zil/gverbs.zil:899-900
bool vLookUnder() {
  tell("There is nothing but dust there.", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LOOK-BEHIND () ...>
// Source: zil/gverbs.zil:862-863
bool vLookBehind() {
  auto &g = Globals::instance();
  tell("There is nothing behind the ", g.prso, ".", CR);
  return RTRUE;
}


// ZIL: <ROUTINE V-RANDOM () ...> (gverbs.zil:134-139)
// #RANDOM n: <RANDOM <- 0 ,P-NUMBER>> reseeds the interpreter's generator.
bool vRandom() {
  auto &g = Globals::instance();
  if (!g.prso || g.prso->getId() != ObjectIds::INTNUM) {
    printLine("Illegal call to #RND.");
    return RFALSE;
  }
  GMacros::seedRandom(static_cast<uint32_t>(g.pNumber));
  return RTRUE;
}

// ZIL: <ROUTINE V-RECORD () <DIROUT 4> <RTRUE>> (gverbs.zil:141-143)
// The interpreter's command-recording stream has no equivalent here.
bool vRecord() {
  return RTRUE;
}

// ZIL: <ROUTINE V-UNRECORD () <DIROUT -4> <RTRUE>> (gverbs.zil:145-147)
bool vUnrecord() {
  return RTRUE;
}

// ZIL: <ROUTINE V-VERIFY () ...> (gverbs.zil:123-128)
bool vVerify() {
  printLine("Verifying disk...");
  printLine("The disk is correct.");
  return RTRUE;
}

// ZIL: <ROUTINE V-THROW-OFF () ...>
// Source: zil/gverbs.zil:1462-1463
bool vThrowOff() {
  tell("You can't throw anything off of that!", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-LOOK-ON () ...>
// Source: zil/gverbs.zil:892-897
bool vLookOn() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::SURFACEBIT)) {
    perform(V_LOOK_INSIDE, g.prso);
    return RTRUE;
  }
  tell("Look on a ", g.prso, "???", CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SGIVE () ...> (gverbs.zil:1210-1212)

// ZIL: <ROUTINE V-SKIP () <TELL <PICK-ONE ,WHEEEEE> CR>>
// Source: zil/gverbs.zil:1269-1270
bool vSkip() {
  tell(VerbTables::wheeeee().pickOne(), CR);
  return RTRUE;
}

// ZIL: <ROUTINE V-SSPRAY () <PERFORM ,V?SPRAY ,PRSI ,PRSO>>
// Source: zil/gverbs.zil:1294-1295
bool vSspray() {
  auto &g = Globals::instance();
  perform(V_SPRAY, g.prsi, g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE V-COMMAND-FILE () <DIRIN 1> <RTRUE>> (gverbs.zil:130-132)
// The interpreter's command-file input stream has no equivalent here.
bool vCommandFile() {
  return RTRUE;
}

// ============================================================================
// ZIL: GVERBS.ZIL Preaction Routines (zil/gverbs.zil)
// ============================================================================

// ZIL: <ROUTINE PRE-BOARD ("AUX" AV) ...>
// Source: zil/gverbs.zil:201-222. Every path but the vehicle-on-the-ground
// one ends in RFATAL.
bool preBoard() {
  auto &g = Globals::instance();
  ZObject *av = g.winner ? g.winner->getLocation() : nullptr;
  if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    if (g.prso->getLocation() != g.here) {
      tell("The ", g.prso, " must be on the ground to be boarded.", CR);
    } else if (av && av->hasFlag(ObjectFlag::VEHBIT)) {
      tell("You are already in the ", av, "!", CR);
    } else {
      return RFALSE;
    }
  } else if (g.prso && (g.prso->getId() == ObjectIds::WATER ||
                        g.prso->getId() == ObjectIds::GLOBAL_WATER)) {
    perform(V_SWIM, g.prso);
    return RTRUE;
  } else {
    tell("You have a theory on how to board a ", g.prso, ", perhaps?", CR);
  }
  return GMacros::rfatal();
}

// ZIL: <ROUTINE PRE-BURN () ...>
// Source: zil/gverbs.zil:243-249. Note the four punctuation marks.
bool preBurn() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    tellNoPrsi();
    return RTRUE;
  }
  if (GMacros::isFlaming(g.prsi)) {
    return RFALSE;
  }
  tell("With a ", g.prsi, "??!?", CR);
  return RTRUE;
}

// ZIL: <ROUTINE PRE-DROP () ...>
// Source: zil/gverbs.zil:474-478
bool preDrop() {
  auto &g = Globals::instance();
  if (g.winner && g.prso && g.prso == g.winner->getLocation()) {
    perform(V_DISEMBARK, g.prso);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-FILL ("AUX" TX) ...>
// Source: zil/gverbs.zil:646-662
bool preFill() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    if (globalIn(ObjectIds::GLOBAL_WATER, g.here)) {
      perform(V_FILL, g.prso, g.getObject(ObjectIds::GLOBAL_WATER));
      return RTRUE;
    }
    ZObject *water = g.getObject(ObjectIds::WATER);
    if (water && g.winner && water->getLocation() == g.winner->getLocation()) {
      perform(V_FILL, g.prso, water);
      return RTRUE;
    }
    tell("There is nothing to fill it with.", CR);
    return RTRUE;
  }
  if (g.prsi->getId() == ObjectIds::WATER) {
    return RFALSE;
  }
  if (g.prsi->getId() != ObjectIds::GLOBAL_WATER) {
    perform(V_PUT, g.prsi, g.prso);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-GIVE () ...>
// Source: zil/gverbs.zil:708-711
bool preGive() {
  auto &g = Globals::instance();
  if (g.prso && !isHeld(g.prso)) {
    tell("That's easy for you to say since you don't even have the ", g.prso,
         ".", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-MOVE () ...>
// Source: zil/gverbs.zil:910-912
bool preMove() {
  auto &g = Globals::instance();
  if (isHeld(g.prso)) {
    tell("You aren't an accomplished enough juggler.", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-MUNG () ...>
// Source: zil/gverbs.zil:923-936. One sentence, built in three pieces.
bool preMung() {
  auto &g = Globals::instance();
  if (!g.prsi || !g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    tell("Trying to destroy the ", g.prso, " with ");
    if (!g.prsi) {
      tell("your bare hands");
    } else {
      tell("a ", g.prsi);
    }
    tell(" is futile.", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-PUT () ...> (gverbs.zil:1075-1081)
bool prePut() {
  // ZIL: the Zork I branch of PRE-PUT is just PRE-GIVE, so PUT of something
  // the player is not holding gets "That's easy for you to say...".
  return preGive();
}

// ZIL: <ROUTINE PRE-READ () ...>
// Source: zil/gverbs.zil:1137-1141
bool preRead() {
  auto &g = Globals::instance();
  if (!g.lit) {
    tell("It is impossible to read in the dark.", CR);
    return RTRUE;
  }
  if (g.prsi && !g.prsi->hasFlag(ObjectFlag::TRANSBIT)) {
    tell("How does one look through a ", g.prsi, "?", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-SGIVE () ...> (gverbs.zil:1206-1210)
bool preSGive() {
  auto &g = Globals::instance();
  perform(V_GIVE, g.prsi, g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE PRE-TAKE () ...>
// Source: zil/gverbs.zil:1353-1380
bool preTake() {
  auto &g = Globals::instance();
  if (!g.prso) return RFALSE;
  if (g.prso->getLocation() == g.winner) {
    if (g.prso->hasFlag(ObjectFlag::WEARBIT)) {
      tell("You are already wearing it.", CR);
    } else {
      tell("You already have that!", CR);
    }
    return RTRUE;
  }
  ZObject *loc = g.prso->getLocation();
  if (loc && loc->hasFlag(ObjectFlag::CONTBIT) &&
      !loc->hasFlag(ObjectFlag::OPENBIT)) {
    tell("You can't reach something that's inside a closed container.", CR);
    return RTRUE;
  }
  if (g.prsi) {
    // ZIL: TAKE x FROM GROUND simply drops the indirect object.
    if (g.prsi == g.getObject(ObjectIds::GROUND)) {
      g.prsi = nullptr;
      return RFALSE;
    }
    if (g.prsi != g.prso->getLocation()) {
      tell("The ", g.prso, " isn't in the ", g.prsi, ".", CR);
      return RTRUE;
    }
    g.prsi = nullptr;
    return RFALSE;
  }
  if (g.winner && g.prso == g.winner->getLocation()) {
    tell("You're inside of it!", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ZIL: <ROUTINE PRE-TURN () ...>
// Source: zil/gverbs.zil:1488-1503. The BOOK is exempt from the bare-hands
// refusal in Zork I.
bool preTurn() {
  auto &g = Globals::instance();
  // ZIL: <EQUAL? ,PRSI <> ,ROOMS> is true when there is no indirect object
  // OR when it is ROOMS, which is what the (FIND RMUNGBIT) no-object idiom
  // leaves behind.
  bool noTool = !g.prsi || g.prsi->getId() == ObjectIds::ROOMS;
  if (noTool && g.prso && g.prso->getId() != ObjectIds::BOOK) {
    tell("Your bare hands don't appear to be enough.", CR);
    return RTRUE;
  }
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::TURNBIT)) {
    tell("You can't turn that!", CR);
    return RTRUE;
  }
  return RFALSE;
}

// ============================================================================
// ZIL: GVERBS.ZIL System Routines (zil/gverbs.zil)
// ============================================================================

// ZIL: <ROUTINE CCOUNT (OBJ "AUX" (CNT 0) X) ...>
// Source: zil/gverbs.zil:1979-1986. Worn objects do not count.
int ccount(const ZObject *obj) {
  if (!obj) return 0;
  int cnt = 0;
  for (const auto *child : obj->getContents()) {
    if (child && !child->hasFlag(ObjectFlag::WEARBIT)) {
      cnt++;
    }
  }
  return cnt;
}




// ZIL: <ROUTINE DO-WALK (DIR) ...> (gverbs.zil:470-473)
int doWalk(Direction dir) {
  return vWalkDir(dir);
}

// ZIL: <ROUTINE FIND-IN (WHERE WHAT) ...> (gverbs.zil:656-663)
ZObject *findIn(const ZObject *container, ObjectFlag flag) {
  if (!container) return nullptr;
  for (auto *child : container->getContents()) {
    if (child && child->hasFlag(flag)) {
      return child;
    }
  }
  return nullptr;
}

// ZIL: <ROUTINE FINISH ("AUX" WRD) ...>
// Source: zil/gverbs.zil:33-53. Loops until the player picks one of the
// three words; anything else simply asks again.
void finish() {
  vScore();
  // A test asks for non-interactive behaviour, and FINISH would otherwise end
  // the process the way the ZIL ends the game.
  if (DeathSystem::inTestMode()) return;
  while (true) {
    crlf();
    tell("Would you like to restart the game from the beginning, restore a "
         "saved game position, or end this session of the game?", CR);
    tell("(Type RESTART, RESTORE, or QUIT):", CR);
    tell(">");
    std::string line = readLine();
    size_t start = line.find_first_not_of(" \t");
    std::string word;
    if (start != std::string::npos) {
      size_t end = line.find_first_of(" \t", start);
      word = line.substr(start, end == std::string::npos ? std::string::npos
                                                         : end - start);
      for (char &c : word) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
      }
    }
    if (word == "restart") {
      // TODO(H1): a real RESTART re-initialises the whole machine.
      tell("Failed.", CR);
    } else if (word == "restore") {
      if (SaveSystem::restore(SAVE_FILE) == SaveSystem::SaveError::SUCCESS) {
        tell("Ok.", CR);
      } else {
        tell("Failed.", CR);
      }
    } else if (word == "quit" || word == "q") {
      std::exit(0);
    }
  }
}


// ZIL: <ROUTINE GLOBAL-IN? (OBJ WHERE) ...> (gverbs.zil:715-720)
bool globalIn(ObjectId objId, const ZObject *room) {
  auto &g = Globals::instance();
  if (auto *obj = g.getObject(objId)) {
    if (obj->getLocation() == room) return true;
    if (auto *zroom = dynamic_cast<const ZRoom *>(room)) {
      if (zroom->hasGlobal(objId)) return true;
    }
    if (auto *lg = g.getObject(ObjectIds::LOCAL_GLOBALS)) {
      if (obj->getLocation() == lg) {
        if (auto *zroom = dynamic_cast<const ZRoom *>(room)) {
          return zroom->hasGlobal(objId);
        }
        return true;
      }
    }
    if (auto *go = g.getObject(ObjectIds::GLOBAL_OBJECTS)) {
      if (obj->getLocation() == go) return true;
    }
  }
  return false;
}


// ZIL: <ROUTINE HACK-HACK (STR) ...>
// Source: zil/gverbs.zil:2024-2028
void hackHack(std::string_view str) {
  auto &g = Globals::instance();
  ZObject *globals = g.getObject(ObjectIds::GLOBAL_OBJECTS);
  if (g.prso && globals && g.prso->getLocation() == globals &&
      (g.prsa == V_WAVE || g.prsa == V_RAISE || g.prsa == V_LOWER)) {
    tell("The ", g.prso, " isn't here!", CR);
    return;
  }
  tell(str, g.prso, VerbTables::hoHum().pickOne(), CR);
}

// ZIL: <ROUTINE HELD? (CAN "AUX" (LOC <LOC .CAN>)) ...> (gverbs.zil:722-727)
bool isHeld(const ZObject *obj) {
  if (!obj) return false;
  auto &g = Globals::instance();
  const ZObject *loc = obj->getLocation();
  while (loc) {
    if (loc == g.winner) return true;
    loc = loc->getLocation();
  }
  return false;
}

// ZIL: <ROUTINE HIT-SPOT () ...>
// Source: zil/gverbs.zil:518-524
bool hitSpot() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->getId() == ObjectIds::WATER &&
      !globalIn(ObjectIds::GLOBAL_WATER, g.here)) {
    removeCarefully(g.prso);
  }
  tell("Thank you very much. I was rather thirsty (from all this talking, "
       "probably).",
       CR);
  return RTRUE;
}

// ZIL: <ROUTINE IDROP () ...>
// Source: zil/gverbs.zil:1966-1977
bool iDrop() {
  auto &g = Globals::instance();
  ZObject *loc = g.prso ? g.prso->getLocation() : nullptr;
  if (g.prso && g.prso->getLocation() != g.winner && loc != g.winner) {
    tell("You're not carrying the ", g.prso, ".", CR);
    return RFALSE;
  }
  if (g.prso && g.prso->getLocation() != g.winner &&
      (!loc || !loc->hasFlag(ObjectFlag::OPENBIT))) {
    tell("The ", g.prso, " is closed.", CR);
    return RFALSE;
  }
  if (g.prso && g.winner) {
    g.prso->moveTo(g.winner->getLocation());
  }
  return RTRUE;
}

// ZIL: <ROUTINE ITAKE ("OPTIONAL" (VB T) "AUX" CNT OBJ) ...>
// Source: zil/gverbs.zil:1900-1964
// Returns RFALSE, RFATAL (the load check) or RTRUE; V-TAKE only prints
// "Taken." when the value is exactly T, so RFATAL stays silent there while
// V-PUT's <NOT <ITAKE>> treats it as a success (gverbs.zil:1108).
int iTake(bool vb) {
  auto &g = Globals::instance();
  if (!g.prso) {
    return RFALSE;
  }
  // ZIL: (,DEAD <COND (.VB <TELL "Your hand passes through its object." CR>)>
  //       <RFALSE>)
  if (DeathSystem::isDead()) {
    if (vb) {
      tell("Your hand passes through its object.", CR);
    }
    return RFALSE;
  }
  // ZIL: (<NOT <FSET? ,PRSO ,TAKEBIT>> <TELL <PICK-ONE ,YUKS> CR> <RFALSE>)
  if (!g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
    if (vb) {
      tell(VerbTables::yuks().pickOne(), CR);
    }
    return RFALSE;
  }
  ZObject *loc = g.prso->getLocation();
  // ZIL: ;"Kludge for parser calling itake" - a closed container's contents
  // fail silently so ITAKE-CHECK can fall through to the verb's own message.
  if (loc && loc->hasFlag(ObjectFlag::CONTBIT) &&
      !loc->hasFlag(ObjectFlag::OPENBIT)) {
    return RFALSE;
  }
  // ZIL: (<AND <NOT <IN? <LOC ,PRSO> ,WINNER>>
  //            <G? <+ <WEIGHT ,PRSO> <WEIGHT ,WINNER>> ,LOAD-ALLOWED>> ...)
  if (!(loc && loc->getLocation() == g.winner) &&
      weight(g.prso) + weight(g.winner) > g.loadAllowed) {
    if (vb) {
      tell("Your load is too heavy");
      if (g.loadAllowed < g.loadMax) {
        tell(", especially in light of your condition.");
      } else {
        tell(".");
      }
      crlf();
    }
    return GMacros::rfatal();
  }
  // ZIL: (<AND <VERB? TAKE> <G? <SET CNT <CCOUNT ,WINNER>> ,FUMBLE-NUMBER>
  //            <PROB <* .CNT ,FUMBLE-PROB>>> ...)
  int cnt = 0;
  if (g.prsa == V_TAKE && (cnt = ccount(g.winner)) > FUMBLE_NUMBER &&
      GMacros::prob(cnt * FUMBLE_PROB)) {
    tell("You're holding too many things already!", CR);
    return RFALSE;
  }
  g.prso->moveTo(g.winner);
  g.prso->clearFlag(ObjectFlag::NDESCBIT);
  g.prso->setFlag(ObjectFlag::TOUCHBIT);
  scoreObj(g.prso);
  return RTRUE;
}

// ZIL: <ROUTINE LKP (STR) ...> (gverbs.zil:880-890)
bool lkp(std::string_view text) {
  printLine(text);
  return true;
}

// ZIL: <ROUTINE MUNG-ROOM (RM STR) ...> (gverbs.zil:2183-2189)
void mungRoom(ZObject *room, std::string_view desc) {
  if (room) {
    room->setFlag(ObjectFlag::RMUNGBIT);
    room->setLongDesc(desc);
  }
}


// ZIL: <ROUTINE OTHER-SIDE (DOBJ "AUX" (P 0) TX) ...>
// Source: zil/gverbs.zil:2173-2181. Scans HERE's exits for the DEXIT whose
// door is DOBJ and returns that direction.
std::optional<Direction> otherSideDir(const ZObject *door) {
  auto &g = Globals::instance();
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  if (!room || !door) return std::nullopt;
  for (const auto &[dir, exit] : room->getExits()) {
    if (exit.type == ExitType::DOOR && exit.doorObject == door->getId()) {
      return dir;
    }
  }
  return std::nullopt;
}

ZObject *otherSide(const ZObject *door) {
  auto &g = Globals::instance();
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  if (auto dir = otherSideDir(door); dir && room) {
    if (RoomExit *exit = room->getExit(*dir)) {
      return g.getObject(exit->targetRoom);
    }
  }
  return nullptr;
}


// ZIL: <ROUTINE PRINT-CONTENTS (OBJ ...) ...>
// Source: zil/gverbs.zil:1730-1748. "a X, a Y, and a Z" on one line, and
// THIS-IS-IT when the container held exactly one thing.
void printContents(const ZObject *obj) {
  if (!obj) return;
  auto contents = obj->getContents();
  if (contents.empty()) return;
  const ZObject *it = nullptr;
  bool two = false;
  bool first = true;
  for (size_t idx = 0; idx < contents.size(); ++idx) {
    const ZObject *f = contents[idx];
    bool hasNext = idx + 1 < contents.size();
    if (first) {
      first = false;
    } else {
      tell(", ");
      if (!hasNext) {
        tell("and ");
      }
    }
    tell("a ", f);
    if (!it && !two) {
      it = f;
    } else {
      two = true;
      it = nullptr;
    }
  }
  if (it && !two) {
    thisIsIt(const_cast<ZObject *>(it));
  }
}

// ZIL: <ROUTINE REMOVE-CAREFULLY (OBJ "AUX" OLIT) ...>
// Source: zil/gverbs.zil:610-618
void removeCarefully(ZObject *obj) {
  auto &g = Globals::instance();
  if (!obj) return;
  if (obj == g.pItObject) {
    g.pItObject = nullptr;
    g.it = nullptr;
  }
  bool olit = g.lit;
  obj->moveTo(nullptr);
  g.lit = GParser::isLit(g.here);
  if (olit && olit != g.lit) {
    tell("You are left in the dark...", CR);
  }
}

// ZIL: <ROUTINE SCORE-OBJ (OBJ "AUX" TEMP) ...>
// Source: zil/gverbs.zil:1867-1870. The VALUE is zeroed so it is only ever
// awarded once.
void scoreObj(ZObject *obj) {
  if (!obj) return;
  int temp = obj->getProperty(P_VALUE);
  if (temp > 0) {
    scoreUpd(temp);
    obj->setProperty(P_VALUE, 0);
  }
}

// ZIL: <ROUTINE SCORE-UPD (NUM) ...>
// Source: zil/gverbs.zil:1851-1865
bool scoreUpd(int num) {
  auto &g = Globals::instance();
  g.baseScore += num;
  g.score += num;
  if (g.score == 350 && !g.wonFlag) {
    g.wonFlag = true;
    if (ZObject *map = g.getObject(ObjectIds::MAP)) {
      map->clearFlag(ObjectFlag::INVISIBLE);
    }
    if (ZObject *woh = g.getObject(RoomIds::WEST_OF_HOUSE)) {
      woh->clearFlag(ObjectFlag::TOUCHBIT);
    }
    tell("An almost inaudible voice whispers in your ear, \"Look to your "
         "treasures for the final secret.\"",
         CR);
  }
  return RTRUE;
}

// ZIL: <ROUTINE SEE-INSIDE? (OBJ) ...>
// Source: zil/gverbs.zil:1839-1841
bool seeInside(const ZObject *obj) {
  return obj && !obj->hasFlag(ObjectFlag::INVISIBLE) &&
         (obj->hasFlag(ObjectFlag::TRANSBIT) ||
          obj->hasFlag(ObjectFlag::OPENBIT));
}

// ZIL: <ROUTINE SHAKE-LOOP ("AUX" X) ...>
// Source: zil/gverbs.zil:1242-1267
bool shakeLoop(ZObject *obj) {
  auto &g = Globals::instance();
  ZObject *target = g.here;
  if (g.here && g.here->getId() == RoomIds::UP_A_TREE) {
    target = g.getObject(RoomIds::FOREST_PATH);
  } else if (g.here && !g.here->hasFlag(ObjectFlag::RLANDBIT)) {
    target = g.getObject(ObjectIds::PSEUDO_OBJECT);
  }
  ZObject *src = obj ? obj : g.prso;
  if (!src) return RFALSE;
  while (!src->getContents().empty()) {
    ZObject *x = src->getContents().front();
    x->setFlag(ObjectFlag::TOUCHBIT);
    x->moveTo(target);
  }
  return RTRUE;
}

// ZIL: <ROUTINE TELL-NO-PRSI () <TELL "You didn't say with what!" CR>>
// Source: zil/gverbs.zil:240-241
void tellNoPrsi() {
  tell("You didn't say with what!", CR);
}

// ZIL: <ROUTINE THIS-IS-IT (OBJ) ...> (gverbs.zil:1420-1424)
void thisIsIt(ZObject *obj) {
  auto &g = Globals::instance();
  g.it = obj;
  g.pItObject = obj;
}

// ZIL: <ROUTINE WEIGHT (OBJ "AUX" CONT (WT 0)) ...>
// Source: zil/gverbs.zil:1988-1998. Anything worn by the player counts as 1;
// everything else counts its own recursive weight. The object's own SIZE is
// always added, so a container weighs its SIZE plus its contents.
int weight(const ZObject *obj) {
  if (!obj) return 0;
  auto &g = Globals::instance();
  int wt = 0;
  for (const auto *cont : obj->getContents()) {
    if (obj == g.player && cont->hasFlag(ObjectFlag::WEARBIT)) {
      wt += 1;
    } else {
      wt += weight(cont);
    }
  }
  return wt + obj->getProperty(P_SIZE);
}

// ZIL: <ROUTINE YES? () ...>
// Source: zil/gverbs.zil:1872-1877. Only the FIRST word is inspected, and
// only YES or Y count.
bool yes() {
  tell(">");
  std::string response = readLine();
  size_t start = response.find_first_not_of(" \t");
  if (start == std::string::npos) return RFALSE;
  size_t end = response.find_first_of(" \t", start);
  std::string word = response.substr(start, end == std::string::npos
                                                ? std::string::npos
                                                : end - start);
  for (char &c : word) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return word == "yes" || word == "y";
}

} // namespace Verbs
