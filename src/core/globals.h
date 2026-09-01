#pragma once
#include "object.h"
#include "types.h"
#include <memory>
#include <unordered_map>

/**
 * @brief Global game state singleton (mirrors ZIL global variables)
 *
 * This class manages all global game state including:
 * - Current location and actor (HERE, WINNER from ZIL)
 * - Parser results (PRSO, PRSI, PRSA)
 * - Game progress (score, moves)
 * - Display mode settings
 * - Object registry for all game entities
 *
 * Access via Globals::instance() singleton pattern.
 *
 * @see ZIL equivalent: GGLOBALS.ZIL global variables
 */
class Globals {
public:
  static Globals &instance();

  // Core objects (ZIL: global variables in GGLOBALS.ZIL)
  ZObject *here = nullptr;   ///< Current room (ZIL: ,HERE)
  ZObject *winner = nullptr; ///< Current actor, usually player (ZIL: ,WINNER)
  ZObject *player = nullptr; ///< Player object (ZIL: ,ADVENTURER)
  ZObject *prso =
      nullptr; ///< Parser Result Subject Object - direct object (ZIL: ,PRSO)
  ZObject *prsi = nullptr; ///< Parser Result Subject Indirect - indirect object
                           ///< (ZIL: ,PRSI)
  VerbId prsa = 0; ///< Parser Result Subject Action - current verb (ZIL: ,PRSA)
  ZObject *it = nullptr; ///< Last referenced object (ZIL: ,P-IT-OBJECT)

  // Game state (ZIL: SCORE-MAX, FALSE-FLAG)
  static constexpr int SCORE_MAX = 350;
  bool falseFlag = false; // ZIL: ,FALSE-FLAG
  bool lit = false; // Is current room lit?
  int score = 0;
  int moves = 0;
  int loadMax = 100;
  int loadAllowed = 100;

  // Lamp state (Requirement 47)
  int lampBattery = 330;   // Lamp battery life in turns (starts at 330)
  bool lampWarned = false; // Has low battery warning been shown?

  // Puzzle and Conditional Exit Flags (ZIL: 1dungeon.zil:1223-1235)
  bool cyclopsFlag = false;   // ZIL: ,CYCLOPS-FLAG (Cyclops asleep or fled)
  bool deflate = false;       // ZIL: ,DEFLATE (Can squeeze through tight beach passages)
  bool domeFlag = false;      // ZIL: ,DOME-FLAG (Rope tied to railing in Dome Room)
  bool emptyHanded = false;   // ZIL: ,EMPTY-HANDED (Shaft basket lowered/raised empty)
  bool lldFlag = false;       // ZIL: ,LLD-FLAG (Land of Living Dead spirits dispersed)
  bool lowTide = false;       // ZIL: ,LOW-TIDE (Reservoir gates opened, water level low)
  bool magicFlag = false;     // ZIL: ,MAGIC-FLAG (Odysseus spoken, Strange Passage open)
  bool rainbowFlag = false;   // ZIL: ,RAINBOW-FLAG (Rainbow solidified, walkable)
  bool trollFlag = false;     // ZIL: ,TROLL-FLAG (Troll dead/fled, Troll Room passages unblocked)
  bool wonFlag = false;       // ZIL: ,WON-FLAG (Score-max reached, Stone Barrow entrance open)
  bool coffinCure = false;    // ZIL: ,COFFIN-CURE (Prayer answered in Egypt Room)

  // Additional puzzle state
  bool rugMoved = false;      // Has the rug been moved to reveal trap door?
  bool gateFlag = false;      // Dam control panel activated (Green Bubble light)
  bool gatesOpen = false;     // Are dam sluice gates open?
  bool grunlock = false;      // Is grate unlocked? (ZIL: GRUNLOCK)
  int waterLevel = 0;         // Water level in Maintenance Room (0=Dry, >0=Rising)
  bool grateRevealed = false; // Has the grating been revealed in the Clearing?
  int matchCount = 5;         // Number of matches in matchbook
  bool buoyFlag = true;       // Notice buoy feel on river (ZIL: BUOY-FLAG)

  // Display modes
  bool verboseMode = true;     // Full descriptions
  bool briefMode = false;      // Short descriptions for visited rooms
  bool superbriefMode = false; // Minimal descriptions
  bool scripting = false;      // Transcript output enabled

  // ZIL: GMAIN.ZIL globals
  bool pWon = false;       ///< ZIL: ,P-WON (Parser succeeded flag)
  bool pMult = false;      ///< ZIL: ,P-MULT (Multiple object command flag)
  int pNotHere = 0;        ///< ZIL: ,P-NOT-HERE (Missing object counter)
  VerbId lPrsa = 0;        ///< ZIL: ,L-PRSA (Last action)
  ZObject *lPrso = nullptr;///< ZIL: ,L-PRSO (Last direct object)
  ZObject *lPrsi = nullptr;///< ZIL: ,L-PRSI (Last indirect object)
  bool debug = false;      ///< ZIL: ,DEBUG (Debug execution trace)

  // Parser state
  bool pCont = false;
  bool quoteFlag = false;

  // Object registry - using unordered_map for O(1) lookup
  void registerObject(ObjectId id, std::unique_ptr<ZObject> obj);
  ZObject *getObject(ObjectId id);
  const std::unordered_map<ObjectId, std::unique_ptr<ZObject>> &
  getAllObjects() const {
    return objects_;
  }

  // Reset for testing
  void reset();

private:
  Globals() = default;
  std::unordered_map<ObjectId, std::unique_ptr<ZObject>> objects_;
};
