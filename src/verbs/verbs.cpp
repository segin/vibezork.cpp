#include "verbs.h"
#include "verb_tables.h"
#include "../systems/combat.h"
#include "../systems/death.h"
#include "../systems/npc.h"
#include "../systems/score.h"
#include "core/globals.h"
#include "core/gmacros.h"
#include "core/gmain.h"
#include "core/io.h"
#include "parser/gparser.h"
#include "parser/parser.h"
#include "systems/lamp.h"
#include "systems/light.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"
#include <fstream>
#include <sstream>

// External state from actions.cpp
extern bool damGatesOpen;

namespace Verbs {

// ZIL: <GLOBAL FUMBLE-NUMBER 7> <GLOBAL FUMBLE-PROB 8> (gverbs.zil:1896-1898)
constexpr int FUMBLE_NUMBER = 7;
constexpr int FUMBLE_PROB = 8;

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



bool vQuit() {
  // Authentic Zork: V-QUIT from gverbs.zil
  // First show score
  vScore();

  // Ask for confirmation - authentic message
  print("Do you wish to leave the game? (Y is affirmative): ");
  std::string response = readLine();

  // Convert to lowercase for comparison
  for (char &c : response) {
    c = std::tolower(c);
  }

  // Accept Y/YES
  if (response == "yes" || response == "y") {
    exit(0);
  }

  // Player declines - authentic response
  printLine("Ok.");
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

bool vLock() {
  auto &g = Globals::instance();

  // PRE-LOCK checks (Requirement 24, 34)

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to lock?");
    return RTRUE;
  }

  // Check if key is specified
  if (!g.prsi) {
    printLine("What do you want to lock it with?");
    return RTRUE;
  }

  // Verify object can be locked (has DOORBIT or CONTBIT flag)
  if (!g.prso->hasFlag(ObjectFlag::DOORBIT) &&
      !g.prso->hasFlag(ObjectFlag::CONTBIT)) {
    printLine("You can't lock that.");
    return RTRUE;
  }

  // Verify player has the key (must be in inventory or accessible)
  if (!isObjectAccessible(g.prsi)) {
    printLine("You don't have that.");
    return RTRUE;
  }

  // Verify the key has TOOLBIT flag (is a tool/key)
  if (!g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
    printLine("You can't lock anything with that.");
    return RTRUE;
  }

  // Call object action handler first (Requirement 24)
  // This allows objects to override default behavior (e.g., check for correct
  // key)
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default LOCK behavior per ZIL V-LOCK (gverbs.zil:855-856)
  printLine("It doesn't seem to work.");

  return RTRUE;
}

bool vUnlock() {
  auto &g = Globals::instance();

  // PRE-UNLOCK checks (Requirement 24, 34)

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to unlock?");
    return RTRUE;
  }

  // Check if key is specified
  if (!g.prsi) {
    printLine("What do you want to unlock it with?");
    return RTRUE;
  }

  // Verify player has the key (must be in inventory or accessible)
  if (!isObjectAccessible(g.prsi)) {
    printLine("You don't have that.");
    return RTRUE;
  }

  // Verify the key has TOOLBIT flag (is a tool/key)
  if (!g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
    printLine("You can't unlock anything with that.");
    return RTRUE;
  }

  // Call object action handler first (Requirement 24)
  // This allows objects to override default behavior (e.g., check for correct
  // key)
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default UNLOCK behavior per ZIL V-UNLOCK (gverbs.zil:1508-1509)
  printLine("It doesn't seem to work.");

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

bool vEnter() {
  auto &g = Globals::instance();

  // Try ENTER as special movement first
  if (trySpecialMovement(V_ENTER, Direction::IN))
    return RTRUE;

  // Try regular IN direction (e.g., entering through window)
  ZRoom *currentRoom = dynamic_cast<ZRoom *>(g.here);
  if (currentRoom) {
    RoomExit *inExit = currentRoom->getExit(Direction::IN);
    if (inExit && inExit->type == ExitType::NORMAL) {
      return vWalkDir(Direction::IN);
    }
  }

  // If PRSO is set, try to enter that object
  if (g.prso) {
    // Check if it's a vehicle
    if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
      g.winner->moveTo(g.prso);
      printLine("You are now in the " + g.prso->getDesc() + ".");
      return RTRUE;
    }
    printLine("You can't enter that.");
    return RTRUE;
  }

  printLine("You can't go that way.");
  return RTRUE;
}

bool vExit() {
  auto &g = Globals::instance();

  // Try EXIT as special movement, priority to OUT
  if (trySpecialMovement(V_EXIT, Direction::OUT))
    return RTRUE;

  // Try other directions
  if (trySpecialMovement(V_EXIT, Direction::NORTH))
    return RTRUE;
  if (trySpecialMovement(V_EXIT, Direction::SOUTH))
    return RTRUE;
  if (trySpecialMovement(V_EXIT, Direction::EAST))
    return RTRUE;
  if (trySpecialMovement(V_EXIT, Direction::WEST))
    return RTRUE;

  printLine("You can't exit here.");
  return RTRUE;
}

bool vClimbUp() {
  auto &g = Globals::instance();

  // Try CLIMB as special movement UP
  if (trySpecialMovement(V_CLIMB_UP, Direction::UP))
    return RTRUE;

  // If PRSO is set, try to climb that object
  if (g.prso) {
    printLine("You can't climb that.");
    return RTRUE;
  }

  // Default to trying UP direction
  return Verbs::vWalkDir(Direction::UP);
}

bool vClimbDown() {
  auto &g = Globals::instance();

  // Try CLIMB as special movement DOWN
  if (trySpecialMovement(V_CLIMB_DOWN, Direction::DOWN))
    return RTRUE;

  // If PRSO is set, try to climb down that object
  if (g.prso) {
    printLine("You can't climb down that.");
    return RTRUE;
  }

  // Default to trying DOWN direction
  return Verbs::vWalkDir(Direction::DOWN);
}

bool vBoard() {
  auto &g = Globals::instance();

  // If PRSO is set, try to board that object
  if (g.prso) {
    // Check if object is a vehicle
    if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
      // Move player into the vehicle
      g.winner->moveTo(g.prso);
      printLine("You board the " + g.prso->getDesc() + ".");
      return RTRUE;
    }
    printLine("You can't board that.");
    return RTRUE;
  }

  printLine("What do you want to board?");
  return RTRUE;
}

bool vDisembark() {
  auto &g = Globals::instance();

  // Check if player is in a vehicle
  ZObject *location = g.winner->getLocation();
  if (location && location->hasFlag(ObjectFlag::VEHBIT)) {
    // Move player to vehicle's location
    ZObject *vehicleLocation = location->getLocation();
    if (vehicleLocation) {
      g.winner->moveTo(vehicleLocation);
      printLine("You disembark.");
      return RTRUE;
    }
  }

  printLine("You're not in anything.");
  return RTRUE;
}

bool vRead() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    // Look for readable objects in scope
    std::vector<ZObject *> readableObjects;

    // Check current room
    for (const auto *obj : g.here->getContents()) {
      if (obj->hasFlag(ObjectFlag::READBIT) &&
          !obj->hasFlag(ObjectFlag::INVISIBLE)) {
        readableObjects.push_back(const_cast<ZObject *>(obj));
      }
    }

    // Check inventory
    for (const auto *obj : g.winner->getContents()) {
      if (obj->hasFlag(ObjectFlag::READBIT)) {
        readableObjects.push_back(const_cast<ZObject *>(obj));
      }
    }

    // Check open containers in room and inventory
    auto checkContainer = [&](ZObject *container) {
      if (container->hasFlag(ObjectFlag::CONTBIT) &&
          container->hasFlag(ObjectFlag::OPENBIT)) {
        for (const auto *obj : container->getContents()) {
          if (obj->hasFlag(ObjectFlag::READBIT)) {
            readableObjects.push_back(const_cast<ZObject *>(obj));
          }
        }
      }
    };

    for (const auto *obj : g.here->getContents()) {
      checkContainer(const_cast<ZObject *>(obj));
    }
    for (const auto *obj : g.winner->getContents()) {
      checkContainer(const_cast<ZObject *>(obj));
    }

    if (readableObjects.empty()) {
      printLine("What do you want to read?");
      return RTRUE;
    }

    if (readableObjects.size() == 1) {
      // Implicit object selection
      g.prso = readableObjects[0];
      print("(" + g.prso->getDesc() + ")\n");
    } else {
      printLine("What do you want to read?");
      return RTRUE;
    }
  }

  // Check if object has READBIT flag
  if (!g.prso->hasFlag(ObjectFlag::READBIT)) {
    printLine("How does one read a " + g.prso->getDesc() + "?");
    return RTRUE;
  }

  // Check if object has text
  if (!g.prso->hasText()) {
    printLine("There is nothing written on the " + g.prso->getDesc() + ".");
    return RTRUE;
  }

  // Check if player is holding the object
  if (g.prso->getLocation() != g.winner) {
    // Check if object can be taken
    if (!g.prso->hasFlag(ObjectFlag::TAKEBIT) ||
        g.prso->hasFlag(ObjectFlag::TRYTAKEBIT)) {
      printLine("You can't take that.");
      return RTRUE;
    }

    // Check if object is accessible
    ZObject *objLocation = g.prso->getLocation();
    bool accessible = false;

    if (objLocation == g.here) {
      accessible = true;
    } else if (objLocation && objLocation->hasFlag(ObjectFlag::CONTBIT) &&
               objLocation->hasFlag(ObjectFlag::OPENBIT)) {
      ZObject *containerLocation = objLocation->getLocation();
      if (containerLocation == g.here || containerLocation == g.winner) {
        accessible = true;
      }
    }

    if (!accessible) {
      printLine("You can't see any such thing.");
      return RTRUE;
    }

    // Check inventory weight limit
    int currentWeight = 0;
    for (const auto *obj : g.winner->getContents()) {
      currentWeight += obj->getProperty(P_SIZE);
    }

    int objectSize = g.prso->getProperty(P_SIZE);
    if (objectSize == 0) {
      objectSize = 5;
    }

    if (currentWeight + objectSize > g.loadAllowed) {
      printLine("You're carrying too much.");
      return RTRUE;
    }

    // Implicit TAKE
    print("(Taken)\n");
    g.prso->moveTo(g.winner);
  }

  // Display the text
  printLine(g.prso->getText());
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

bool vTurn() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to turn?");
    return RTRUE;
  }

  // Call object action handler first
  // This allows objects to override default behavior
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default: Nothing happens
  printLine("Nothing obvious happens.");
  return RTRUE;
}

bool vPush() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to push?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-PUSH uses HACK-HACK
  print("Pushing the ");
  print(g.prso->getDesc());
  printLine(" isn't notably useful.");
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

bool vMove() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to move?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-MOVE
  if (g.prso->hasFlag(ObjectFlag::TAKEBIT)) {
    print("Moving the ");
    print(g.prso->getDesc());
    printLine(" reveals nothing.");
  } else {
    print("You can't move the ");
    print(g.prso->getDesc());
    printLine(".");
  }
  return RTRUE;
}

// Interaction Verbs (Requirement 27)

bool vTie() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to tie?");
    return RTRUE;
  }

  // Check if indirect object is specified (tie X to Y)
  if (!g.prsi) {
    printLine("What do you want to tie it to?");
    return RTRUE;
  }

  // Call object action handlers
  if (g.prso->performAction()) {
    return RTRUE;
  }
  if (g.prsi->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-TIE
  if (g.prsi == g.winner) {
    printLine("You can't tie anything to yourself.");
  } else {
    print("You can't tie the ");
    print(g.prso->getDesc());
    printLine(" to that.");
  }
  return RTRUE;
}

bool vUntie() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to untie?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-UNTIE
  printLine("This cannot be tied, so it cannot be untied!");
  return RTRUE;
}

bool vListen() {
  auto &g = Globals::instance();

  // If object is specified, listen to that object
  if (g.prso) {
    // Call object action handler
    if (g.prso->performAction()) {
      return RTRUE;
    }

    // Authentic ZIL V-LISTEN
    print("The ");
    print(g.prso->getDesc());
    printLine(" makes no sound.");
    return RTRUE;
  }

  // No object specified - listen to the room
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  if (room) {
    room->performRoomAction(M_LISTEN);
  }

  // Default - no specific message for room listening in ZIL
  printLine("You hear nothing unusual.");
  return RTRUE;
}

bool vSmell() {
  auto &g = Globals::instance();

  // If object is specified, smell that object
  if (g.prso) {
    // Call object action handler
    if (g.prso->performAction()) {
      return RTRUE;
    }

    // Authentic ZIL V-SMELL
    print("It smells like a ");
    print(g.prso->getDesc());
    printLine(".");
    return RTRUE;
  }

  // No object specified - smell the room
  printLine("You smell nothing unusual.");
  return RTRUE;
}

bool vYell() {
  auto &g = Globals::instance();

  // Call room action handler for special yell behavior
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  if (room) {
    room->performRoomAction(M_YELL);
  }

  // Default message
  printLine("Aaaarrrrgggghhhh!");
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

bool vEat() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to eat?");
    return RTRUE;
  }

  // Authentic ZIL V-EAT logic
  if (g.prso->hasFlag(ObjectFlag::FOODBIT)) {
    // Check if player is holding it
    if (g.prso->getLocation() != g.winner &&
        (!g.prso->getLocation() ||
         g.prso->getLocation()->getLocation() != g.winner)) {
      printLine("You're not holding that.");
      return RTRUE;
    }

    // Call object action handler first
    if (g.prso->performAction()) {
      return RTRUE;
    }

    // Remove and print message
    g.prso->moveTo(nullptr);
    printLine("Thank you very much. It really hit the spot.");
    return RTRUE;
  }

  // Not food - check if drinkable
  if (g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
    // Redirect to drink
    return Verbs::vDrink();
  }

  // Neither food nor drink
  print("I don't think that the ");
  print(g.prso->getDesc());
  printLine(" would agree with you.");
  return RTRUE;
}

bool vDrink() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to drink?");
    return RTRUE;
  }

  // Authentic ZIL - V-DRINK calls V-EAT
  // But check DRINKBIT specifically
  if (g.prso->hasFlag(ObjectFlag::DRINKBIT)) {
    // Call object action handler first
    if (g.prso->performAction()) {
      return RTRUE;
    }

    // Remove and print message
    g.prso->moveTo(nullptr);
    printLine("Thank you very much. I was rather thirsty (from all this "
              "talking, probably).");
    return RTRUE;
  }

  // Not drinkable
  print("I don't think that the ");
  print(g.prso->getDesc());
  printLine(" would agree with you.");
  return RTRUE;
}

// Light Source Verbs (Requirement 30)

bool vLampOn() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to turn on?");
    return RTRUE;
  }

  // Check if object has LIGHTBIT flag (is a light source)
  if (!g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
    printLine("You can't turn that on.");
    return RTRUE;
  }

  // Check if already on
  if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
    printLine("It's already on.");
    return RTRUE;
  }

  // Check if lamp has battery/fuel (Requirement 47)
  // For the lamp, check if it has been depleted
  if (g.prso->getProperty(P_CAPACITY) == 0) {
    printLine("The lamp has no more power.");
    return RTRUE;
  }

  // Call object action handler first
  // This allows objects to override default behavior
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default LAMP-ON behavior
  // Set ONBIT flag
  g.prso->setFlag(ObjectFlag::ONBIT);

  // Enable lamp timer if this is the brass lantern (Requirement 47)
  if (g.prso->getId() == ObjectIds::LAMP) {
    LampSystem::enableLampTimer();
  }

  // Update room lighting
  // This will be handled by the light system when implemented
  // For now, just set the flag

  printLine("The " + g.prso->getDesc() + " is now on.");

  return RTRUE;
}

bool vLampOff() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to turn off?");
    return RTRUE;
  }

  // Check if object has LIGHTBIT flag (is a light source)
  if (!g.prso->hasFlag(ObjectFlag::LIGHTBIT)) {
    printLine("You can't turn that off.");
    return RTRUE;
  }

  // Check if lamp is on
  if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
    printLine("It's already off.");
    return RTRUE;
  }

  // Call object action handler first
  // This allows objects to override default behavior
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default LAMP-OFF behavior
  // Clear ONBIT flag
  g.prso->clearFlag(ObjectFlag::ONBIT);

  // Disable lamp timer if this is the brass lantern (Requirement 47)
  if (g.prso->getId() == ObjectIds::LAMP) {
    LampSystem::disableLampTimer();
  }

  // Update room lighting
  // This will be handled by the light system when implemented
  // For now, just clear the flag

  printLine("The " + g.prso->getDesc() + " is now off.");

  return RTRUE;
}

// Special Action Verbs (Requirement 31)

bool vInflate() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to inflate?");
    return RTRUE;
  }

  // Call object action handler first
  // This allows objects to override default behavior (e.g., boat with pump)
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default: Can't inflate that
  printLine("You can't inflate that.");
  return RTRUE;
}

bool vDeflate() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to deflate?");
    return RTRUE;
  }

  // Call object action handler first
  // This allows objects to override default behavior (e.g., boat with pump)
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default: Can't deflate that
  printLine("You can't deflate that.");
  return RTRUE;
}

bool vPray() {
  auto &g = Globals::instance();

  // PRAY is typically a room-specific action
  // Call room action handler if present
  ZRoom *room = dynamic_cast<ZRoom *>(g.here);
  if (room) {
    room->performRoomAction(M_PRAY);
  }

  // Authentic ZIL V-PRAY default
  printLine("If you pray enough, your prayers may be answered.");
  return RTRUE;
}

bool vExorcise() {
  auto &g = Globals::instance();

  // Call object action handler first if object specified
  if (g.prso && g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-EXORCISE
  printLine("What a bizarre concept!");
  return RTRUE;
}

bool vWave() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to wave?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-WAVE uses HACK-HACK
  print("Waving the ");
  print(g.prso->getDesc());
  printLine(" isn't notably useful.");
  return RTRUE;
}

bool vRub() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to rub?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-RUB uses HACK-HACK
  print("Fiddling with the ");
  print(g.prso->getDesc());
  printLine(" isn't notably useful.");
  return RTRUE;
}

bool vRing() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to ring?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-RING
  printLine("How, exactly, can you ring that?");
  return RTRUE;
}

// Combat Verbs (Requirement 29)

bool vAttack() {
  auto &g = Globals::instance();

  // Check if target is specified
  if (!g.prso) {
    printLine("What do you want to attack?");
    return RTRUE;
  }

  // Authentic ZIL V-ATTACK logic
  // Check if target is an actor
  if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    print("I've known strange people, but fighting a ");
    print(g.prso->getDesc());
    printLine("?");
    return RTRUE;
  }

  // Check for weapon - must have one
  if (!g.prsi) {
    print("Trying to attack a ");
    print(g.prso->getDesc());
    printLine(" with your bare hands is suicidal.");
    return RTRUE;
  }

  // Check if holding the weapon
  if (g.prsi->getLocation() != g.winner) {
    print("You aren't even holding the ");
    print(g.prsi->getDesc());
    printLine(".");
    return RTRUE;
  }

  // Check if it's actually a weapon
  if (!g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    print("Trying to attack the ");
    print(g.prso->getDesc());
    print(" with a ");
    print(g.prsi->getDesc());
    printLine(" is suicidal.");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Start combat using the combat system
  CombatSystem::startCombat(g.prso, g.prsi);
  CombatSystem::processCombatRound();

  return RTRUE;
}

bool vThrow() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to throw?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Authentic ZIL V-THROW - drop the object first
  g.prso->moveTo(g.here);

  // Check if throwing at self
  if (g.prsi == g.winner) {
    print("A terrific throw! The ");
    print(g.prso->getDesc());
    printLine(" hits you squarely in the head. Normally, this wouldn't do much "
              "damage, but by incredible mischance, you fall over backwards "
              "trying to duck, and break your neck, justice being swift and "
              "merciful in the Great Underground Empire.");
    // This should trigger death - for now just return
    return RTRUE;
  }

  // Check if throwing at an actor
  if (g.prsi && g.prsi->hasFlag(ObjectFlag::ACTORBIT)) {
    print("The ");
    print(g.prsi->getDesc());
    print(" ducks as the ");
    print(g.prso->getDesc());
    printLine(" flies by and crashes to the ground.");
  } else {
    printLine("Thrown.");
  }

  return RTRUE;
}

bool vSwing() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("What do you want to swing?");
    return RTRUE;
  }

  // Authentic ZIL V-SWING
  if (!g.prsi) {
    // No target - just swing
    printLine("Whoosh!");
    return RTRUE;
  }

  // Target specified - perform attack with swapped objects
  // SWING weapon AT target -> ATTACK target WITH weapon
  ZObject *weapon = g.prso;
  ZObject *target = g.prsi;
  g.prso = target;
  g.prsi = weapon;

  bool result = vAttack();

  // Restore original values
  g.prso = weapon;
  g.prsi = target;

  return result;
}

// Meta-Game Verbs (Requirement 32, 65, 66, 67, 68)

bool vScore() {
  auto &g = Globals::instance();
  auto &scoreSystem = ScoreSystem::instance();

  int currentScore = (g.score > 0) ? g.score : scoreSystem.getScore();
  int currentMoves = g.moves;  // ZIL: ,MOVES, the only move counter

  // Display current score
  printLine(std::format("Your score is {} (total of 350 points), in {} {}.",
                        currentScore, currentMoves,
                        currentMoves == 1 ? "move" : "moves"));

  // Display rank based on score using ScoreSystem logic (ZIL: 1actions.zil:4034-4043)
  std::string_view rank = "Beginner";
  if (currentScore >= 350) rank = "Master Adventurer";
  else if (currentScore >= 330) rank = "Wizard";
  else if (currentScore >= 300) rank = "Master";
  else if (currentScore >= 200) rank = "Adventurer";
  else if (currentScore >= 100) rank = "Junior Adventurer";
  else if (currentScore >= 50) rank = "Novice Adventurer";
  else if (currentScore >= 25) rank = "Amateur Adventurer";

  printLine(std::format("This gives you the rank of {}.", rank));

  return RTRUE;
}

bool vDiagnose() {
  auto &g = Globals::instance();

  // Check if in combat and track health
  if (CombatSystem::isInCombat()) {
    auto &combatManager = CombatSystem::CombatManager::instance();
    auto playerCombatant = combatManager.getPlayerCombatant();

    if (playerCombatant && playerCombatant->object == g.winner) {
      int health = playerCombatant->health;
      int maxHealth = playerCombatant->maxHealth;

      if (health == maxHealth) {
        printLine("You are in perfect health.");
      } else if (health > maxHealth * 3 / 4) {
        printLine("You have a few grazes.");
      } else if (health > maxHealth / 2) {
        printLine("You have some serious wounds.");
      } else if (health > 0) {
        printLine("You are staggering.");
      } else {
        printLine("You are dead.");
      }
      return RTRUE;
    }
  }

  // Default / Out of combat
  // Since health is ephemeral in current CombatSystem, assume perfect health
  // when not fighting
  printLine("You are in perfect health.");

  return RTRUE;
}

bool vVerbose() {
  auto &g = Globals::instance();

  // Set verbose mode flag
  g.verboseMode = true;
  g.briefMode = false;
  g.superbriefMode = false;

  printLine("Maximum verbosity.");

  return RTRUE;
}

bool vBrief() {
  auto &g = Globals::instance();

  // Set brief mode flag
  g.verboseMode = false;
  g.briefMode = true;
  g.superbriefMode = false;

  printLine("Brief descriptions.");

  return RTRUE;
}

bool vSuperbrief() {
  auto &g = Globals::instance();

  // Set superbrief mode flag
  g.verboseMode = false;
  g.briefMode = false;
  g.superbriefMode = true;

  printLine("Superbrief descriptions.");

  return RTRUE;
}

// Game Control Verbs (Requirement 33, 60, 61, 62, 69, 70)

bool vSave() {
  auto &g = Globals::instance();

  // Prompt for filename (Requirement 60)
  printLine("Enter save filename:");
  std::string filename = readLine();

  // Handle empty filename
  if (filename.empty()) {
    printLine("Save cancelled.");
    return RTRUE;
  }

  // Add .sav extension if not present
  if (filename.find('.') == std::string::npos) {
    filename += ".sav";
  }

  // Try to open file for writing
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    printLine("Error: Could not create save file.");
    return RTRUE;
  }

  // Serialize game state (Requirement 60)
  // Format: Simple text-based format for readability and debugging

  // Write header
  file << "ZORK1_SAVE_V1\n";

  // Save player location (Requirement 60.2)
  if (g.here) {
    file << "LOCATION:" << g.here->getId() << "\n";
  } else {
    file << "LOCATION:0\n";
  }

  // Save score and moves (Requirement 60.2)
  file << "SCORE:" << g.score << "\n";
  file << "MOVES:" << g.moves << "\n";

  // Save display mode flags
  file << "VERBOSE:" << (g.verboseMode ? 1 : 0) << "\n";
  file << "BRIEF:" << (g.briefMode ? 1 : 0) << "\n";
  file << "SUPERBRIEF:" << (g.superbriefMode ? 1 : 0) << "\n";

  // Save load limits
  file << "LOADMAX:" << g.loadMax << "\n";
  file << "LOADALLOWED:" << g.loadAllowed << "\n";

  // Save all object states (Requirement 60.3, 60.4)
  file << "OBJECTS_BEGIN\n";
  for (const auto &[id, obj] : g.getAllObjects()) {
    if (!obj)
      continue;

    // Save object ID
    file << "OBJ:" << id << "\n";

    // Save object location (Requirement 60.3)
    ZObject *loc = obj->getLocation();
    if (loc) {
      file << "LOC:" << loc->getId() << "\n";
    } else {
      file << "LOC:-1\n";
    }

    // Save object flags (Requirement 60.4)
    // We save flags as a bitmask
    uint32_t flags = 0;
    for (int i = 0; i < 32; i++) {
      if (obj->hasFlag(static_cast<ObjectFlag>(1 << i))) {
        flags |= (1 << i);
      }
    }
    file << "FLAGS:" << flags << "\n";

    // Save key properties (Requirement 60.4)
    file << "P_SIZE:" << obj->getProperty(P_SIZE) << "\n";
    file << "P_CAPACITY:" << obj->getProperty(P_CAPACITY) << "\n";
    file << "P_VALUE:" << obj->getProperty(P_VALUE) << "\n";
    file << "P_STRENGTH:" << obj->getProperty(P_STRENGTH) << "\n";

    file << "OBJ_END\n";
  }
  file << "OBJECTS_END\n";

  // Close file
  file.close();

  if (file.fail()) {
    printLine("Error: Failed to write save file.");
    return RTRUE;
  }

  printLine("Game saved.");
  return RTRUE;
}

bool vRestore() {
  auto &g = Globals::instance();

  // Prompt for filename (Requirement 61)
  printLine("Enter save filename to restore:");
  std::string filename = readLine();

  // Handle empty filename
  if (filename.empty()) {
    printLine("Restore cancelled.");
    return RTRUE;
  }

  // Add .sav extension if not present
  if (filename.find('.') == std::string::npos) {
    filename += ".sav";
  }

  // Try to open file for reading
  std::ifstream file(filename);
  if (!file) {
    printLine("Error: Could not open save file.");
    return RTRUE;
  }

  // Read and verify header
  std::string line;
  if (!std::getline(file, line) || line != "ZORK1_SAVE_V1") {
    printLine("Error: Invalid or corrupted save file.");
    return RTRUE;
  }

  // Temporary storage for parsed values
  ObjectId playerLocation = 0;
  int savedScore = 0;
  int savedMoves = 0;
  bool savedVerbose = true;
  bool savedBrief = false;
  bool savedSuperbrief = false;
  int savedLoadMax = 100;
  int savedLoadAllowed = 100;

  // Map of object ID to saved state
  struct ObjectState {
    ObjectId location = static_cast<ObjectId>(-1);
    uint32_t flags = 0;
    int size = 0;
    int capacity = 0;
    int value = 0;
    int strength = 0;
  };
  std::map<ObjectId, ObjectState> objectStates;

  // Parse save file (Requirement 61.1)
  ObjectId currentObjId = 0;
  bool inObjects = false;

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
      if (line == "OBJECTS_BEGIN") {
        inObjects = true;
        continue;
      }
      if (line == "OBJECTS_END") {
        inObjects = false;
        continue;
      }
      if (line == "OBJ_END") {
        continue;
      }
      continue;
    }

    std::string key = line.substr(0, colonPos);
    std::string value = line.substr(colonPos + 1);

    if (!inObjects) {
      // Parse global state
      if (key == "LOCATION") {
        playerLocation = static_cast<ObjectId>(std::stoi(value));
      } else if (key == "SCORE") {
        savedScore = std::stoi(value);
      } else if (key == "MOVES") {
        savedMoves = std::stoi(value);
      } else if (key == "VERBOSE") {
        savedVerbose = (std::stoi(value) != 0);
      } else if (key == "BRIEF") {
        savedBrief = (std::stoi(value) != 0);
      } else if (key == "SUPERBRIEF") {
        savedSuperbrief = (std::stoi(value) != 0);
      } else if (key == "LOADMAX") {
        savedLoadMax = std::stoi(value);
      } else if (key == "LOADALLOWED") {
        savedLoadAllowed = std::stoi(value);
      }
    } else {
      // Parse object state
      if (key == "OBJ") {
        currentObjId = static_cast<ObjectId>(std::stoi(value));
        objectStates[currentObjId] = ObjectState();
      } else if (key == "LOC") {
        objectStates[currentObjId].location =
            static_cast<ObjectId>(std::stoi(value));
      } else if (key == "FLAGS") {
        objectStates[currentObjId].flags =
            static_cast<uint32_t>(std::stoul(value));
      } else if (key == "P_SIZE") {
        objectStates[currentObjId].size = std::stoi(value);
      } else if (key == "P_CAPACITY") {
        objectStates[currentObjId].capacity = std::stoi(value);
      } else if (key == "P_VALUE") {
        objectStates[currentObjId].value = std::stoi(value);
      } else if (key == "P_STRENGTH") {
        objectStates[currentObjId].strength = std::stoi(value);
      }
    }
  }

  file.close();

  // Apply restored state (Requirement 61.2, 61.3, 61.4)

  // Restore player location
  ZObject *newLocation = g.getObject(playerLocation);
  if (newLocation) {
    g.here = newLocation;
    if (g.winner) {
      g.winner->moveTo(newLocation);
    }
  }

  // Restore score and moves
  g.score = savedScore;
  g.moves = savedMoves;

  // Restore display modes
  g.verboseMode = savedVerbose;
  g.briefMode = savedBrief;
  g.superbriefMode = savedSuperbrief;

  // Restore load limits
  g.loadMax = savedLoadMax;
  g.loadAllowed = savedLoadAllowed;

  // Restore object states
  for (const auto &[id, state] : objectStates) {
    ZObject *obj = g.getObject(id);
    if (!obj)
      continue;

    // Restore location
    if (state.location == static_cast<ObjectId>(-1)) {
      obj->moveTo(nullptr);
    } else {
      ZObject *loc = g.getObject(state.location);
      if (loc) {
        obj->moveTo(loc);
      }
    }

    // Restore flags
    for (int i = 0; i < 32; i++) {
      ObjectFlag flag = static_cast<ObjectFlag>(1 << i);
      if (state.flags & (1 << i)) {
        obj->setFlag(flag);
      } else {
        obj->clearFlag(flag);
      }
    }

    // Restore properties
    if (state.size > 0)
      obj->setProperty(P_SIZE, state.size);
    if (state.capacity > 0)
      obj->setProperty(P_CAPACITY, state.capacity);
    if (state.value > 0)
      obj->setProperty(P_VALUE, state.value);
    if (state.strength > 0)
      obj->setProperty(P_STRENGTH, state.strength);
  }

  printLine("Game restored.");

  // Show current location
  Verbs::vLook();

  return RTRUE;
}

bool vRestart() {
  // Confirm with player (Requirement 62.5)
  printLine("Are you sure you want to restart? (yes/no)");
  std::string response = readLine();

  // Convert to lowercase for comparison
  for (char &c : response) {
    c = std::tolower(c);
  }

  if (response != "yes" && response != "y") {
    printLine("Restart cancelled.");
    return RTRUE;
  }

  auto &g = Globals::instance();

  // Reset all game state (Requirement 62.1, 62.4)
  g.score = 0;
  g.moves = 0;
  g.verboseMode = true;
  g.briefMode = false;
  g.superbriefMode = false;
  g.loadMax = 100;
  g.loadAllowed = 100;
  g.lit = false;
  g.pCont = false;
  g.quoteFlag = false;

  // Clear all objects and reinitialize world (Requirement 62.2, 62.3)
  g.reset();

  // Reinitialize the world
  initializeWorld();

  printLine("ZORK I: The Great Underground Empire");
  printLine(
      "Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.");
  printLine("ZORK is a registered trademark of Infocom, Inc.");
  printLine("C++ Port - Release 1");
  crlf();

  // Show starting location
  Verbs::vLook();

  return RTRUE;
}

bool vVersion() {
  // Display game name and version (Requirement 69.2)
  printLine("ZORK I: The Great Underground Empire");

  // Display port information (Requirement 69.3)
  printLine("C++ Port - Release 1");

  // Display copyright (Requirement 69.4)
  printLine(
      "Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.");
  printLine("ZORK is a registered trademark of Infocom, Inc.");

  // Display interpreter info
  printLine("Interpreter: C++23 Native");

  return RTRUE;
}

bool vScript() {
  auto &g = Globals::instance();
  if (g.scripting) {
    printLine("Scripting is already on.");
    return RTRUE;
  }
  g.scripting = true;
  printLine("Scripting initiated.");
  return RTRUE;
}

bool vUnscript() {
  auto &g = Globals::instance();
  if (!g.scripting) {
    printLine("Scripting is already off.");
    return RTRUE;
  }
  g.scripting = false;
  printLine("Scripting ended.");
  return RTRUE;
}

// Communication Verbs

bool vTalk() {
  auto &g = Globals::instance();

  // Check if target is specified
  if (!g.prso) {
    printLine("Talk to whom?");
    return RTRUE;
  }

  // Check if target is an actor/NPC
  if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    printLine("You can't talk to that.");
    return RTRUE;
  }

  // Call object action handler for custom responses
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default response
  printLine("There is no response.");
  return RTRUE;
}

bool vAsk() {
  auto &g = Globals::instance();

  // Check if target is specified
  if (!g.prso) {
    printLine("Ask whom?");
    return RTRUE;
  }

  // Check if target is an actor/NPC
  if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    printLine("You can't ask that.");
    return RTRUE;
  }

  // Call object action handler for custom responses
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default response
  printLine("There is no response.");
  return RTRUE;
}

bool vTell() {
  auto &g = Globals::instance();

  // Check if target is specified
  if (!g.prso) {
    printLine("Tell whom?");
    return RTRUE;
  }

  // Check if target is an actor/NPC
  if (!g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
    printLine("You can't tell that anything.");
    return RTRUE;
  }

  // Call object action handler for custom responses
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default response
  printLine("There is no response.");
  return RTRUE;
}

bool vOdysseus() {
  // Special verb for the cyclops puzzle
  // This is called when player types "odysseus" or "ulysses"
  return NPCSystem::handleOdysseus() ? RTRUE : RFALSE;
}

// Easter eggs / special words - authentic ZIL responses

bool vHello() {
  auto &g = Globals::instance();

  // If object specified, greet that object
  if (g.prso) {
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
      print("The ");
      print(g.prso->getDesc());
      printLine(" bows his head to you in greeting.");
    } else {
      print("It's a well known fact that only schizophrenics say \"Hello\" to "
            "a ");
      print(g.prso->getDesc());
      printLine(".");
    }
    return RTRUE;
  }

  // No object - generic hello responses
  // ZIL has a PICK-ONE from HELLOS table
  static const char *hellos[] = {"Hello.", "Good day.",
                                 "Nice weather we've been having lately.",
                                 "Goodbye."};
  static int helloIdx = 0;
  printLine(hellos[helloIdx % 4]);
  helloIdx++;
  return RTRUE;
}

bool vZork() {
  // Authentic ZIL V-ZORK
  printLine("At your service!");
  return RTRUE;
}

bool vPlugh() {
  // Authentic ZIL V-ADVENT (handles PLUGH and XYZZY)
  // Reference to Colossal Cave Adventure
  printLine("A hollow voice says \"Fool.\"");
  return RTRUE;
}

bool vFrobozz() {
  // Authentic ZIL V-FROBOZZ
  printLine(
      "The FROBOZZ Corporation created, owns, and operates this dungeon.");
  return RTRUE;
}

// Additional common verbs - authentic ZIL responses

bool vWait() {
  // Authentic ZIL V-WAIT - passes time
  printLine("Time passes...");
  // In full implementation, this would call CLOCKER multiple times
  return RTRUE;
}

bool vSwim() {
  // Authentic ZIL V-SWIM
  printLine("Go jump in a lake!");
  return RTRUE;
}

bool vBack() {
  // Authentic ZIL V-BACK
  printLine("Sorry, my memory is poor. Please give a direction.");
  return RTRUE;
}

bool vJump() {
  // Authentic ZIL V-LEAP/V-SKIP responses
  static const char *responses[] = {
      "Very good. Now you can go to the second grade.",
      "Are you enjoying yourself?", "Wheeeeeeeeee!!!!!",
      "Do you expect me to applaud?"};
  static int idx = 0;
  printLine(responses[idx % 4]);
  idx++;
  return RTRUE;
}

bool vCurse() {
  auto &g = Globals::instance();

  // Authentic ZIL V-CURSES
  if (g.prso) {
    if (g.prso->hasFlag(ObjectFlag::ACTORBIT)) {
      printLine("Insults of this nature won't help you.");
    } else {
      printLine("What a loony!");
    }
  } else {
    printLine("Such language in a high-class establishment like this!");
  }
  return RTRUE;
}

// V-OVERBOARD - Throw something overboard from a vehicle
// ZIL: gverbs.zil lines 996-1009
bool vOverboard() {
  auto &g = Globals::instance();
  ZObject *winner = g.winner ? g.winner : g.player;
  ZObject *locn = winner ? winner->getLocation() : nullptr;

  if (g.prsi && g.prsi->getId() == ObjectIds::TEETH) {
    if (locn && locn->hasFlag(ObjectFlag::VEHBIT)) {
      if (g.prso) {
        g.prso->moveTo(locn->getLocation());
        printLine(std::format("Ahoy -- {} overboard!", g.prso->getDesc()));
        return RTRUE;
      }
    } else {
      printLine("You're not in anything!");
      return RTRUE;
    }
  }

  if (locn && locn->hasFlag(ObjectFlag::VEHBIT)) {
    return vThrow();
  }

  printLine("Huh?");
  return RTRUE;
}

// New ZIL Audit Verbs implementation
bool vMung() {
  printLine("Trying to destroy things is a waste of time.");
  return RTRUE;
}

bool vWear() {
  printLine("You can't wear that!");
  return RTRUE;
}

bool vFind() {
  print("You can't see any ");
  if (Globals::instance().prso)
    print(Globals::instance().prso->getDesc());
  printLine(" here.");
  return RTRUE;
}

bool vLeap() {
  return vJump(); // Share logic with JUMP
}

bool vSay() {
  printLine("Talking to yourself is a sign of impending mental collapse.");
  return RTRUE;
}

bool vKick() {
  printLine("Violence isn't the answer to this one.");
  return RTRUE;
}

bool vBreathe() {
  printLine("How can you breathe that?");
  return RTRUE;
}

bool vRape() {
  printLine("What a loony!");
  return RTRUE;
}

// Phase 10.3 Batch 1: Movement & Positioning

bool vClimbFoo() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::CLIMBBIT)) {
    print("Climbing the ");
    print(g.prso->getDesc());
    printLine(" doesn't get you anywhere.");
    return RTRUE;
  }
  printLine("You can't climb that.");
  return RTRUE;
}

bool vThrough() {
  auto &g = Globals::instance();
  if (g.prso) {
    if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
      printLine("You should use 'BOARD' to enter a vehicle.");
      return RTRUE;
    }
    print("You hit your head against the ");
    print(g.prso->getDesc());
    printLine(" as you attempt this feat.");
    return RTRUE;
  }
  return RTRUE;
}

bool vStand() {
  printLine("You are already standing.");
  return RTRUE;
}

bool vAlarm() {
  print("The ");
  if (Globals::instance().prso)
    print(Globals::instance().prso->getDesc());
  printLine(" isn't sleeping.");
  return RTRUE;
}



bool vLaunch() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    printLine("You can't launch that.");
  } else {
    printLine("That's not a vehicle!");
  }
  return RTRUE;
}

// Phase 10.3 Batch 2: Manipulation

bool vCut() {
  printLine("It doesn't seem to work.");
  return RTRUE;
}

bool vLower() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->performAction())
    return RTRUE;
  printLine("You can't lower that.");
  return RTRUE;
}

bool vRaise() {
  auto &g = Globals::instance();
  if (g.prso && g.prso->performAction())
    return RTRUE;
  printLine("You can't raise that.");
  return RTRUE;
}

bool vWind() {
  auto &g = Globals::instance();

  // Check if object is specified
  if (!g.prso) {
    printLine("Wind what?");
    return RTRUE;
  }

  // Call object action handler first
  if (g.prso->performAction()) {
    return RTRUE;
  }

  // Default WIND behavior
  // ZIL: <TELL "You cannot wind up a " D ,PRSO "." CR>
  printLine("You cannot wind up a " + g.prso->getDesc() + ".");
  return RTRUE;
}

bool vMake() {
  printLine("You can't do that.");
  return RTRUE;
}

bool vMelt() {
  printLine("It's not clear that needs melting.");
  return RTRUE;
}

bool vPlay() {
  printLine("That's silly!");
  return RTRUE;
}

bool vPlug() {
  printLine("This has no effect.");
  return RTRUE;
}

bool vPourOn() {
  printLine("You can't pour that on anything.");
  return RTRUE;
}

bool vPushTo() {
  printLine("You can't push things to that.");
  return RTRUE;
}



bool vShake() {
  printLine("Shaken.");
  return RTRUE;
}

bool vSpin() {
  printLine("You can't spin that.");
  return RTRUE;
}

bool vSqueeze() {
  printLine("You can't squeeze that.");
  return RTRUE;
}

bool vTieUp() {
  printLine("You can't tie that up.");
  return RTRUE;
}

// Phase 10.3 Batch 3: Interactions

bool vAnswer() {
  printLine("There is no one waiting for an answer.");
  return RTRUE;
}

bool vReply() {
  printLine("It is hardly likely that the ");
  if (Globals::instance().prso)
    print(Globals::instance().prso->getDesc());
  printLine(" is interested.");
  return RTRUE;
}

bool vCommand() {
  if (Globals::instance().prso) {
    print("The ");
    print(Globals::instance().prso->getDesc());
    printLine(" pays no attention.");
  } else {
    printLine("Command whom?");
  }
  return RTRUE;
}

bool vEcho() {
  printLine("Echo... echo... echo...");
  return RTRUE;
}

bool vFollow() {
  printLine("You're not following anything.");
  return RTRUE;
}

bool vKiss() {
  printLine("I'd sooner kiss a pig.");
  return RTRUE;
}

bool vMumble() {
  printLine("You'll have to speak up if you expect me to hear you!");
  return RTRUE;
}

bool vRepent() {
  printLine("It could well be too late!");
  return RTRUE;
}

bool vSend() {
  printLine("Why would you send for that?");
  return RTRUE;
}

bool vWish() {
  printLine("With luck, your wish will come true.");
  return RTRUE;
}

bool vSpray() {
  printLine("You can't spray that.");
  return RTRUE;
}

// Phase 10.3 Batch 4: Magic/Misc

bool vBlast() {
  printLine("Start small, why don't you?");
  return RTRUE;
}

bool vBurn() {
  printLine("You can't burn that.");
  return RTRUE;
}

bool vChant() {
  printLine("Chanting won't help you.");
  return RTRUE;
}

bool vDisenchant() {
  printLine("You can't disenchant that.");
  return RTRUE;
}

bool vEnchant() {
  printLine("You're not a wizard.");
  return RTRUE;
}

bool vIncant() {
  printLine("The incantation fails.");
  return RTRUE;
}

bool vWin() {
  printLine("Nature, red in tooth and claw...");
  return RTRUE;
}

bool vTreasure() {
  printLine("This is a treasure/adventure.");
  return RTRUE;
}

bool vStay() {
  printLine("You will be captured.");
  return RTRUE;
}

// Phase 10.3 Batch 5: Cleanup & Edge Cases

bool vBrush() {
  printLine("You can't brush that.");
  return RTRUE;
}

bool vBug() {
  printLine(
      "If you find a bug, please start an issue on the tracker."); // Modernized
  return RTRUE;
}

bool vChomp() {
  printLine("Preposterous!");
  // ZIL: "I don't know how to do that. I win in all cases."
  return RTRUE;
}

bool vCount() {
  printLine("You have lost your mind.");
  return RTRUE;
}

bool vCross() {
  printLine("You can't cross that.");
  return RTRUE;
}

bool vHatch() {
  printLine("I don't think that can be hatched.");
  // ZIL: "Bizarre!"
  return RTRUE;
}

bool vKnock() {
  printLine("Why knock on that?");
  return RTRUE;
}

bool vLeave() {
  printLine("Leave what?");
  return RTRUE;
}

bool vLeanOn() {
  printLine("That's not a good idea.");
  return RTRUE;
}

bool vPump() {
  printLine("It's not a pump.");
  return RTRUE;
}

bool vStrike() {
  printLine("You have to be more specific!"); // Intransitive response
  return RTRUE;
}

bool vReadPage() {
  printLine("You can't read that.");
  return RTRUE;
}

bool vPick() {
  printLine("Pick what?");
  return RTRUE;
}

bool vApply() {
  printLine("Apply what?");
  return RTRUE;
}

// Phase 12 Batch 2: Missing Verbs

bool vOil() {
  printLine("You probably don't have anything to oil that with.");
  return RTRUE;
}

bool vStab() {
  // Alias to Attack
  return Verbs::vAttack();
}

bool vDrinkFrom() {
  printLine("You can't drink from that.");
  return RTRUE;
}

bool vLookUnder() {
  printLine("There is nothing but dust there.");
  return RTRUE;
}

bool vLookBehind() {
  printLine("There is nothing behind it.");
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

bool vThrowOff() {
  // "Throw object off object" -> Throw
  return Verbs::vThrow();
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

// ZIL: <ROUTINE V-SKIP () ...> (gverbs.zil:1269-1277)
bool vSkip() {
  static const std::vector<std::string> wheeeee = {
    "Very good. Now you can go to the second grade.",
    "Are you enjoying yourself?",
    "Wheeeeeeeeee!!!!!"
  };
  printLine(wheeeee[GMacros::random(static_cast<int>(wheeeee.size())) - 1]);
  return true;
}

// ZIL: <ROUTINE V-SSPRAY () ...> (gverbs.zil:1294-1296)
bool vSspray() {
  auto &g = Globals::instance();
  std::swap(g.prso, g.prsi);
  return vSpray();
}

// ZIL: <ROUTINE V-COMMAND-FILE () <DIRIN 1> <RTRUE>> (gverbs.zil:130-132)
// The interpreter's command-file input stream has no equivalent here.
bool vCommandFile() {
  return RTRUE;
}

// ============================================================================
// ZIL: GVERBS.ZIL Preaction Routines (zil/gverbs.zil)
// ============================================================================

// ZIL: <ROUTINE PRE-BOARD ("AUX" AV) ...> (gverbs.zil:201-223)
bool preBoard() {
  auto &g = Globals::instance();
  if (!g.prso) return false;
  ZObject *av = g.winner ? g.winner->getLocation() : nullptr;
  if (g.prso->hasFlag(ObjectFlag::VEHBIT)) {
    if (g.prso->getLocation() != g.here) {
      printLine(std::format("The {} must be on the ground to be boarded.", g.prso->getDesc()));
      return true;
    } else if (av && av->hasFlag(ObjectFlag::VEHBIT)) {
      printLine(std::format("You are already in the {}!", av->getDesc()));
      return true;
    }
    return false;
  }
  if (g.prso->getId() == ObjectIds::GLOBAL_WATER) {
    vSwim();
    return true;
  }
  printLine(std::format("You have a theory on how to board a {}, perhaps?", g.prso->getDesc()));
  return true;
}

// ZIL: <ROUTINE PRE-BURN () ...> (gverbs.zil:243-250)
bool preBurn() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    tellNoPrsi();
    return true;
  }
  if (g.prsi->hasFlag(ObjectFlag::ONBIT) || g.prsi->hasFlag(ObjectFlag::FLAMEBIT)) {
    return false;
  }
  printLine(std::format("With a {}?!?", g.prsi->getDesc()));
  return true;
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

// ZIL: <ROUTINE PRE-FILL ("AUX" TX) ...> (gverbs.zil:646-654)
bool preFill() {
  auto &g = Globals::instance();
  if (!g.prsi) {
    ZObject *water = g.getObject(ObjectIds::GLOBAL_WATER);
    if (water && globalIn(ObjectIds::GLOBAL_WATER, g.here)) {
      g.prsi = water;
      printLine(std::format("(with {})", water->getDesc()));
      return false;
    }
    printLine("There is nothing to fill it with.");
    return true;
  }
  return false;
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

// ZIL: <ROUTINE PRE-MOVE () ...> (gverbs.zil:910-922)
bool preMove() {
  auto &g = Globals::instance();
  if (g.prso && isHeld(g.prso)) {
    printLine("You aren't an accomplished enough juggler.");
    return true;
  }
  return false;
}

// ZIL: <ROUTINE PRE-MUNG () ...> (gverbs.zil:923-937)
bool preMung() {
  auto &g = Globals::instance();
  if (!g.prsi || !g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {
    if (g.prso) {
      if (!g.prsi) {
        printLine(std::format("Trying to destroy the {} with your bare hands is futile.", g.prso->getDesc()));
      } else {
        printLine(std::format("Trying to destroy the {} with a {} is futile.", g.prso->getDesc(), g.prsi->getDesc()));
      }
    } else {
      printLine("Trying to destroy things with your bare hands is futile.");
    }
    return true;
  }
  return false;
}

// ZIL: <ROUTINE PRE-PUT () ...> (gverbs.zil:1075-1081)
bool prePut() {
  // ZIL: the Zork I branch of PRE-PUT is just PRE-GIVE, so PUT of something
  // the player is not holding gets "That's easy for you to say...".
  return preGive();
}

// ZIL: <ROUTINE PRE-READ () ...> (gverbs.zil:1137-1144)
bool preRead() {
  auto &g = Globals::instance();
  if (!g.lit) {
    printLine("It is impossible to read in the dark.");
    return true;
  }
  if (g.prsi && !g.prsi->hasFlag(ObjectFlag::TRANSBIT)) {
    printLine(std::format("How does one look through a {}?", g.prsi->getDesc()));
    return true;
  }
  return false;
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

// ZIL: <ROUTINE PRE-TURN () ...> (gverbs.zil:1488-1494)
bool preTurn() {
  auto &g = Globals::instance();
  if (!g.prso || !g.prso->hasFlag(ObjectFlag::TURNBIT)) {
    printLine("You can't turn that!");
    return true;
  }
  if (g.prsi && !g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
    printLine("You can't turn it with that!");
    return true;
  }
  return false;
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

// ZIL: <ROUTINE FINISH ("AUX" WRD) ...> (gverbs.zil:33-54)
void finish() {
  vScore();
  printLine("\nWould you like to restart the game from the beginning, restore a saved");
  printLine("game, or end this session of the game? (Type RESTART, RESTORE, or QUIT):");
  print("> ");
  std::string choice = readLine();
  if (choice == "restart" || choice == "RESTART") {
    vRestart();
  } else if (choice == "restore" || choice == "RESTORE") {
    vRestore();
  } else if (choice == "quit" || choice == "QUIT" || choice == "q" || choice == "Q") {
    vQuit();
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


// ZIL: <ROUTINE HACK-HACK (STR) ...> (gverbs.zil:720-725)
void hackHack(std::string_view str) {
  auto &g = Globals::instance();
  if (g.prso) {
    printLine(std::format("{} {}", str, g.prso->getDesc()));
  } else {
    printLine(std::format("{} that.", str));
  }
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

// ZIL: <ROUTINE HIT-SPOT () ...> (gverbs.zil:728-732)
bool hitSpot() {
  auto &g = Globals::instance();
  if (g.prso) {
    printLine(std::format("Fiddling with the {} doesn't seem to help.", g.prso->getDesc()));
  } else {
    printLine("Fiddling with that doesn't seem to help.");
  }
  return true;
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


// ZIL: <ROUTINE OTHER-SIDE (DOOR) ...> (gverbs.zil:1050-1065)
ZObject *otherSide(const ZObject *door) {
  if (!door) return nullptr;
  return door->getLocation();
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

// ZIL: <ROUTINE REMOVE-CAREFULLY (OBJ) ...> (gverbs.zil:1212-1219)
void removeCarefully(ZObject *obj) {
  if (obj && obj->getLocation()) {
    obj->moveTo(nullptr);
  }
}

// ZIL: <ROUTINE SCORE-OBJ (OBJ "AUX" TEMP) ...> (gverbs.zil:1220-1234)
void scoreObj(ZObject *obj) {
  if (!obj) return;
  int val = obj->getProperty(P_VALUE);
  if (val > 0) {
    scoreUpd(val);
    obj->setProperty(P_VALUE, 0);
  }
}

// ZIL: <ROUTINE SCORE-UPD (VAL) ...> (gverbs.zil:1236-1248)
void scoreUpd(int val) {
  auto &g = Globals::instance();
  g.score += val;
  if (g.score >= Globals::SCORE_MAX) {
    g.wonFlag = true;
  }
}

// ZIL: <ROUTINE SEE-INSIDE? (OBJ) ...>
// Source: zil/gverbs.zil:1839-1841
bool seeInside(const ZObject *obj) {
  return obj && !obj->hasFlag(ObjectFlag::INVISIBLE) &&
         (obj->hasFlag(ObjectFlag::TRANSBIT) ||
          obj->hasFlag(ObjectFlag::OPENBIT));
}

// ZIL: <ROUTINE SHAKE-LOOP (OBJ) ...> (gverbs.zil:1260-1280)
bool shakeLoop(ZObject *obj) {
  if (!obj) return false;
  auto &g = Globals::instance();
  if (obj->hasFlag(ObjectFlag::CONTBIT) && obj->hasFlag(ObjectFlag::OPENBIT)) {
    auto contents = obj->getContents();
    for (auto *child : contents) {
      if (child) {
        child->moveTo(g.here);
        printLine(std::format("A {} spills out onto the floor.", child->getDesc()));
      }
    }
    return true;
  }
  return false;
}

// ZIL: <ROUTINE TELL-NO-PRSI () ...> (gverbs.zil:240-242)
void tellNoPrsi() {
  printLine("You must specify what to use.");
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

// ZIL: <ROUTINE YES? () ...> (gverbs.zil:1515-1530)
bool yes() {
  print("> ");
  std::string response = readLine();
  return response == "yes" || response == "y" || response == "YES" || response == "Y";
}

} // namespace Verbs
