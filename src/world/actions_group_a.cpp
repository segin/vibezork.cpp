#include "core/gmacros.h"
#include "core/gglobals.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "systems/death.h"
#include "core/gmain.h"
#include "verbs/verb_tables.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"

// Helpers from actions.cpp (ZIL 1actions.zil:29-44, 488-505)
int openClose(ZObject *obj, std::string_view stropn, std::string_view strcls);
void touchAll(ZObject *obj);
int otvalFrob(ZObject *o = nullptr);

// Helper to get direction from object (e.g. for "walk north")
// Maps direction objects (NORTH_OBJECT etc.) to Direction enum
static Direction getDirection(ZObject *obj) {
  if (!obj)
    return Direction::IN; // Default or invalid
  // This relies on object IDs for directions.
  // If we don't have direction objects, we check synonyms?
  // Simplify: The parser usually resolves direction words to Direction enum.
  // If prso is an OBJECT, it might be "north wall" or "north".
  // Assuming simple mapping if ID matches known direction objects.
  // For now, if we don't have Direction Objects mapped, this helper might be
  // limited. BUT deadFunction uses it for "TIMBER_ROOM and WEST". If "WEST" is
  // an object passed as PRSO? In ZIL: <EQUAL? ,PRSO ,P?WEST>. So PRSO is a
  // DIRECTION object. I'll assume standard naming or return Direction::WEST if
  // name is "west".
  std::string name = obj->getDesc(); // or synonyms
  // Fast path:
  if (name == "north" || name == "n")
    return Direction::NORTH;
  if (name == "south" || name == "s")
    return Direction::SOUTH;
  if (name == "east" || name == "e")
    return Direction::EAST;
  if (name == "west" || name == "w")
    return Direction::WEST;
  if (name == "ne" || name == "northeast")
    return Direction::NE;
  if (name == "nw" || name == "northwest")
    return Direction::NW;
  if (name == "se" || name == "southeast")
    return Direction::SE;
  if (name == "sw" || name == "southwest")
    return Direction::SW;
  if (name == "up" || name == "u")
    return Direction::UP;
  if (name == "down" || name == "d")
    return Direction::DOWN;
  if (name == "in")
    return Direction::IN;
  if (name == "out")
    return Direction::OUT;
  return Direction::IN; // Fallback
}
// Returns true if handled
// Helper for simple "stupid containers" that refuse insertion
// Updated to check containerId against PRSI
static bool stupidContainerAction(ObjectId containerId,
                                  const std::string &contentName) {
  auto &g = Globals::instance();

  if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
    print("The ");
    print(contentName);
    printLine(" are safely inside; there's no need to do that.");
    return true;
  }

  if (g.prsa == V_LOOK_INSIDE || g.prsa == V_EXAMINE) {
    print("There are lots of ");
    print(contentName);
    printLine(" in there.");
    return true;
  }

  // Logic: If putting X into THIS CONTAINER
  // ZIL: <AND <VERB? PUT> <EQUAL? ,PRSI .OBJ>>
  if (g.prsa == V_PUT || g.prsa == V_PUT_ON) {
    if (g.prsi && g.prsi->getId() == containerId) {
      print("Don't be silly. It wouldn't be a ");
      print(g.prsi->getDesc()); // Use object name
      printLine(" anymore.");
      return true;
    }
  }

  return false;
}

bool bagOfCoinsAction() {
  return stupidContainerAction(ObjectIds::BAG_OF_COINS, "coins");
}

bool trunkAction() {
  // FIXME: TRUNK object ID not defined yet. Commented out to fix compilation.
  // return stupidContainerAction(ObjectIds::TRUNK, "jewels");
  return false;
}

// WEAPON-FUNCTION helper (shared by AXE-F and STILETTO-FUNCTION)
// ZIL: Prevents taking weapon if NPC wielder is present
// W = weapon object, V = villain/NPC wielding it
static bool weaponFunction(ZObject *weapon, ZObject *villain) {
  auto &g = Globals::instance();

  // If villain is not in current room, allow normal handling
  if (!villain || villain->getLocation() != g.here) {
    return false;
  }

  // Only applies to TAKE verb
  if (g.prsa != V_TAKE) {
    return false;
  }

  // Check if weapon is in villain's possession
  if (weapon->getLocation() == villain) {
    // Villain swings weapon out of reach
    print("The ");
    print(villain->getDesc());
    printLine(" swings it out of your reach.");
    return true;
  } else {
    // Weapon is white-hot (magical curse)
    print("The ");
    print(weapon->getDesc());
    printLine(" seems white-hot. You can't hold on to it.");
    return true;
  }
}

// AXE-F - Axe interaction with Troll
// ZIL: <COND (,TROLL-FLAG <>) (T <WEAPON-FUNCTION ,AXE ,TROLL>)>
bool axeAction() {
  auto &g = Globals::instance();

  // Check TROLL-FLAG - if true, troll is dead, allow normal handling
  // TROLL-FLAG is stored as a property on the troll or as global state
  ZObject *troll = g.getObject(ObjectIds::TROLL);

  // If troll is dead (not in game), allow normal behavior
  if (!troll || troll->getLocation() == nullptr) {
    return false;
  }

  // If troll has NDESCBIT set, it's dead/unconscious
  if (troll->hasFlag(ObjectFlag::NDESCBIT)) {
    return false;
  }

  // Call weapon function
  return weaponFunction(g.prso, troll);
}

// GRUE-FUNCTION - (gglobals.zil:191-206)
bool grueAction() {
  return GGlobals::grueFunction();
}

// BARROW-DOOR-FCN - Barrow door is too heavy to open/close
// ZIL: OPEN/CLOSE prints "The door is too heavy."
// Source: 1actions.zil lines 432-434
bool barrowDoorAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
    printLine("The door is too heavy.");
    return true;
  }
  return false;
}

// BARROW-FCN - Stone barrow entrance, THROUGH goes west
// ZIL: <COND (<VERB? THROUGH> <DO-WALK ,P?WEST>)>
// Source: 1actions.zil lines 436-438
bool barrowAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_THROUGH || g.prsa == V_ENTER) {
    Verbs::vWalkDir(Direction::WEST);
    return true;
  }
  return false;
}

// CELLAR-FCN (Room action for Cellar)
// CELLAR-FCN - Cellar room handler
// ZIL: M-LOOK prints desc. M-ENTER slams TRAP-DOOR if open/untouched.
// Source: 1actions.zil lines 531-543
int cellarAction(int rarg) {
  auto &g = Globals::instance();

  if (rarg == M_LOOK) {
    printLine("You are in a dark and damp cellar with a narrow passageway "
              "leading north, and a crawlway to the south. On the west is the "
              "bottom of a steep metal ramp which is unclimbable.");
  } else if (rarg == M_ENTER) {
    ZObject *trapdoor = g.getObject(ObjectIds::TRAP_DOOR);
    if (trapdoor && trapdoor->hasFlag(ObjectFlag::OPENBIT) &&
        !trapdoor->hasFlag(ObjectFlag::TOUCHBIT)) {
      trapdoor->clearFlag(ObjectFlag::OPENBIT);
      trapdoor->setFlag(ObjectFlag::TOUCHBIT);
      // ZIL ends this TELL with CR CR (1actions.zil:544)
      printLine("The trap door crashes shut, and you hear someone barring it.");
      crlf();
    }
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

// ZIL: <ROUTINE CHIMNEY-F ()
//        <COND (<VERB? EXAMINE>
//               <TELL "The chimney leads ">
//               <COND (<==? ,HERE ,KITCHEN> <TELL "down">) (T <TELL "up">)>
//               <TELL "ward, and looks climbable." CR>)>>
// Source: zil/1actions.zil:547-553
//
// EXAMINE is the whole routine.  Climbing the chimney is the rooms' business:
// the Kitchen's DOWN exit and the Living Room's UP exit run
// UP-CHIMNEY-FUNCTION, so the invented CLIMB branches here are gone.
int chimneyAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_EXAMINE) {
    tell("The chimney leads ");
    tell(g.here && g.here->getId() == RoomIds::KITCHEN ? "down" : "up");
    tell("ward, and looks climbable.", CR);
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

// CLEARING-FCN (Room action for Clearing)
// CLEARING-FCN - Clearing room handler
// ZIL: M-ENTER hides grate if not revealed. M-LOOK prints desc + grate status.
// Source: 1actions.zil lines 815-831
int clearingAction(int rarg) {
  auto &g = Globals::instance();
  ZObject *grate = g.getObject(ObjectIds::GRATE);

  if (rarg == M_ENTER) {
    if (!g.grateRevealed && grate) {
      grate->setFlag(ObjectFlag::INVISIBLE);
    }
    // Note: Logic for revealing it usually happens via LEAF-PILE or unlocking
    // from outside?
  } else if (rarg == M_LOOK) {
    printLine("You are in a clearing, with a forest surrounding you on all "
              "sides. A path leads south.");

    if (grate) {
      if (grate->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("There is an open grating, descending into darkness.");
      } else if (g.grateRevealed) {
        printLine("There is a grating securely fastened into the ground.");
      }
    }
  }
  return M_NOT_HANDLED;
}

// CRACK-FCN
// CRACK-FCN
// ZIL: THROUGH -> "You can't fit".
// Source: 1actions.zil lines 381-383
bool crackAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_THROUGH) { // Assuming V_THROUGH exists
    printLine("You can't fit through the crack.");
    return RTRUE;
  }

  // Original C++ had LOOK_INSIDE/EXAMINE logic. ZIL does not.
  // We should let it fall through or keep if useful?
  // ZIL fidelity suggests removing custom EXAMINE unless it matches LDESC.
  // LDESC: "There's a narrow crack in the rock."
  // Original C++: "The crack is too small for you to see anything."
  // Detailed analysis: LOOK INSIDE != EXAMINE.
  // I will KEEP LOOK_INSIDE as "Too small" but REMOVE EXAMINE interception.
  if (g.prsa == V_LOOK_INSIDE) {
    printLine("The crack is too small for you to see anything.");
    return RTRUE;
  }

  return RFALSE;
}

// CRETIN-FCN (handles "me", "self", "cretin")
// CRETIN-FCN (handles "me", "self", "cretin") - (gglobals.zil:221-265)
bool cretinAction() {
  return GGlobals::cretinFcn();
}

// DAM-ROOM-FCN (Room action for Dam)
// DAM-ROOM-FCN (Room action for Dam)
// ZIL: Handles M-LOOK logic based on water level and gates status.
// Source: 1actions.zil lines 1156-1185
int damRoomAction(int rarg) {
  auto &g = Globals::instance();

  if (rarg == M_LOOK) {
    printLine("You are standing on the top of the Flood Control Dam #3, which "
              "was quite a tourist attraction in times far distant. There are "
              "paths to the north, south, and west, and a scramble down.");

    if (g.lowTide && g.gatesOpen) {
      printLine("The water level behind the dam is low: The sluice gates have "
                "been opened. Water rushes through the dam and downstream.");
    } else if (g.gatesOpen) {
      printLine("The sluice gates are open, and water rushes through the dam. "
                "The water level behind the dam is still high.");
    } else if (g.lowTide) {
      printLine("The sluice gates are closed. The water level in the reservoir "
                "is quite low, but the level is rising quickly.");
    } else {
      printLine("The sluice gates on the dam are closed. Behind the dam, there "
                "can be seen a wide reservoir. Water is pouring over the top "
                "of the now abandoned dam.");
    }

    print("There is a control panel here, on which a large metal bolt is "
          "mounted. Directly above the bolt is a small green plastic bubble");
    if (g.gateFlag) {
      print(" which is glowing serenely");
    }
    printLine(".");
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

// DBOAT-FUNCTION (Deflated boat)
// IBOAT-FUNCTION (Inflatable Boat / Pile of Plastic)
// ZIL: Handles INFLATE with PUMP (Success) or LUNGS (Fail). Must be on ground.
// Source: 1actions.zil lines 2820-2840
bool inflatableBoatAction() {
  auto &g = Globals::instance();
  ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATABLE);

  if (g.prsa == V_INFLATE || g.prsa == V_FILL) {
    // Validation: Boat must be on ground (not held)
    if (boat && boat->getLocation() !=
                    g.here) { // Assuming global 'here' is current room
      // Check if held? If held by player, loc == winner.
      // ZIL: <NOT <IN? ,INFLATABLE-BOAT ,HERE>> -> "Must be on ground".
      // If player holds it, it is IN PLAYER, not IN HERE.
      printLine("The boat must be on the ground to be inflated.");
      return true;
    }

    if (g.prsi && g.prsi->getId() == ObjectIds::PUMP) {
      printLine("The boat inflates and appears seaworthy.");
      // TODO: Boat Label Logic (lines 2828-2830)

      // Transform: Remove Inflatable, Add Inflated
      // Need access to Inflated Boat (600)
      if (boat) {
        ZObject *loc = boat->getLocation(); // Should be HERE
        boat->moveTo(nullptr);              // Remove logic

        ZObject *inflated = g.getObject(ObjectIds::BOAT_INFLATED);
        if (inflated) {
          inflated->moveTo(loc);
        } else {
          // Create if missing? world_init should create it.
          // Assuming it exists but is invisible/nowhere.
        }
      }
      return true;
    } else if (g.prsi && g.prsi->getId() == ObjectIds::LUNGS) {
      printLine("You don't have enough lung power to inflate it.");
      return true;
    } else if (g.prsi) {
      printLine("With a " + g.prsi->getDesc() + "? Surely you jest!");
      return true;
    }
  }
  return false;
}

// DBOAT-FUNCTION (Punctured Boat)
// ZIL: INFLATE -> Fail. PLUG/PUT with PUTTY -> Repaired (becomes Inflatable).
// Source: 1actions.zil lines 2652-2667
bool puncturedBoatAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_INFLATE || g.prsa == V_FILL) {
    printLine("No chance. Some moron punctured it.");
    return true;
  }

  // Check for PLUG/PUT with PUTTY
  bool isFix = false;
  if (g.prsa == V_PLUG && g.prsi && g.prsi->getId() == ObjectIds::PUTTY)
    isFix = true;
  if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && g.prso &&
      g.prso->getId() == ObjectIds::PUTTY)
    isFix = true; // PUT PUTTY (ON BOAT)

  if (isFix) {
    // FIX-BOAT Logic
    printLine("Well done. The boat is repaired.");

    ZObject *punctured = g.getObject(ObjectIds::BOAT_PUNCTURED);
    if (punctured) {
      ZObject *loc = punctured->getLocation();
      punctured->moveTo(nullptr); // Remove logic

      // Move Inflatable Boat to here
      ZObject *inflatable = g.getObject(ObjectIds::BOAT_INFLATABLE);
      if (inflatable) {
        inflatable->moveTo(loc);
      }
    }
    return true;
  } else if (g.prsa == V_PLUG && g.prsi) {
    printLine("With a " + g.prsi->getDesc() + "?");
    return true;
  }

  return false;
}

// ZIL: <ROUTINE DEAD-FUNCTION ("OPTIONAL" (FOO <>) "AUX" M) ...>
// Source: zil/1actions.zil:3113-3172
//
// The player's own ACTION while dead. Note the ZIL clause order: RUB is
// listed both with the "beyond your capabilities" group and with TAKE, and
// the first COND clause wins, so RUB never reaches the TAKE line.
bool deadFunction() {
  auto &g = Globals::instance();

  if (g.prsa == V_WALK) {
    if (g.here && g.here->getId() == RoomIds::TIMBER_ROOM &&
        getDirection(g.prso) == Direction::WEST) {
      tell("You cannot enter in your condition.", CR);
      return true;
    }
    return false;
  }

  // ZIL: these fall through to the normal handlers.
  if (g.prsa == V_BRIEF || g.prsa == V_VERBOSE || g.prsa == V_SUPERBRIEF ||
      g.prsa == V_VERSION || g.prsa == V_SAVE || g.prsa == V_RESTORE ||
      g.prsa == V_QUIT || g.prsa == V_RESTART) {
    return false;
  }

  if (g.prsa == V_ATTACK || g.prsa == V_MUNG || g.prsa == V_ALARM ||
      g.prsa == V_SWING) {
    tell("All such attacks are vain in your condition.", CR);
    return true;
  }

  if (g.prsa == V_OPEN || g.prsa == V_CLOSE || g.prsa == V_EAT ||
      g.prsa == V_DRINK || g.prsa == V_INFLATE || g.prsa == V_DEFLATE ||
      g.prsa == V_TURN || g.prsa == V_BURN || g.prsa == V_TIE ||
      g.prsa == V_UNTIE || g.prsa == V_RUB) {
    tell("Even such an action is beyond your capabilities.", CR);
    return true;
  }

  if (g.prsa == V_WAIT) {
    tell("Might as well. You've got an eternity.", CR);
    return true;
  }
  if (g.prsa == V_LAMP_ON) {
    tell("You need no light to guide you.", CR);
    return true;
  }
  if (g.prsa == V_SCORE) {
    tell("You're dead! How can you think of your score?", CR);
    return true;
  }
  if (g.prsa == V_TAKE) {
    tell("Your hand passes through its object.", CR);
    return true;
  }
  if (g.prsa == V_DROP || g.prsa == V_THROW || g.prsa == V_INVENTORY) {
    tell("You have no possessions.", CR);
    return true;
  }
  if (g.prsa == V_DIAGNOSE) {
    tell("You are dead.", CR);
    return true;
  }

  if (g.prsa == V_LOOK) {
    tell("The room looks strange and unearthly");
    if (g.here && g.here->getContents().empty()) {
      tell(".");
    } else {
      tell(" and objects appear indistinct.");
    }
    crlf();
    if (g.here && !g.here->hasFlag(ObjectFlag::ONBIT)) {
      tell("Although there is no light, the room seems dimly illuminated.", CR);
    }
    crlf();
    return false; // ZIL returns <> so the room description still runs
  }

  if (g.prsa == V_PRAY) {
    if (g.here == g.getObject(RoomIds::SOUTH_TEMPLE)) {
      if (auto *lamp = g.getObject(ObjectIds::LAMP)) {
        lamp->clearFlag(ObjectFlag::INVISIBLE);
      }
      if (g.winner) g.winner->setAction(nullptr);
      DeathSystem::setAlwaysLit(false);
      DeathSystem::setDead(false);
      ZObject *troll = g.getObject(ObjectIds::TROLL);
      if (troll && troll->getLocation() == g.getObject(RoomIds::TROLL_ROOM)) {
        g.trollFlag = false;
      }
      tell("From the distance the sound of a lone trumpet is heard. The room "
           "becomes very bright and you feel disembodied. In a moment, the "
           "brightness fades and you find yourself rising as if from a long "
           "sleep, deep in the woods. In the distance you can faintly hear a "
           "songbird and the sounds of the forest.",
           CR, CR);
      Verbs::goTo(g.getObject(RoomIds::FOREST_1));
      return true;
    }
    tell("Your prayers are not heard.", CR);
    return true;
  }

  // ZIL: anything else at all
  tell("You can't even do that.", CR);
  g.pCont = 0;
  return true; // caller maps this to RFATAL below
}

// DEEP-CANYON-F (Room Action)
// ZIL: M-LOOK with water sound logic.
// Source: 1actions.zil lines 1730-1745
int deepCanyonRoomAction(int rarg) {
  auto &g = Globals::instance();

  if (rarg == M_LOOK) {
    printLine("You are on the south edge of a deep canyon. Passages lead off "
              "to the east, northwest and southwest. A stairway leads down.");

    if (g.gatesOpen && !g.lowTide) {
      printLine(" You can hear a loud roaring sound, like that of rushing "
                "water, from below.");
    } else if (!g.gatesOpen && g.lowTide) {
      // Nothing (CRLF only in ZIL)
    } else {
      printLine(" You can hear the sound of flowing water from below.");
    }
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

// DOME-ROOM-FCN (Room action)
// DOME-ROOM-FCN (Room action)
// ZIL: M-LOOK with rope (DOME-FLAG). M-ENTER with Spirit/Leap logic.
// Source: 1actions.zil lines 1030-1050
int domeRoomAction(int rarg) {
  auto &g = Globals::instance();

  if (rarg == M_LOOK) {
    printLine("You are at the periphery of a large dome, which forms the "
              "ceiling of another room below. Protecting you from a "
              "precipitous drop is a wooden railing which circles the dome.");
    if (g.domeFlag) {
      printLine("Hanging down from the railing is a rope which ends about ten "
                "feet from the floor below.");
    }
  } else if (rarg == M_ENTER) {
    if (DeathSystem::isDead()) {
      printLine("As you enter the dome you feel a strong pull as if from a "
                "wind drawing you over the railing and down.");
      ZObject *torchRoom = g.getObject(RoomIds::TORCH_ROOM);
      if (torchRoom && g.winner) {
        g.winner->moveTo(torchRoom);
        g.here = torchRoom;
      }
      // RTRUE implies handled/stop?
      return M_HANDLED;
    }

    if (g.prsa == V_LEAP) {
      DeathSystem::jigsUp(
          "I'm afraid that the leap you attempted has done you in.");
    }
  }
  return M_NOT_HANDLED;
}

// FRONT-DOOR-FCN
// ZIL: OPEN, BURN, MUNG, LOOK-BEHIND.
// Source: 1actions.zil lines 2163-2172
bool frontDoorAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_OPEN) {
    printLine("The door cannot be opened.");
    return true;
  }

  if (g.prsa == V_BURN) {
    printLine("You cannot burn this door.");
    return true;
  }

  if (g.prsa == V_MUNG) {
    printLine("You can't seem to damage the door.");
    return true;
  }

  if (g.prsa == V_LOOK_BEHIND) {
    printLine("It won't open.");
    return true;
  }

  return false;
}

// GARLIC-F
// GARLIC-F
// ZIL: EAT.
// Source: 1actions.zil lines 4160-4165
bool garlicAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_EAT) {
    printLine("What the heck! You won't make friends this way, but nobody "
              "around here is too friendly anyhow. Gulp!");
    if (g.prso)
      g.prso->moveTo(nullptr); // Consumed
    return true;
  }
  return false;
}

// GHOSTS-F
// GHOSTS-F
// ZIL: TELL, EXORCISE, ATTACK/MUNG, Catch-All.
// Source: 1actions.zil lines 264-273
bool ghostsAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_TELL) {
    printLine("The spirits jeer loudly and ignore you.");
    g.pCont = 0; // ZIL: <SETG P-CONT <>> (1actions.zil:266)
    return true;
  }

  // ZIL uses EXORCISE. Verifying V_EXORCISE exists.
  // If grep fails, I might map V_COMMAND?
  // Start with V_EXORCISE or V_SAY?
  // I'll check if V_EXORCISE is valid in next step.
  // Assuming V_EXORCISE is valid based on previous actions using it/checking
  // it.
  if (g.prsa == V_EXORCISE) {
    printLine("Only the ceremony itself has any effect.");
    return true;
  }

  // ZIL: <AND <VERB? ATTACK MUNG> <EQUAL? ,PRSO ,GHOSTS>>
  if (g.prsa == V_ATTACK || g.prsa == V_MUNG) {
    if (g.prso && g.prso->getId() == ObjectIds::GHOSTS) {
      printLine("How can you attack a spirit with material objects?");
      return true;
    }
  }

  // Catch-All (ZIL handles T with "Unable to interact")
  printLine("You seem unable to interact with these spirits.");
  return true;
}

// GRANITE-WALL-F
// GRANITE-WALL-F
// ZIL: Room-specific interactions (North Temple, Treasure Room, Slide Room).
// ZIL: <ROUTINE GRANITE-WALL-F () ...>
// Source: zil/1actions.zil:95-111
//
// Three rooms answer for the wall and everywhere else denies it. Only FIND,
// and TAKE/RAISE/LOWER, are handled; the Slide Room adds READ.
int graniteWallAction() {
  auto &g = Globals::instance();
  const ObjectId room = g.here ? g.here->getId() : 0;

  const bool find = g.prsa == V_FIND;
  const bool grab =
      g.prsa == V_TAKE || g.prsa == V_RAISE || g.prsa == V_LOWER;

  if (room == RoomIds::NORTH_TEMPLE) {
    if (find) {
      printLine("The west wall is solid granite here.");
      return M_HANDLED;
    }
    if (grab) {
      printLine("It's solid granite.");
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }
  if (room == RoomIds::TREASURE_ROOM) {
    if (find) {
      printLine("The east wall is solid granite here.");
      return M_HANDLED;
    }
    if (grab) {
      printLine("It's solid granite.");
      return M_HANDLED;
    }
    return M_NOT_HANDLED;
  }
  if (room == RoomIds::SLIDE_ROOM) {
    if (find || g.prsa == V_READ)
      printLine("It only SAYS \"Granite Wall\".");
    else
      printLine("The wall isn't granite.");
    return M_HANDLED;
  }
  printLine("There is no granite wall here.");
  return M_HANDLED;
}

// GRATE-FUNCTION
// ZIL: Lock/Unlock interactions with Grate and Keys.
// Source: 1actions.zil lines 850-870
bool grateAction() {
  auto &g = Globals::instance();
  auto room = g.here->getId();

  // Helper: Is PRSI Keys?
  bool withKeys = (g.prsi && g.prsi->getId() == ObjectIds::KEYS);

  // OPEN with KEYS -> Delegate to UNLOCK
  if (g.prsa == V_OPEN && withKeys) {
    // Fall through to UNLOCK logic?
    // Or explicitly set PRSA to UNLOCK?
    // ZIL says <PERFORM ,V?UNLOCK ...>
    // We can just execute UNLOCK block below.
    g.prsa = V_UNLOCK;
  }

  if (g.prsa == V_LOCK) {
    if (room == RoomIds::GRATING_ROOM) {
      g.grunlock = false;
      printLine("The grate is locked.");
      // ZIL: <SETG GRUNLOCK <>> (1actions.zil:876); no lock flag exists.
      return true;
    }
    if (room == RoomIds::CLEARING) { // ZIL: GRATING-CLEARING
      printLine("You can't lock it from this side.");
      return true;
    }
  }

  if (g.prsa == V_UNLOCK) {
    if (g.prso && g.prso->getId() == ObjectIds::GRATE) {
      if (room == RoomIds::GRATING_ROOM && withKeys) {
        g.grunlock = true; // Unlocked
        printLine("The grate is unlocked.");
        return true;
      }
      if (room == RoomIds::CLEARING && withKeys) {
        printLine("You can't reach the lock from here.");
        return true;
      }
      // Default Unlock failure
      if (g.prsi) {
        printLine("Can you unlock a grating with a " + g.prsi->getDesc() + "?");
      } else {
        printLine("Unlock it with what?");
      }
      return true;
    }
  }

  return false;
}

// GUNK-FUNCTION
// ZIL: Unconditional interaction causes crumbling.
// Source: 1actions.zil lines 2553-2556
bool gunkAction() {
  auto &g = Globals::instance();
  // ZIL: <REMOVE-CAREFULLY ,GUNK> <TELL ...>
  // No verb check implies any interaction triggers this.
  // Logic: If action called, it crumbles.
  printLine("The slag was rather insubstantial, and crumbles into dust at your "
            "touch.");
  if (g.prso)
    g.prso->moveTo(nullptr); // Consumed/Removed
  return true;
}

// HOT-BELL-F
// HOT-BELL-F
// ZIL: Take, Ring, Rub/Ring-With (Burn/Heat), Pour-On (Cool).
// Source: 1actions.zil lines 351-368
bool hotBellAction() {
  auto &g = Globals::instance();

  // TAKE
  if (g.prsa == V_TAKE) {
    printLine("The bell is very hot and cannot be taken.");
    return true;
  }

  // RING (Simple) - "Too hot to reach" implies manual ringing without tools?
  // ZIL: (<VERB? RING> ... too hot to reach) comes AFTER the RING+PRSI check.
  // So we check RING+PRSI first.

  // RUB or RING+PRSI
  if (g.prsa == V_RUB || (g.prsa == V_RING && g.prsi)) {
    ZObject *tool = g.prsi;
    if (tool) {
      if (tool->hasFlag(ObjectFlag::BURNBIT)) {
        printLine("The " + tool->getDesc() + " burns and is consumed.");
        tool->moveTo(nullptr); // Consumed
        return true;
      }
      if (tool->getId() == ObjectIds::HANDS) {
        printLine("The bell is too hot to touch.");
        return true;
      }
    }
    printLine("The heat from the bell is too intense.");
    return true;
  }

  // POUR-ON (Cooling)
  if (g.prsa == V_POUR_ON) {
    // Logic assumes PRSO is Water, PRSI is Bell (context).
    // ZIL: Removes PRSO (Water).
    if (g.prso)
      g.prso->moveTo(nullptr);

    printLine("The water cools the bell and is evaporated.");

    // Transformation: Swap HOT_BELL for BELL
    ZObject *hotBell = g.getObject(ObjectIds::HOT_BELL); // Assuming this object
    ZObject *regularBell = g.getObject(ObjectIds::BELL);

    if (hotBell && regularBell) {
      // Swap location
      ZObject *loc = hotBell->getLocation();
      hotBell->moveTo(nullptr);
      regularBell->moveTo(loc);
    }
    return true;
  }

  // RING (Fallthrough for simple Ring)
  if (g.prsa == V_RING) {
    printLine("The bell is too hot to reach.");
    return true;
  }

  return false;
}

// IBOAT-FUNCTION
// ZIL: Inflation logic (Pump vs Lungs).
// Source: 1actions.zil lines 2820-2840
bool iboatFunction() {
  auto &g = Globals::instance();

  // INFLATE / FILL
  if (g.prsa == V_INFLATE ||
      g.prsa == V_FILL) { // FILL maps to INFLATE context often
    // Check Location (Must be on ground)
    if (g.prso && g.prso->getLocation() != g.here) {
      printLine("The boat must be on the ground to be inflated.");
      return true;
    }

    // Check Tool (PRSI)
    if (g.prsi && g.prsi->getId() == ObjectIds::PUMP) {
      printLine("The boat inflates and appears seaworthy.");
      // Label hint (ZIL checks touchbit, we'll just print it for fidelity/clue)
      printLine("A tan label is lying inside the boat.");

      // Swap: Inflatable -> Inflated
      ZObject *inflatable = g.getObject(ObjectIds::BOAT_INFLATABLE);
      ZObject *inflated = g.getObject(ObjectIds::BOAT_INFLATED);

      if (inflatable && inflated) {
        inflatable->moveTo(nullptr);
        inflated->moveTo(g.here);
        g.it = inflated;
      }
      return true;
    }

    if (g.prsi && g.prsi->getId() == ObjectIds::LUNGS) {
      printLine("You don't have enough lung power to inflate it.");
      return true;
    }

    // Default Tool
    if (g.prsi) {
      printLine("With a " + g.prsi->getDesc() + "? Surely you jest!");
    } else {
      printLine("Inflate it with what?");
    }
    return true;
  }

  return false;
}
// RBOAT-FUNCTION - Inflated boat room function
// ZIL: Room function for INFLATED-BOAT
// Source: 1actions.zil lines 2722-2815
bool inflatedBoatAction() {
  auto &g = Globals::instance();
  ZObject *winner = g.winner ? g.winner : g.player;
  if (!winner) {
    winner = g.getObject(ObjectIds::ADVENTURER);
  }

  // ============================================================================
  // VERB: WALK - Direction validation
  // ============================================================================
  if (g.prsa == V_WALK) {
    // Check if player is in boat (location check)
    if (!winner || !winner->getLocation() ||
        (winner->getLocation()->getId() != ObjectIds::BOAT_INFLATED &&
         winner->getLocation()->getId() != ObjectIds::INFLATED_BOAT)) {
      return RFALSE; // Not in boat, not handled
    }

    Direction dir = getDirection(g.prso);
    if (dir == Direction::EAST || dir == Direction::WEST) {
      return RFALSE;
    }
    if (g.prso && (g.prso->getId() == ObjectIds::SAND || g.prso->getId() == ObjectIds::GROUND)) {
      return RFALSE;
    }
    if (g.here && g.here->getId() == RoomIds::RESERVOIR &&
        (dir == Direction::NORTH || dir == Direction::SOUTH)) {
      return RFALSE;
    }
    if (g.here && g.here->getId() == RoomIds::IN_STREAM &&
        dir == Direction::SOUTH) {
      return RFALSE;
    }

    printLine("Read the label for the boat's instructions.");
    return RTRUE;
  }

  // ============================================================================
  // VERB: LAUNCH
  // ============================================================================
  if (g.prsa == V_LAUNCH) {
    // Check if player is in boat
    if (!winner || !winner->getLocation() ||
        (winner->getLocation()->getId() != ObjectIds::BOAT_INFLATED &&
         winner->getLocation()->getId() != ObjectIds::INFLATED_BOAT)) {
      printLine("You're not in the boat!");
      return RTRUE;
    }

    // Check if already on water
    if (g.here && (g.here->getId() == RoomIds::RESERVOIR ||
                   g.here->getId() == RoomIds::IN_STREAM ||
                   g.here->getId() == RoomIds::RIVER_1 ||
                   g.here->getId() == RoomIds::RIVER_2 ||
                   g.here->getId() == RoomIds::RIVER_3 ||
                   g.here->getId() == RoomIds::RIVER_4 ||
                   g.here->getId() == RoomIds::RIVER_5)) {
      std::string waterName = "river";
      if (g.here->getId() == RoomIds::RESERVOIR) {
        waterName = "reservoir";
      } else if (g.here->getId() == RoomIds::IN_STREAM) {
        waterName = "stream";
      }
      printLine(std::format("You are on the {}, or have you forgotten?", waterName));
      return RTRUE;
    }

    // TODO: GO-NEXT RIVER-LAUNCH, enable I-RIVER timer
    printLine("You can't launch it here.");
    return RTRUE;
  }

  // ============================================================================
  // VERB: DROP/PUT weapons - Punctures boat
  // ============================================================================
  if ((g.prsa == V_DROP && g.prso && g.prso->hasFlag(ObjectFlag::WEAPONBIT)) ||
      (g.prsa == V_PUT && g.prso && g.prso->hasFlag(ObjectFlag::WEAPONBIT) &&
       g.prsi && g.prsi->getId() == ObjectIds::BOAT_INFLATED)) {

    // Puncture the boat
    ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
    ZObject *puncturedBoat = g.getObject(ObjectIds::BOAT_PUNCTURED);

    if (boat && puncturedBoat && g.here) {
      // Remove inflated boat
      boat->moveTo(nullptr);

      // Add punctured boat
      puncturedBoat->moveTo(g.here);

      // Move player out of boat
      g.winner->moveTo(g.here);

      // Print message
      print("It seems that the ");
      if (g.prsa == V_DROP || g.prsa == V_PUT) {
        print(g.prso->getDesc());
      }
      printLine(" didn't agree with the boat, as evidenced by the loud "
                "hissing noise issuing therefrom. With a pathetic sputter, "
                "the boat deflates, leaving you without.");

      // Check for death condition (RLANDBIT - can't reach land)
      if (g.here->hasFlag(ObjectFlag::RLANDBIT)) {
        crlf();
        if (g.here->getId() == RoomIds::RESERVOIR ||
            g.here->getId() == RoomIds::IN_STREAM) {
          // Drowning death
          printLine("Another pathetic sputter, this time from you, heralds "
                    "your drowning.");
          // TODO: JIGS-UP
        } else {
          // River death
          printLine(
              "In other words, fighting the fierce currents of the Frigid "
              "River. You manage to hold your own for a bit, but then you are "
              "carried over a waterfall and into some nasty rocks. Ouch!");
          // TODO: JIGS-UP
        }
      }

      return RTRUE;
    }
  }

  // ============================================================================
  // VERB: ATTACK/MUNG with weapons - Punctures boat
  // ============================================================================
  if ((g.prsa == V_ATTACK || g.prsa == V_MUNG) && g.prsi &&
      g.prsi->hasFlag(ObjectFlag::WEAPONBIT)) {

    // Same logic as DROP weapons
    ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
    ZObject *puncturedBoat = g.getObject(ObjectIds::BOAT_PUNCTURED);

    if (boat && puncturedBoat && g.here) {
      boat->moveTo(nullptr);
      puncturedBoat->moveTo(g.here);
      g.winner->moveTo(g.here);

      print("It seems that the ");
      print(g.prsi->getDesc());
      printLine(" didn't agree with the boat, as evidenced by the loud "
                "hissing noise issuing therefrom. With a pathetic sputter, "
                "the boat deflates, leaving you without.");

      if (g.here->hasFlag(ObjectFlag::RLANDBIT)) {
        crlf();
        if (g.here->getId() == RoomIds::RESERVOIR ||
            g.here->getId() == RoomIds::IN_STREAM) {
          printLine("Another pathetic sputter, this time from you, heralds "
                    "your drowning.");
        } else {
          printLine(
              "In other words, fighting the fierce currents of the Frigid "
              "River. You manage to hold your own for a bit, but then you are "
              "carried over a waterfall and into some nasty rocks. Ouch!");
        }
      }

      return RTRUE;
    }
  }

  // ============================================================================
  // VERB: BOARD - Check for sharp weapons
  // ============================================================================
  if (g.prsa == V_BOARD) {
    // Check if carrying any sharp weapons
    ZObject *sceptre = g.getObject(ObjectIds::SCEPTRE);
    ZObject *knife = g.getObject(ObjectIds::KNIFE);
    ZObject *sword = g.getObject(ObjectIds::SWORD);
    ZObject *rustyKnife = g.getObject(ObjectIds::RUSTY_KNIFE);
    ZObject *axe = g.getObject(ObjectIds::AXE);
    ZObject *stiletto = g.getObject(ObjectIds::STILETTO);

    bool hasSharpWeapon = false;
    if ((sceptre && sceptre->getLocation() == g.winner) ||
        (knife && knife->getLocation() == g.winner) ||
        (sword && sword->getLocation() == g.winner) ||
        (rustyKnife && rustyKnife->getLocation() == g.winner) ||
        (axe && axe->getLocation() == g.winner) ||
        (stiletto && stiletto->getLocation() == g.winner)) {
      hasSharpWeapon = true;
    }

    if (hasSharpWeapon) {
      printLine("Oops! Something sharp seems to have slipped and punctured the "
                "boat. The boat deflates to the sounds of hissing, "
                "sputtering, and cursing.");

      // Puncture the boat
      ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
      ZObject *puncturedBoat = g.getObject(ObjectIds::BOAT_PUNCTURED);

      if (boat && puncturedBoat && g.here) {
        boat->moveTo(nullptr);
        puncturedBoat->moveTo(g.here);
      }

      return RTRUE;
    }
  }

  // ============================================================================
  // VERB: INFLATE/FILL - Already inflated
  // ============================================================================
  if (g.prsa == V_INFLATE || g.prsa == V_FILL) {
    printLine("Inflating it further would probably burst it.");
    return RTRUE;
  }

  // ============================================================================
  // VERB: DEFLATE
  // ============================================================================
  if (g.prsa == V_DEFLATE) {
    // Check if player is inside boat
    if (winner && winner->getLocation() &&
        (winner->getLocation()->getId() == ObjectIds::BOAT_INFLATED ||
         winner->getLocation()->getId() == ObjectIds::INFLATED_BOAT)) {
      printLine("You can't deflate the boat while you're in it.");
      return RTRUE;
    }

    // Check if boat is on ground (in HERE)
    ZObject *boat = g.getObject(ObjectIds::BOAT_INFLATED);
    if (!boat) {
      boat = g.getObject(ObjectIds::INFLATED_BOAT);
    }
    if (boat && boat->getLocation() != g.here) {
      printLine("The boat must be on the ground to be deflated.");
      return RTRUE;
    }

    // Deflate the boat
    if (boat && g.here) {
      printLine("The boat deflates.");
      g.deflate = true;
      boat->moveTo(nullptr);

      ZObject *deflatableBoat = g.getObject(ObjectIds::BOAT_INFLATABLE);
      if (!deflatableBoat) {
        deflatableBoat = g.getObject(ObjectIds::INFLATABLE_BOAT);
      }
      if (deflatableBoat) {
        deflatableBoat->moveTo(g.here);
      }

      return RTRUE;
    }
  }

  return RFALSE;
}

// KITCHEN-FCN
// ZIL: M-LOOK (Window state), M-BEG (Stairs Logic)
// Source: 1actions.zil lines 385-401
bool kitchenAction() {
  auto &g = Globals::instance();

  // M-LOOK (Room Description)
  if (g.prsa == V_LOOK) {
    printLine(
        "You are in the kitchen of the white house. A table seems to have been "
        "used recently for the preparation of food. A passage leads to the "
        "west and a dark staircase can be seen leading upward. A dark chimney "
        "leads down and to the east is a small window which is ");

    ZObject *win = g.getObject(ObjectIds::KITCHEN_WINDOW);
    if (win && win->hasFlag(ObjectFlag::OPENBIT)) {
      printLine("open.");
    } else {
      printLine("slightly ajar.");
    }
    return true;
  }

  // M-BEG (Stairs) - Handling CLIMB on STAIRS
  if (g.prso && g.prso->getId() == ObjectIds::STAIRS) {
    if (g.prsa == V_CLIMB_UP) {
      Verbs::vWalkDir(Direction::UP);
      return true;
    }
    if (g.prsa == V_CLIMB_DOWN) {
      printLine("There are no stairs leading down.");
      return true;
    }
  }

  return false;
}

// KNIFE-F - Knife object action
// ZIL: On TAKE, clears ATTIC-TABLE NDESCBIT (makes table visible)
// Source: 1actions.zil lines 926-929
bool knifeAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_TAKE) {
    // ZIL: <FCLEAR ,ATTIC-TABLE ,NDESCBIT>
    // When knife is taken, clear the NDESCBIT on attic table
    // This makes the table visible/described in room descriptions
    ZObject *atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (atticTable) {
      atticTable->clearFlag(ObjectFlag::NDESCBIT);
    }
    // Return false to allow normal TAKE handling
  }
  return false;
}
// LARGE-BAG-F
// ZIL: Prevents interaction if Thief is present (Alive/defending).
// Source: 1actions.zil lines 2094-2112
bool largeBagAction() {
  auto &g = Globals::instance();
  ZObject *thief = g.getObject(ObjectIds::THIEF);

  // If Thief isn't here, bag is accessible (Return false to let default run)
  if (!thief || thief->getLocation() != g.here) {
    return false;
  }

  // Thief is HERE. He blocks access.

  // TAKE
  if (g.prsa == V_TAKE) {
    // ZIL checks for Unconscious state (ROBBER-U-DESC).
    // We'll approximate or assume active if HERE.
    // We can check if he has "NDESCBIT" or similar if logic dictates.
    // For now, we block.
    // If he acts "unconscious" (specific flag?), print that message.
    // Else default taunt.

    // Hypothetical check: If description matches unconscious?
    // Simpler: Just block. Fidelity suggests checking state.
    // If we implement Unconscious thief later, we update this.
    // Default Logic:
    printLine("The bag will be taken over his dead body.");
    return true;
  }

  // PUT (into bag)
  if (g.prsa == V_PUT && g.prsi && g.prsi->getId() == ObjectIds::BAG) {
    printLine("It would be a good trick.");
    return true;
  }

  // OPEN / CLOSE
  if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
    printLine("Getting close enough would be a good trick.");
    return true;
  }

  // EXAMINE / LOOK-INSIDE
  if (g.prsa == V_EXAMINE || g.prsa == V_LOOK_INSIDE) {
    printLine("The bag is underneath the thief, so one can't say what, if "
              "anything, is inside.");
    return true;
  }

  return false;
}
// LEAK-FUNCTION
// ZIL: Repairs leak with Putty.
// Source: 1actions.zil lines 1362-1377
bool leakFunction() {
  auto &g = Globals::instance();

  // Logic only applies if Water Level > 0 (Leaking)
  if (g.waterLevel > 0) {
    // PUT PUTTY (ON LEAK)
    if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && g.prso &&
        g.prso->getId() == ObjectIds::PUTTY) {
      // Success
      g.waterLevel = -1;
      printLine("By some miracle of Zorkian technology, you have managed to "
                "stop the leak in the dam.");
      return true;
    }

    // PLUG LEAK (WITH PUTTY)
    if (g.prsa == V_PLUG) {
      if (g.prsi && g.prsi->getId() == ObjectIds::PUTTY) {
        // Success
        g.waterLevel = -1;
        printLine("By some miracle of Zorkian technology, you have managed to "
                  "stop the leak in the dam.");
        return true;
      }
      // PLUG with Hand/Other
      if (g.prsi) {
        printLine("With a " + g.prsi->getDesc() +
                  "? Do you know how big this dam is? You could only stop a "
                  "tiny leak with that.");
      } else {
        printLine("Plug it with what?");
      }
      return true;
    }
  }

  return false;
}

bool leakAction() {
  auto &g = Globals::instance();

  // Only active if WATER-LEVEL > 0 (dam is leaking)
  if (g.waterLevel <= 0) {
    return false;
  }

  // PUT putty on leak
  if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && g.prso &&
      g.prso->getId() == ObjectIds::PUTTY) {
    // FIX-MAINT-LEAK: repair the dam
    g.waterLevel = -1;
    printLine("By some miracle of Zorkian technology, you have managed to stop "
              "the leak in the dam.");
    return true;
  }

  // PLUG with putty
  if (g.prsa == V_PLUG) {
    if (g.prsi && g.prsi->getId() == ObjectIds::PUTTY) {
      // FIX-MAINT-LEAK: repair the dam
      g.waterLevel = -1;
      printLine("By some miracle of Zorkian technology, you have managed to "
                "stop the leak in the dam.");
      return true;
    } else {
      print("With ");
      if (g.prsi)
        print(g.prsi->getDesc());
      else
        print("that");
      printLine("? You must be joking.");
      return true;
    }
  }

  return false;
}

// LIVING-ROOM-FCN - Living room handler with dynamic description
// ZIL: M-LOOK shows door/trophy/rug/trap door state, M-END updates score
// Source: 1actions.zil lines 449-485
int livingRoomAction(int rarg) {
  auto &g = Globals::instance();
  // ZIL: ,RUG-MOVED and ,MAGIC-FLAG are globals, set by RUG-FCN and by
  // V-ODYSSEUS; reading file-local copies here left the description stale.
  const bool rugMoved = g.rugMoved;
  const bool magicFlag = g.magicFlag;

  // M-LOOK: Dynamic room description
  if (rarg == M_LOOK) {
    print("You are in the living room. There is a doorway to the east");

    // Check door state (magic cyclops door vs nailed shut)
    if (magicFlag) {
      print(". To the west is a cyclops-shaped opening in an old wooden door, "
            "above which is some strange gothic lettering, ");
    } else {
      print(", a wooden door with strange gothic lettering to the west, "
            "which appears to be nailed shut, ");
    }

    print("a trophy case, ");

    // Check rug/trap door state
    ZObject *trapDoor = g.getObject(ObjectIds::TRAP_DOOR);
    bool trapOpen = trapDoor && trapDoor->hasFlag(ObjectFlag::OPENBIT);

    if (rugMoved && trapOpen) {
      printLine("and a rug lying beside an open trap door.");
    } else if (rugMoved) {
      printLine("and a closed trap door at your feet.");
    } else if (trapOpen) {
      printLine("and an open trap door at your feet.");
    } else {
      printLine("and a large oriental rug in the center of the room.");
    }
    return M_HANDLED;
  }

  // ZIL: M-END recomputes the score from the trophy case's contents whenever
  // something was taken or put into the case, touching everything inside so
  // nested treasures count, then SCORE-UPD 0 reports any change.
  // Source: zil/1actions.zil:477-485
  if (rarg == M_END) {
    if (g.prsa == V_TAKE ||
        (g.prsa == V_PUT && g.prsi &&
         g.prsi->getId() == ObjectIds::TROPHY_CASE)) {
      ZObject *tcase = g.getObject(ObjectIds::TROPHY_CASE);
      if (g.prso && tcase && g.prso->getLocation() == tcase)
        touchAll(g.prso);
      g.score = g.baseScore + otvalFrob(nullptr);
      Verbs::scoreUpd(0);
      return M_NOT_HANDLED;
    }
  }
  return M_NOT_HANDLED;
}

// LOUD-ROOM-FCN - Loud room echo puzzle handler
// ZIL: ECHO command sets LOUD-FLAG and clears BAR SACREDBIT, making bar
// takeable Source: 1actions.zil lines 1660-1728
static bool loudFlag = false; // LOUD-FLAG - room has been quieted
extern bool damGatesOpen;     // From actions.cpp - dam gates state

int loudRoomAction(int rarg) {
  auto &g = Globals::instance();

  // M-LOOK: Dynamic room description
  if (rarg == M_LOOK) {
    print("This is a large room with a ceiling which cannot be detected from "
          "the ground. There is a narrow passage from east to west and a stone "
          "stairway leading upward.");

    // Check if room is quiet or loud
    if (loudFlag || !damGatesOpen) {
      printLine(" The room is eerie in its quietness.");
    } else {
      printLine(
          " The room is deafeningly loud with an undetermined rushing sound. "
          "The sound seems to reverberate from all of the walls, making it "
          "difficult even to think.");
    }
    return M_HANDLED;
  }

  // M-END: Eject player if room is too loud
  if (rarg == M_END && damGatesOpen && !loudFlag) {
    printLine(
        "It is unbearably loud here, with an ear-splitting roar seeming to "
        "come from all around you. There is a pounding in your head which "
        "won't "
        "stop. With a tremendous effort, you scramble out of the room.");
    // Player gets ejected to random adjacent room (simplified: go west)
    Verbs::vWalkDir(Direction::WEST);
  }
  return M_NOT_HANDLED;
}

// Handle ECHO command in loud room (called from verb handler)
bool handleEchoInLoudRoom() {
  auto &g = Globals::instance();

  // Only works in loud room when it's loud
  if (g.here && g.here->getId() != RoomIds::LOUD_ROOM) {
    return false;
  }

  if (!loudFlag && damGatesOpen) {
    // Saying ECHO quiets the room and makes platinum bar takeable
    loudFlag = true;

    // Clear SACREDBIT on platinum bar (makes it takeable by thief too)
    ZObject *bar = g.getObject(ObjectIds::BAR);
    if (bar) {
      bar->clearFlag(ObjectFlag::SACREDBIT);
    }

    printLine("The acoustics of the room change subtly.");
    return true;
  }

  // Room is already quiet - just echo
  printLine("Echo...");
  return true;
}

// MACHINE-ROOM-FCN
int machineRoomAction(int rarg) {
  // Stub
  return M_NOT_HANDLED;
}

// MAZE-11-FCN (Special maze room)
// ZIL: M-LOOK shows grating state (open/closed/hidden)
// Source: Reference to CLEARING-FCN for grate logic
int maze11Action(int rarg) {
  auto &g = Globals::instance();

  if (rarg == M_LOOK) {
    printLine("This is part of a maze of twisty little passages, all alike.");

    ZObject *grate = g.getObject(ObjectIds::GRATE);
    if (grate && g.grateRevealed) {
      if (grate->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("There is an open grating in the floor, leading down into "
                  "darkness.");
      } else {
        printLine("There is a grating securely fastened into the ground.");
      }
    }
    return M_HANDLED;
  }
  return M_NOT_HANDLED;
}

// MOUNTAIN-RANGE-F
bool mountainRangeAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_CLIMB_UP || g.prsa == V_CLIMB_DOWN) {
    printLine("The mountains are impassable.");
    return true;
  }
  return false;
}

// MSWITCH-FUNCTION (Machine switch)
// ZIL: TURN with SCREWDRIVER on closed machine -> transforms contents
// Source: 1actions.zil lines 2531-2551
bool machineSwitchAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_TURN) {
    // Requires SCREWDRIVER
    if (g.prsi && g.prsi->getId() == ObjectIds::SCREWDRIVER) {
      ZObject *machine = g.getObject(ObjectIds::MACHINE);
      if (machine && machine->hasFlag(ObjectFlag::OPENBIT)) {
        // Machine lid is open - doesn't work
        printLine("The machine doesn't seem to want to do anything.");
        return true;
      }

      // Machine lid is closed - run transformation
      printLine("The machine comes to life (figuratively) with a dazzling "
                "display of colored lights and bizarre noises. After a few "
                "moments, the excitement abates.");

      // Check for COAL inside machine
      ZObject *coal = g.getObject(ObjectIds::COAL);
      if (machine && coal && coal->getLocation() == machine) {
        // Transform COAL to DIAMOND
        coal->moveTo(nullptr); // Remove coal
        ZObject *diamond = g.getObject(ObjectIds::DIAMOND);
        if (diamond) {
          diamond->moveTo(machine);
        }
      } else if (machine) {
        // Remove all contents and add GUNK
        auto contents = machine->getContents();
        for (ZObject *obj : contents) {
          obj->moveTo(nullptr);
        }
        ZObject *gunk = g.getObject(ObjectIds::GUNK);
        if (gunk) {
          gunk->moveTo(machine);
        }
      }
      return true;
    } else {
      // Wrong tool
      if (g.prsi) {
        printLine("It seems that a " + g.prsi->getDesc() + " won't do.");
      } else {
        printLine("You need a tool to turn the switch.");
      }
      return true;
    }
  }

  return false;
}

// NOT-HERE-OBJECT-F (gglobals.zil:52-74)
bool notHereObjectAction() {
  return GGlobals::notHereObjectF();
}

// ZIL: PATH-OBJECT (gglobals.zil:282-288)
// Handles TAKE/FOLLOW, FIND, DIG for PATHOBJ
bool pathObjectAction() {
  return GGlobals::pathObject();
}

// NULL-F (gglobals.zil:85-87)
bool nullAction() {
  return GGlobals::nullF();
}

// PUTTY-FCN
// PUTTY-FCN - Putty interactions
// ZIL: Handles V_OIL and V_PUT to prevent using putty as lubricant
// Source: 1actions.zil lines 1379-1384
bool puttyAction() {
  auto &g = Globals::instance();

  // V_OIL with putty as indirect object, or V_PUT with putty as direct object
  if ((g.prsa == V_OIL && g.prsi && g.prsi->getId() == ObjectIds::PUTTY) ||
      (g.prsa == V_PUT && g.prso && g.prso->getId() == ObjectIds::PUTTY)) {
    printLine("The all-purpose gunk isn't a lubricant.");
    return RTRUE;
  }

  return RFALSE;
}

// RAINBOW-FCN
// RAINBOW-FCN - Rainbow/pot of gold interactions
// ZIL: Handles CROSS/THROUGH (with location and rainbow flag checks) and
// LOOK-UNDER Source: 1actions.zil lines 2634-2650
bool rainbowAction() {
  auto &g = Globals::instance();

  // Handle CROSS / THROUGH
  if (g.prsa == V_CROSS || g.prsa == V_THROUGH) {
    // Special case: Canyon View
    if (g.here && g.here->getId() == RoomIds::CANYON_VIEW) {
      printLine("From here?!?");
      return RTRUE;
    }

    // Check if rainbow is solid (RAINBOW-FLAG)
    if (g.rainbowFlag) {
      // Rainbow is solid, can cross
      if (g.here && g.here->getId() == RoomIds::ARAGAIN_FALLS) {
        // GOTO END-OF-RAINBOW
        ZObject *endOfRainbow = g.getObject(RoomIds::END_OF_RAINBOW);
        if (endOfRainbow) {
          g.player->moveTo(endOfRainbow);
          g.here = endOfRainbow;
        }
        return RTRUE;
      } else if (g.here && g.here->getId() == RoomIds::END_OF_RAINBOW) {
        // GOTO ARAGAIN-FALLS
        ZObject *aragainFalls = g.getObject(RoomIds::ARAGAIN_FALLS);
        if (aragainFalls) {
          g.player->moveTo(aragainFalls);
          g.here = aragainFalls;
        }
        return RTRUE;
      } else {
        printLine("You'll have to say which way...");
        return RTRUE;
      }
    } else {
      // Rainbow not solid
      printLine("Can you walk on water vapor?");
      return RTRUE;
    }
  }

  // Handle LOOK-UNDER
  if (g.prsa == V_LOOK_UNDER) {
    printLine("The Frigid River flows under the rainbow.");
    return RTRUE;
  }

  return RFALSE;
}

// RESERVOIR-FCN
int reservoirAction(int rarg) {
  // Handle water level, swimming, etc.
  return M_NOT_HANDLED;
}

// RESERVOIR-NORTH-FCN
int reservoirNorthAction(int rarg) {
  // Stub
  return M_NOT_HANDLED;
}

// RESERVOIR-SOUTH-FCN
int reservoirSouthAction(int rarg) {
  // Stub
  return M_NOT_HANDLED;
}

// ROBBER-FUNCTION (Thief NPC AI)
bool robberAction() {
  // Complex thief AI - stub for now
  return false;
}

// RUSTY-KNIFE-FCN - Cursed knife that kills player when used to attack
// ZIL Source: 1actions.zil lines 907-924
bool rustyKnifeAction() {
  auto &g = Globals::instance();

  // TAKE with sword present - sword glows
  if (g.prsa == V_TAKE) {
    ZObject *sword = g.getObject(ObjectIds::SWORD);
    if (sword && sword->getLocation() == g.winner) {
      printLine("As you touch the rusty knife, your sword gives a single pulse "
                "of blinding blue light.");
    }
    return false; // Allow take to proceed
  }

  // ATTACK with rusty knife or SWING at something - kills player!
  if ((g.prsa == V_ATTACK && g.prsi &&
       g.prsi->getId() == ObjectIds::RUSTY_KNIFE) ||
      (g.prsa == V_SWING && g.prso &&
       g.prso->getId() == ObjectIds::RUSTY_KNIFE && g.prsi)) {
    // Remove knife and kill player
    ZObject *knife = g.getObject(ObjectIds::RUSTY_KNIFE);
    if (knife)
      knife->moveTo(nullptr);

    printLine(
        "As the knife approaches its victim, your mind is submerged by an "
        "overmastering will. Slowly, your hand turns, until the rusty blade "
        "is an inch from your neck. The knife seems to sing as it savagely "
        "slits your throat.");
    // Trigger death using jigsUp (the knife curse has already printed its
    // message)
    DeathSystem::jigsUp("", DeathSystem::DeathCause::OTHER);
    return true;
  }

  return false;
}

// SAILOR-FCN (gglobals.zil:122-162)
bool sailorAction() {
  return GGlobals::sailorFcn();
}

// SAND-FUNCTION
bool sandAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_DIG) {
    // Check for shovel
    ZObject *shovel = g.getObject(ObjectIds::SHOVEL);
    if (g.prsi == shovel || (shovel && shovel->getLocation() == g.winner)) {
      printLine("You dig in the sand and reveal a scarab!");
      // Reveal scarab
      return true;
    } else {
      printLine("You dig with your hands but find nothing.");
      return true;
    }
  }
  return false;
}

// SANDWICH-BAG-FCN
bool sandwichBagAction() {
  return stupidContainerAction(ObjectIds::SANDWICH_BAG, "food");
}

// SCEPTRE-FUNCTION
bool sceptreAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_WAVE) {
    // Check if at rainbow
    printLine("A dazzling display of color!");
    return true;
  }
  return false;
}

// SLIDE-FUNCTION
bool slideAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_CLIMB_DOWN) {
    printLine("You slide down and land in the Cellar.");
    // Move player to Cellar
    return true;
  }
  return false;
}

// SONGBIRD-F
// ZIL: <ROUTINE SONGBIRD-F () ...>
// Source: zil/1actions.zil:84-93
//
// The songbird is never actually present: every branch explains its absence,
// and the final clause answers anything else.
int songbirdAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_FIND || g.prsa == V_TAKE) {
    printLine("The songbird is not here but is probably nearby.");
    return M_HANDLED;
  }
  if (g.prsa == V_LISTEN) {
    printLine("You can't hear the songbird now.");
    return M_HANDLED;
  }
  if (g.prsa == V_FOLLOW) {
    printLine("It can't be followed.");
    return M_HANDLED;
  }
  printLine("You can't see any songbird here.");
  return M_HANDLED;
}

// SOUTH-TEMPLE-FCN
int southTempleAction(int rarg) {
  // Stub
  return M_NOT_HANDLED;
}

// STAIRS-F (gglobals.zil:110-113)
bool stairsAction() {
  return GGlobals::stairsF();
}

// STILETTO-FUNCTION (Thief's knife)
bool stilettoAction() {
  auto &g = Globals::instance();
  ZObject *thief = g.getObject(ObjectIds::THIEF);
  if (thief && thief->getLocation() == g.here) {
    if (g.prsa == V_TAKE) {
      printLine("The thief deftly snatches the stiletto out of your reach.");
      return true;
    }
  }
  return false;
}

// TEETH-F - Teeth object action handler
// ZIL Source: 1actions.zil lines 48-62
// Handles BRUSH verb with putty death, and EXAMINE/TAKE
bool teethAction() {
  auto &g = Globals::instance();

  // Handle BRUSH TEETH - ZIL checks prso is TEETH
  if (g.prsa == V_BRUSH && g.prso && g.prso->getId() == ObjectIds::TEETH) {
    // Check if brushing WITH PUTTY = death
    // ZIL: <AND <EQUAL? ,PRSI ,PUTTY> <IN? ,PRSI ,WINNER>>
    if (g.prsi && g.prsi->getId() == ObjectIds::PUTTY &&
        g.prsi->getLocation() == g.player) {
      DeathSystem::jigsUp(
          "Well, you seem to have been brushing your teeth with some sort of "
          "glue. As a result, your mouth gets glued together (with your nose) "
          "and you die of respiratory failure.",
          DeathSystem::DeathCause::OTHER);
      return true;
    }
    // BRUSH TEETH with nothing specified
    if (!g.prsi) {
      printLine("Dental hygiene is highly recommended, but I'm not sure what "
                "you want to brush them with.");
      return true;
    }
    // BRUSH TEETH with something else
    printLine("A nice idea, but with a " + g.prsi->getDesc() + "?");
    return true;
  }

  // Handle EXAMINE
  if (g.prsa == V_EXAMINE) {
    printLine("The teeth are razor sharp.");
    return true;
  }

  // Handle TAKE - scenery can't be taken
  if (g.prsa == V_TAKE) {
    printLine("The teeth are embedded in something. You can't take them.");
    return true;
  }

  return false;
}

// TOOL-CHEST-FCN
bool toolChestAction() {
  auto &g = Globals::instance();

  // ZIL: EXAMINE -> "The chests are all empty."
  // ZIL: TAKE/OPEN/PUT -> Remove object, print "The chests are so rusty..."
  // Source: 1actions.zil lines 1332-1340

  if (g.prsa == V_EXAMINE) {
    printLine("The chests are all empty.");
    return true;
  }

  if (g.prsa == V_TAKE || g.prsa == V_OPEN || g.prsa == V_PUT) {
    printLine("The chests are so rusty and corroded that they crumble when you "
              "touch them.");
    g.prso->moveTo(nullptr); // REMOVE-CAREFULLY
    return true;
  }

  return false;
}

// TORCH-ROOM-FCN
int torchRoomAction(int rarg) {
  // Stub
  return M_NOT_HANDLED;
}

// TRAP-DOOR-FCN
// ZIL: <ROUTINE TRAP-DOOR-FCN () ...>
// Source: zil/1actions.zil:507-531
int trapDoorAction() {
  auto &g = Globals::instance();
  ZObject *door = g.getObject(ObjectIds::TRAP_DOOR);

  if (g.prsa == V_RAISE) {
    perform(V_OPEN, door);
    return M_HANDLED;
  }

  const bool inLivingRoom = g.here && g.here->getId() == RoomIds::LIVING_ROOM;

  if ((g.prsa == V_OPEN || g.prsa == V_CLOSE) && inLivingRoom) {
    return openClose(g.prso,
                     "The door reluctantly opens to reveal a rickety "
                     "staircase descending into darkness.",
                     "The door swings shut and closes.");
  }

  if (g.prsa == V_LOOK_UNDER && inLivingRoom) {
    if (door && door->hasFlag(ObjectFlag::OPENBIT))
      printLine("You see a rickety staircase descending into darkness.");
    else
      printLine("It's closed.");
    return M_HANDLED;
  }

  if (g.here && g.here->getId() == RoomIds::CELLAR) {
    const bool open = door && door->hasFlag(ObjectFlag::OPENBIT);
    if ((g.prsa == V_OPEN || g.prsa == V_UNLOCK) && !open) {
      printLine("The door is locked from above.");
      return M_HANDLED;
    }
    if (g.prsa == V_CLOSE && !open) {
      door->clearFlag(ObjectFlag::TOUCHBIT);
      door->clearFlag(ObjectFlag::OPENBIT);
      printLine("The door closes and locks.");
      return M_HANDLED;
    }
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
      printLine(VerbTables::dummy().pickOne());
      return M_HANDLED;
    }
  }

  return M_NOT_HANDLED;
}

// TREASURE-ROOM-FCN
int treasureRoomAction(int rarg) {
  // Handle thief's lair logic
  return M_NOT_HANDLED;
}

// ZIL: <ROUTINE TROLL-ROOM-F (RARG) <COND (<AND <EQUAL? .RARG ,M-ENTER>
//        <IN? ,TROLL ,HERE>> <THIS-IS-IT ,TROLL>)>>
// Source: zil/1actions.zil:4175-4178
int trollRoomAction(int rarg) {
  auto &g = Globals::instance();
  if (rarg == M_ENTER) {
    ZObject *troll = g.getObject(ObjectIds::TROLL);
    if (troll && troll->getLocation() == g.here) {
      Verbs::thisIsIt(troll);
    }
  }
  return M_NOT_HANDLED;
}

// TRUNK-F is already defined above

// CANYON-VIEW-F
bool canyonViewAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_EXAMINE || g.prsa == V_LOOK) {
    printLine("You can see a deep canyon below.");
    return true;
  }
  return false;
}

// CHALICE-FCN
// CHALICE-FCN - Chalice interaction
// ZIL: PUT rejection ("not a good chalice"). TAKE check (Thief stab).
// Source: 1actions.zil lines 2123-2136
bool chaliceAction() {
  auto &g = Globals::instance();

  // Handle PUT stuff IN CHALICE (PRSI == CHALICE)
  if (g.prsa == V_PUT && g.prsi && g.prsi->getId() == ObjectIds::CHALICE) {
    printLine("You can't. It's not a very good chalice, is it?");
    return RTRUE;
  }

  // Handle TAKE (Thief check)
  if (g.prsa == V_TAKE) {
    ZObject *chalice = g.getObject(ObjectIds::CHALICE);
    ZObject *thief = g.getObject(ObjectIds::THIEF);

    // ZIL: If in Treasure Room, Thief Present, Fighting, Visible...
    // We simplify slightly: If Thief is fighting here and guarding it.
    // Assuming location check matches ZIL.
    if (chalice && chalice->getLocation() &&
        chalice->getLocation()->getId() == RoomIds::TREASURE_ROOM) {
      if (thief && thief->getLocation() &&
          thief->getLocation()->getId() == RoomIds::TREASURE_ROOM) {
        if (thief->hasFlag(ObjectFlag::FIGHTBIT) &&
            !thief->hasFlag(ObjectFlag::INVISIBLE)) {
          printLine("You'd be stabbed in the back first.");
          return RTRUE;
        }
      }
    }
  }

  return RFALSE;
}

// BAT-F - Bat attack/defense, garlic prevents being grabbed
// ZIL: TAKE/ATTACK - if garlic present "can't reach him", else fly-me teleports
const std::vector<ObjectId> BAT_DROPS = {
    RoomIds::MINE_1,       RoomIds::MINE_2,     RoomIds::MINE_3,
    RoomIds::MINE_4,       RoomIds::LADDER_TOP, RoomIds::LADDER_BOTTOM,
    RoomIds::SQUEEKY_ROOM, RoomIds::MINE_ENTRANCE};

// Helper for Fweep printing (ZIL: FWEEP, 1actions.zil:326-330)
// ZIL: <ROUTINE FWEEP (N)
//        <REPEAT () <COND (<L? <SET N <- .N 1>> 1> <RETURN>)
//                         (T <TELL "    Fweep!" CR>)>>
//        <CRLF>>
// Source: zil/1actions.zil:326-330
//
// The decrement happens before the test, so FWEEP prints N-1 lines: the
// bat's <FWEEP 4> is three Fweeps and <FWEEP 6> is five.  A blank line
// follows.
void fweep(int n) {
  while (true) {
    n = n - 1;
    if (n < 1)
      break;
    tell("    Fweep!", CR);
  }
  crlf();
}

// FLY-ME logic (ZIL: FLY-ME, 1actions.zil:317-324)
void flyMe() {
  auto &g = Globals::instance();
  fweep(4);
  tell("The bat grabs you by the scruff of your neck and lifts you away....",
       CR);
  crlf();

  // <GOTO <PICK-ONE ,BAT-DROPS> <>>
  if (!BAT_DROPS.empty()) {
    // ZIL: <GOTO <PICK-ONE ,BAT-DROPS> <>> (1actions.zil:320)
    static GMacros::ZilRandomTable<ObjectId> drops{
        std::vector<ObjectId>(BAT_DROPS.begin(), BAT_DROPS.end())};
    ObjectId targetId = drops.pickOne();
    ZObject *target = g.getObject(targetId);
    if (target) {
      if (g.winner) {
        g.winner->moveTo(target);
      }
      if (g.player && g.player != g.winner) {
        g.player->moveTo(target);
      }
      g.here = target;
      if (target->getId() != RoomIds::ENTRANCE_TO_HADES) {
        Verbs::vLook();
      }
    }
  }
}

// BAT-F
bool batAction() {
  auto &g = Globals::instance();

  // <VERB? TELL> -> FWEEP 6
  if (g.prsa == V_TELL) {
    fweep(6);
    g.pCont = 0; // ZIL: <SETG P-CONT <>> (1actions.zil:310)
    return true;
  }

  // Handle TAKE, ATTACK, MUNG
  if (g.prsa == V_TAKE || g.prsa == V_ATTACK) {
    // Check for garlic - protects player from bat
    // ZIL: <EQUAL? <LOC ,GARLIC> ,WINNER ,HERE>
    ZObject *garlic = g.getObject(ObjectIds::GARLIC);
    bool hasGarlic = false;
    if (garlic) {
      hasGarlic = (garlic->getLocation() == g.player ||
                   garlic->getLocation() == g.here);
    }

    if (hasGarlic) {
      printLine("You can't reach him; he's on the ceiling.");
    } else {
      // FLY-ME
      flyMe();
    }
    return true;
  }

  return false;
}

// BELL-F (Normal bell, not hot)
bool bellAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_RING) {
    // ZIL: <COND (<AND <EQUAL? ,HERE ,LLD-ROOM> <NOT ,LLD-FLAG>> <RFALSE>) ...>
    if (g.here && g.here->getId() == RoomIds::LAND_OF_LIVING_DEAD &&
        !g.lldFlag) {
      return false;
    }

    printLine("Ding, dong.");
    return true;
  }
  return false;
}

// BOARDED-WINDOW-FCN - Boarded window blocking
// ZIL: OPEN = "windows are boarded", MUNG = "can't break windows open"
// Source: 1actions.zil lines 370-374
bool boardedWindowAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_OPEN) {
    printLine("The windows are boarded and can't be opened.");
    return true;
  }
  if (g.prsa == V_MUNG) { // MUNG = break
    printLine("You can't break the windows open.");
    return true;
  }
  return false;
}

// BODY-FUNCTION - Dead adventurer bodies
// ZIL: TAKE = "force keeps you", MUNG/BURN = death
// Source: 1actions.zil lines 2178-2185
bool bodyAction() {
  auto &g = Globals::instance();
  if (g.prsa == V_TAKE) {
    printLine("A force keeps you from taking the bodies.");
    return true;
  }
  if (g.prsa == V_ATTACK || g.prsa == V_BURN) {
    // Death for disrespecting the bodies
    printLine(
        "The voice of the guardian of the dungeon booms out from the darkness, "
        "\"Your disrespect costs you your life!\" and places your head on a "
        "sharp pole.");
    DeathSystem::jigsUp("", DeathSystem::DeathCause::OTHER);
    return true;
  }
  return false;
}

// KITCHEN-FCN - Kitchen room handler
// ZIL: Handles M-LOOK (description with window state) and M-BEG (climb stairs)
// Source: 1actions.zil lines 385-401
int kitchenAction(int rarg) {
  auto &g = Globals::instance();

  // M-LOOK: Print room description with window state
  if (rarg == M_LOOK) { // M-LOOK equivalent
    print("You are in the kitchen of the white house. A table seems to "
          "have been used recently for the preparation of food. A passage "
          "leads to the west and a dark staircase can be seen leading "
          "upward. A dark chimney leads down and to the east is a small "
          "window which is ");

    // Check kitchen window OPENBIT
    ZObject *window = g.getObject(ObjectIds::KITCHEN_WINDOW);
    if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
      printLine("open.");
    } else {
      printLine("slightly ajar.");
    }
  }

  // M-BEG: Handle CLIMB-UP STAIRS
  // ZIL: (<==? .RARG ,M-BEG>
  //        <COND (<AND <VERB? CLIMB-UP> <EQUAL? ,PRSO ,STAIRS>> <DO-WALK ,P?UP>)
  //              (<AND <VERB? CLIMB-UP> <EQUAL? ,PRSO ,STAIRS>>
  //               <TELL "There are no stairs leading down." CR>)>)
  // Source: 1actions.zil:396-400. The second clause repeats the CLIMB-UP test
  // (original bug), so "There are no stairs leading down." is unreachable.
  if (rarg == M_BEG) {
    ZObject *stairs = g.getObject(ObjectIds::STAIRS);
    if (g.prsa == V_CLIMB_UP && g.prso == stairs) {
      return Verbs::doWalk(Direction::UP);
    }
    if (g.prsa == V_CLIMB_UP && g.prso == stairs) {
      printLine("There are no stairs leading down.");
      return M_HANDLED;
    }
  }
  return M_NOT_HANDLED;
}

// =============================================================================
// TUBE-FUNCTION - Tube of putty
// ZIL: PUT refused. SQUEEZE dispenses putty if open, else "closed" or "empty"
// Source: 1actions.zil lines 1386-1400
// =============================================================================
bool tubeAction() {
  auto &g = Globals::instance();

  if (g.prsa == V_PUT && g.prsi && g.prsi->getId() == ObjectIds::TUBE) {
    printLine("The tube refuses to accept anything.");
    return RTRUE;
  }

  if (g.prsa == V_SQUEEZE && g.prso && g.prso->getId() == ObjectIds::TUBE) {
    ZObject *tube = g.prso;
    ZObject *putty = g.getObject(ObjectIds::PUTTY);

    if (tube->hasFlag(ObjectFlag::OPENBIT)) {
      if (putty && putty->getLocation() == tube) {
        // Move putty to player
        putty->moveTo(g.player);
        printLine("The viscous material oozes into your hand.");
      } else {
        printLine("The tube is apparently empty.");
      }
    } else {
      printLine("The tube is closed.");
    }
    return RTRUE;
  }

  return RFALSE;
}

// =============================================================================
// UP-CHIMNEY-FUNCTION - Chimney climbing (already handled by chimneyAction)
// ZIL: Climb up chimney if carrying only lamp
// Source: 1actions.zil lines 553-575
// =============================================================================
// Note: UP-CHIMNEY-FUNCTION logic is already in chimneyAction()

// =============================================================================
// RIVER-FUNCTION - River interactions
// ZIL: Handles PUT into river (objects, inflated boat, player drowning), LEAP/THROUGH
// Source: 1actions.zil lines 2669-2690
// =============================================================================
bool riverAction() {
  auto &g = Globals::instance();

  // ZIL: <COND (<VERB? PUT> <COND (<EQUAL? ,PRSI ,RIVER> ...)>)>
  if (g.prsa == V_PUT) {
    if (g.prsi && g.prsi->getId() == ObjectIds::RIVER) {
      if (g.prso && (g.prso == g.player || g.prso->getId() == ObjectIds::ME)) {
        // ZIL: <JIGS-UP "You splash around for a while, fighting the current, then you drown.">
        DeathSystem::jigsUp(
            "You splash around for a while, fighting the current, then you drown.",
            DeathSystem::DeathCause::DROWNING);
        return RTRUE;
      }

      if (g.prso && (g.prso->getId() == ObjectIds::BOAT_INFLATED ||
                     g.prso->getId() == ObjectIds::INFLATED_BOAT)) {
        // ZIL: <TELL "You should get in the boat then launch it." CR>
        printLine("You should get in the boat then launch it.");
        return RTRUE;
      }

      if (g.prso) {
        std::string desc = g.prso->getDesc();
        bool hasBurn = g.prso->hasFlag(ObjectFlag::BURNBIT);
        g.prso->moveTo(nullptr); // REMOVE-CAREFULLY

        if (hasBurn) {
          // ZIL: "The " D ,PRSO " floats for a moment, then sinks."
          printLine(std::format("The {} floats for a moment, then sinks.", desc));
        } else {
          // ZIL: "The " D ,PRSO " splashes into the water and is gone forever."
          printLine(std::format("The {} splashes into the water and is gone forever.", desc));
        }
        return RTRUE;
      }
    }
  }

  // ZIL: <COND (<VERB? LEAP THROUGH> ...)>
  if (g.prsa == V_JUMP || g.prsa == V_ENTER || g.prsa == V_SWIM) {
    printLine(
        "A look before leaping reveals that the river is wide and dangerous, "
        "with swift currents and large, half-hidden rocks. You decide to forgo your "
        "swim.");
    return RTRUE;
  }

  return RFALSE;
}

// =============================================================================
// WATER-F - Water interactions
// ZIL: Complex handler for TAKE, PUT, FILL, DROP, GIVE, THROW with water
// Handles bottle filling, evaporation, puddles in vehicles
// Source: 1actions.zil lines 3300-3390
// =============================================================================
bool waterAction() {
  auto &g = Globals::instance();

  ZObject *water = g.getObject(ObjectIds::WATER);
  ZObject *bottle = g.getObject(ObjectIds::BOTTLE);

  // FILL verb - "fill bottle with water"
  if (g.prsa == V_FILL) {
    if (!bottle) {
      printLine("You have nothing to fill.");
      return RTRUE;
    }
    if (!bottle->hasFlag(ObjectFlag::OPENBIT)) {
      printLine("The bottle is closed.");
      return RTRUE;
    }
    if (!bottle->getContents().empty()) {
      printLine("The bottle is not empty.");
      return RTRUE;
    }
    if (water) {
      water->moveTo(bottle);
      printLine("The bottle is now full of water.");
    } else {
      printLine("There is no water here to fill with.");
    }
    return RTRUE;
  }

  // TAKE water
  if (g.prsa == V_TAKE) {
    if (water && water->getLocation() == bottle) {
      printLine("It's in the bottle. Perhaps you should take that instead.");
      return RTRUE;
    }
    if (bottle && bottle->getLocation() == g.player &&
        !bottle->hasFlag(ObjectFlag::OPENBIT)) {
      printLine("The bottle is closed.");
      return RTRUE;
    }
    if (bottle && bottle->getLocation() == g.player &&
        bottle->hasFlag(ObjectFlag::OPENBIT) &&
        !!bottle->getContents().empty()) {
      if (water) {
        water->moveTo(bottle);
        printLine("The bottle is now full of water.");
        return RTRUE;
      }
    }
    printLine("The water slips through your fingers.");
    return RTRUE;
  }

  // PUT water in container
  if (g.prsa == V_PUT && g.prso == water) {
    if (g.prsi == bottle) {
      if (!bottle->hasFlag(ObjectFlag::OPENBIT)) {
        printLine("The bottle is closed.");
        return RTRUE;
      }
      if (!bottle->getContents().empty()) {
        printLine("The bottle is not empty.");
        return RTRUE;
      }
      if (water) {
        water->moveTo(bottle);
        printLine("The bottle is now full of water.");
      }
      return RTRUE;
    }
    // Water in other containers evaporates
    if (g.prsi) {
      print("The water leaks out of the ");
      print(g.prsi->getDesc());
      printLine(" and evaporates immediately.");
      if (water)
        water->moveTo(nullptr);
      return RTRUE;
    }
  }

  // DROP/GIVE water
  if (g.prsa == V_DROP || g.prsa == V_GIVE) {
    if (water && water->getLocation() == bottle &&
        !bottle->hasFlag(ObjectFlag::OPENBIT)) {
      printLine("The bottle is closed.");
      return RTRUE;
    }
    printLine("The water spills to the floor and evaporates immediately.");
    if (water)
      water->moveTo(nullptr);
    return RTRUE;
  }

  // THROW water
  if (g.prsa == V_THROW) {
    printLine("The water splashes on the walls and evaporates immediately.");
    if (water)
      water->moveTo(nullptr);
    return RTRUE;
  }

  return RFALSE;
}
