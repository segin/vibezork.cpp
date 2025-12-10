#pragma once
#include "types.h"
#include "object.h"
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
    static Globals& instance();
    
    // Core objects (ZIL: global variables in GGLOBALS.ZIL)
    ZObject* here = nullptr;           ///< Current room (ZIL: ,HERE)
    ZObject* winner = nullptr;         ///< Current actor, usually player (ZIL: ,WINNER)
    ZObject* player = nullptr;         ///< Player object (ZIL: ,ADVENTURER)
    ZObject* prso = nullptr;           ///< Parser Result Subject Object - direct object (ZIL: ,PRSO)
    ZObject* prsi = nullptr;           ///< Parser Result Subject Indirect - indirect object (ZIL: ,PRSI)
    VerbId prsa = 0;                   ///< Parser Result Subject Action - current verb (ZIL: ,PRSA)
    
    // Game state
    bool lit = false;                  // Is current room lit?
    int score = 0;
    int moves = 0;
    int loadMax = 100;
    int loadAllowed = 100;
    
    // Lamp state (Requirement 47)
    int lampBattery = 330;             // Lamp battery life in turns (starts at 330)
    bool lampWarned = false;           // Has low battery warning been shown?
    
    // Puzzle state flags
    bool rugMoved = false;             // Has the rug been moved to reveal trap door?
    bool lldFlag = false;              // Land of Living Dead flag (ceremony complete?)
    bool gateFlag = false;             // Dam control panel activated (Green Bubble light)
    bool gatesOpen = false;            // Are dam sluice gates open?
    bool lowTide = false;              // Is water level in reservoir low? (ZIL: LOW-TIDE)
    int waterLevel = 0;                // Water level in Maintenance Room (0=Dry, >0=Rising)
    bool grateRevealed = false;        // Has the grating been revealed in the Clearing?
    
    // Display modes
    bool verboseMode = true;           // Full descriptions
    bool briefMode = false;            // Short descriptions for visited rooms
    bool superbriefMode = false;       // Minimal descriptions
    bool scripting = false;            // Transcript output enabled
    
    // Parser state
    bool pCont = false;
    bool quoteFlag = false;
    
    // Object registry - using unordered_map for O(1) lookup
    void registerObject(ObjectId id, std::unique_ptr<ZObject> obj);
    ZObject* getObject(ObjectId id);
    const std::unordered_map<ObjectId, std::unique_ptr<ZObject>>& getAllObjects() const { return objects_; }
    
    // Reset for testing
    void reset();
    
private:
    Globals() = default;
    std::unordered_map<ObjectId, std::unique_ptr<ZObject>> objects_;
};
