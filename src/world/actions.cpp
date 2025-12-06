#include "world.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include "systems/npc.h"
#include "systems/candle.h"
#include <memory>

// Global flag for winning the game
bool wonFlag = false;

// Room action functions
void westHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        print("You are standing in an open field west of a white house, with a boarded front door.");
        if (wonFlag) {
            print(" A secret path leads southwest into the forest.");
        }
        crlf();
    }
}

void northHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees.");
    }
}

void southHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are facing the south side of a white house. There is no door here, and all the windows are boarded.");
    }
}

void behindHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are behind the white house. A path leads into the forest to the east. In one corner of the house there is a small window which is slightly ajar.");
    }
}

void stoneBarrowAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are standing in front of a massive barrow of stone. In the east face is a huge stone door which is open. You cannot see into the dark of the tomb.");
    }
}

// Maze room action helper - all maze rooms behave similarly
void mazeRoomAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("This is part of a maze of twisty little passages, all alike.");
    } else if (rarg == M_LISTEN) {
        printLine("You hear the sound of your own footsteps echoing confusingly.");
    } else if (rarg == M_YELL) {
        printLine("Your shout echoes through the passages, making it even harder to tell where you are.");
    }
}

// Object action functions
bool mailboxAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE && g.prso && g.prso->getId() == OBJ_MAILBOX) {
        printLine("It is securely anchored.");
        return RTRUE;
    }
    return RFALSE;
}

bool trophyCaseAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE && g.prso && g.prso->getId() == ObjectIds::TROPHY_CASE) {
        printLine("The trophy case is securely fastened to the wall.");
        return RTRUE;
    }
    return RFALSE;
}

bool coffinAction() {
    auto& g = Globals::instance();
    // Coffin is both a treasure and a container, but has no special behavior
    // It can be taken and opened like a normal container
    return RFALSE;
}

bool basketAction() {
    auto& g = Globals::instance();
    
    // Basket is part of the balloon/shaft system
    // It can be raised and lowered, but cannot be taken
    if (g.prsa == V_TAKE && g.prso && 
        (g.prso->getId() == ObjectIds::BASKET || 
         g.prso->getId() == ObjectIds::RAISED_BASKET)) {
        printLine("The basket is securely fastened to the iron chain.");
        return RTRUE;
    }
    
    // Basket cannot be interacted with in other ways in this simplified version
    return RFALSE;
}

bool sackAction() {
    auto& g = Globals::instance();
    // Sack is a portable container with no special behavior
    return RFALSE;
}

bool bagAction() {
    auto& g = Globals::instance();
    // Bag is a portable container with no special behavior
    return RFALSE;
}

bool bottleAction() {
    auto& g = Globals::instance();
    // Bottle is a portable container with no special behavior
    return RFALSE;
}

bool whiteHouseAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The house is a beautiful colonial house which is painted white. It is clear that the owners must have been extremely wealthy.");
        return RTRUE;
    }
    return RFALSE;
}

bool boardAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("The boards are securely fastened.");
        return RTRUE;
    }
    return RFALSE;
}

bool forestAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_EXAMINE) {
        printLine("The forest is a deep, dark, and foreboding place.");
        return RTRUE;
    }
    return RFALSE;
}

// Rug action - handles MOVE/PUSH to reveal trap door (ZIL: RUG-FCN)
bool rugAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_TAKE) {
        printLine("The rug is extremely heavy and cannot be carried.");
        return RTRUE;
    }
    
    if (g.prsa == V_MOVE || g.prsa == V_PUSH || g.prsa == V_PULL) {
        if (g.rugMoved) {
            printLine("Having moved the carpet previously, you find it impossible to move it again.");
        } else {
            printLine("With a great effort, the rug is moved to one side of the room, revealing the dusty cover of a closed trap door.");
            // Reveal the trap door
            if (auto* trapDoor = g.getObject(ObjectIds::TRAP_DOOR)) {
                trapDoor->clearFlag(ObjectFlag::INVISIBLE);
            }
            g.rugMoved = true;
        }
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE || g.prsa == V_LOOK_INSIDE) {
        if (!g.rugMoved) {
            printLine("Underneath the rug is a closed trap door. As you drop the corner of the rug, the trap door is once again concealed from view.");
        } else {
            printLine("The rug is lying on one side of the room.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Ground action - handles PUT X ON GROUND -> DROP (ZIL: GROUND-FUNCTION)
bool groundAction() {
    auto& g = Globals::instance();
    
    // PUT X ON/IN GROUND -> DROP X
    if ((g.prsa == V_PUT || g.prsa == V_PUT_ON) && g.prsi && 
        g.prsi->getId() == ObjectIds::GROUND) {
        // Redirect to DROP
        g.prsa = V_DROP;
        g.prsi = nullptr;
        return Verbs::vDrop();
    }
    
    if (g.prsa == V_DIG) {
        printLine("The ground is too hard for digging here.");
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE) {
        printLine("There's nothing special about the ground.");
        return RTRUE;
    }
    
    return RFALSE;
}

// Global flag to track if window has been opened (per ZIL KITCHEN-WINDOW-FLAG)
static bool kitchenWindowOpened = false;

bool kitchenWindowAction() {
    auto& g = Globals::instance();
    ZObject* window = g.getObject(ObjectIds::KITCHEN_WINDOW);
    
    if (g.prsa == V_OPEN) {
        if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The window is already open.");
        } else {
            if (window) window->setFlag(ObjectFlag::OPENBIT);
            kitchenWindowOpened = true;
            printLine("With great effort, you open the window far enough to allow entry.");
        }
        return RTRUE;
    }
    
    if (g.prsa == V_CLOSE) {
        if (window && !window->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The window is already closed.");
        } else {
            if (window) window->clearFlag(ObjectFlag::OPENBIT);
            printLine("The window closes (more easily than it opened).");
        }
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE) {
        if (!kitchenWindowOpened) {
            printLine("The window is slightly ajar, but not enough to allow entry.");
        } else if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The window is open.");
        } else {
            printLine("The window is closed.");
        }
        return RTRUE;
    }
    
    if (g.prsa == V_LOOK_INSIDE) {
        if (g.here && g.here->getId() == RoomIds::KITCHEN) {
            printLine("You can see a clear area leading towards a forest.");
        } else {
            printLine("You can see what appears to be a kitchen.");
        }
        return RTRUE;
    }
    
    // Handle ENTER/WALK THROUGH window
    if (g.prsa == V_ENTER || g.prsa == V_WALK || g.prsa == V_BOARD) {
        if (window && window->hasFlag(ObjectFlag::OPENBIT)) {
            // Move through the window
            if (g.here && g.here->getId() == RoomIds::KITCHEN) {
                Verbs::vWalkDir(Direction::EAST);
            } else {
                Verbs::vWalkDir(Direction::WEST);
            }
        } else {
            printLine("The window is not open far enough.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Tool action handlers

// Sword action - glows when near enemies
bool swordAction() {
    auto& g = Globals::instance();
    
    // Sword is a weapon with no special behavior in action handler
    // Glow behavior is handled by timer system (I-SWORD timer)
    // This handler is mainly for future extensibility
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::SWORD) {
        // Check if sword is glowing (has ONBIT flag set by timer)
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The sword is glowing with a faint blue light.");
        } else {
            printLine("The sword is a beautiful elvish blade, but it is not glowing.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Lamp action - provides light when on, battery drains over time
bool lampAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_LAMP_ON && g.prso && g.prso->getId() == ObjectIds::LAMP) {
        // Check if lamp is already on
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The lamp is already on.");
            return RTRUE;
        }
        
        // Check if lamp has battery (property P_STRENGTH tracks battery level)
        int battery = g.prso->getProperty(P_STRENGTH);
        if (battery <= 0) {
            printLine("The lamp has no batteries.");
            return RTRUE;
        }
        
        // Turn on the lamp
        g.prso->setFlag(ObjectFlag::ONBIT);
        printLine("The lamp is now on.");
        return RTRUE;
    }
    
    if (g.prsa == V_LAMP_OFF && g.prso && g.prso->getId() == ObjectIds::LAMP) {
        // Check if lamp is already off
        if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The lamp is already off.");
            return RTRUE;
        }
        
        // Turn off the lamp
        g.prso->clearFlag(ObjectFlag::ONBIT);
        printLine("The lamp is now off.");
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::LAMP) {
        int battery = g.prso->getProperty(P_STRENGTH);
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            if (battery <= 10) {
                printLine("The lamp is on, but the batteries are almost dead.");
            } else {
                printLine("The lamp is on and glowing brightly.");
            }
        } else {
            if (battery <= 0) {
                printLine("The lamp is off and the batteries are dead.");
            } else {
                printLine("The lamp is off.");
            }
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Rope action - used for climbing and tying
bool ropeAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::ROPE) {
        printLine("The rope is a large coil of strong rope, suitable for climbing or tying.");
        return RTRUE;
    }
    
    // Rope can be used for climbing in specific locations (handled by room actions)
    // Rope can be tied to objects (handled by TIE verb)
    // These behaviors are location/context-specific and handled elsewhere
    
    return RFALSE;
}

// Wrench action - used for specific puzzles with bolts
bool wrenchAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::WRENCH) {
        printLine("The wrench is a heavy metal tool, suitable for turning bolts and nuts.");
        return RTRUE;
    }
    
    // Wrench is used for specific puzzles (handled by object/room actions)
    
    return RFALSE;
}

// Screwdriver action - used for specific puzzles with screws
bool screwdriverAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::SCREWDRIVER) {
        printLine("The screwdriver is a metal tool with a flat blade, suitable for turning screws.");
        return RTRUE;
    }
    
    // Screwdriver is used for specific puzzles (handled by object/room actions)
    
    return RFALSE;
}

// Shovel action - used for digging
bool shovelAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::SHOVEL) {
        printLine("The shovel is a sturdy digging tool.");
        return RTRUE;
    }
    
    // Shovel is used for digging (handled by DIG verb and room actions)
    
    return RFALSE;
}

// Torch action - temporary light source that burns down
bool torchAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_LAMP_ON && g.prso && g.prso->getId() == ObjectIds::TORCH) {
        // Check if torch is already on
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The torch is already burning.");
            return RTRUE;
        }
        
        // Check if torch has fuel (property P_STRENGTH tracks remaining fuel)
        int fuel = g.prso->getProperty(P_STRENGTH);
        if (fuel <= 0) {
            printLine("The torch is too wet to light.");
            return RTRUE;
        }
        
        // Light the torch
        g.prso->setFlag(ObjectFlag::ONBIT);
        printLine("The torch is now burning.");
        return RTRUE;
    }
    
    if (g.prsa == V_LAMP_OFF && g.prso && g.prso->getId() == ObjectIds::TORCH) {
        // Check if torch is already off
        if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The torch is not burning.");
            return RTRUE;
        }
        
        // Extinguish the torch
        g.prso->clearFlag(ObjectFlag::ONBIT);
        printLine("The torch is now out.");
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::TORCH) {
        int fuel = g.prso->getProperty(P_STRENGTH);
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            if (fuel <= 5) {
                printLine("The torch is burning, but it's almost burned out.");
            } else {
                printLine("The torch is burning brightly.");
            }
        } else {
            if (fuel <= 0) {
                printLine("The torch is too wet to light.");
            } else {
                printLine("The torch is not burning.");
            }
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Candles action - temporary light source that burns down
bool candlesAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_LAMP_ON && g.prso && g.prso->getId() == ObjectIds::CANDLES) {
        // Check if candles are already lit
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The candles are already lit.");
            return RTRUE;
        }
        
        // Check if candles have wax (property P_STRENGTH tracks remaining wax)
        int wax = g.prso->getProperty(P_STRENGTH);
        if (wax <= 0) {
            printLine("The candles are burned down to nothing.");
            return RTRUE;
        }
        
        // Light the candles
        g.prso->setFlag(ObjectFlag::ONBIT);
        printLine("The candles are now lit.");
        
        // Enable the candle timer (Requirement 48: Enable when candles are lit)
        CandleSystem::enableCandleTimer();
        
        return RTRUE;
    }
    
    if (g.prsa == V_LAMP_OFF && g.prso && g.prso->getId() == ObjectIds::CANDLES) {
        // Check if candles are already out
        if (!g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The candles are not lit.");
            return RTRUE;
        }
        
        // Extinguish the candles
        g.prso->clearFlag(ObjectFlag::ONBIT);
        printLine("The candles are now out.");
        
        // Disable the candle timer (Requirement 48: Disable when candles are extinguished)
        CandleSystem::disableCandleTimer();
        
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::CANDLES) {
        int wax = g.prso->getProperty(P_STRENGTH);
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            if (wax <= 5) {
                printLine("The candles are lit, but they're almost burned down.");
            } else {
                printLine("The candles are lit and burning brightly.");
            }
        } else {
            if (wax <= 0) {
                printLine("The candles are burned down to nothing.");
            } else {
                printLine("The candles are not lit.");
            }
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Matches action - one-time use light source
bool matchesAction() {
    auto& g = Globals::instance();
    
    if (g.prsa == V_LAMP_ON && g.prso && g.prso->getId() == ObjectIds::MATCH) {
        // Check if matches have been used
        if (g.prso->hasFlag(ObjectFlag::ONBIT)) {
            printLine("The matches have already been used.");
            return RTRUE;
        }
        
        // Check if matches are still available
        int count = g.prso->getProperty(P_STRENGTH);
        if (count <= 0) {
            printLine("There are no matches left.");
            return RTRUE;
        }
        
        // Use a match
        g.prso->setProperty(P_STRENGTH, count - 1);
        if (count - 1 <= 0) {
            g.prso->setFlag(ObjectFlag::ONBIT);  // Mark as used up
            printLine("You light a match. It burns brightly for a moment, then goes out. You have no more matches.");
        } else {
            printLine("You light a match. It burns brightly for a moment, then goes out.");
        }
        return RTRUE;
    }
    
    if (g.prsa == V_EXAMINE && g.prso && g.prso->getId() == ObjectIds::MATCH) {
        int count = g.prso->getProperty(P_STRENGTH);
        if (count <= 0) {
            printLine("There are no matches left.");
        } else {
            printLine("You have " + std::to_string(count) + " match" + (count != 1 ? "es" : "") + " left.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// ============================================================================
// TREASURE ACTION HANDLERS
// ============================================================================

// Global flag to track if painting has been examined from back
static bool paintingExaminedBack = false;

// Painting action - special treasure that can be taken from wall, has back side
// Based on PAINTING-FCN from 1actions.zil
bool paintingAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::PAINTING) {
        return RFALSE;
    }
    
    // Handle EXAMINE - show different description based on state
    if (g.prsa == V_EXAMINE) {
        // Check if player is looking at the back
        if (paintingExaminedBack) {
            printLine("The back of the painting is covered with canvas, with no visible markings.");
            paintingExaminedBack = false;  // Reset for next examine
        } else {
            printLine("A masterpiece by a neglected genius. It depicts a beautiful landscape with a babbling brook and a distant village. The painting is quite valuable.");
        }
        return RTRUE;
    }
    
    // Handle TURN/FLIP - look at back side
    if (g.prsa == V_TURN) {
        printLine("You turn the painting over, revealing the back.");
        paintingExaminedBack = true;
        return RTRUE;
    }
    
    // Handle BURN - destroys the painting's value (from ZIL MUNG behavior)
    if (g.prsa == V_BURN) {
        // Set TVALUE to 0 (worthless)
        g.prso->setProperty(P_TVALUE, 0);
        printLine("Congratulations! Unlike the other vandals, who merely stole the artist's masterpieces, you have destroyed one.");
        return RTRUE;
    }
    
    return RFALSE;
}

// Forward declaration for BAD-EGG helper
void badEgg();

// Egg action - special treasure that can be opened, contains canary
// Based on EGG-OBJECT from 1actions.zil
bool eggAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::EGG) {
        return RFALSE;
    }
    
    // Handle OPEN with or without tool
    if (g.prsa == V_OPEN) {
        // Check if already open
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The egg is already open.");
            return RTRUE;
        }
        
        // Check if using a tool
        if (!g.prsi) {
            printLine("You have neither the tools nor the expertise.");
            return RTRUE;
        }
        
        // Check if using hands (no tool)
        if (g.prsi->getId() == ObjectIds::ADVENTURER) {
            printLine("I doubt you could do that without damaging it.");
            return RTRUE;
        }
        
        // Check if using a weapon or tool - damages the egg
        if (g.prsi->hasFlag(ObjectFlag::WEAPONBIT) || g.prsi->hasFlag(ObjectFlag::TOOLBIT)) {
            printLine("The egg is now open, but the clumsiness of your attempt has seriously compromised its esthetic appeal.");
            badEgg();
            return RTRUE;
        }
        
        // Using something else - original response
        printLine("The concept of using a " + g.prsi->getDesc() + " is certainly original.");
        return RTRUE;
    }
    
    // Handle CLIMB-ON or sitting on the egg - breaks it
    if (g.prsa == V_CLIMB_ON) {
        printLine("There is a noticeable crunch from beneath you, and inspection reveals that the egg is lying open, badly damaged.");
        badEgg();
        return RTRUE;
    }
    
    // Handle THROW - breaks the egg
    if (g.prsa == V_THROW) {
        g.prso->moveTo(g.here);  // Move egg to current room
        printLine("Your rather indelicate handling of the egg has caused it some damage, although you have succeeded in opening it.");
        badEgg();
        return RTRUE;
    }
    
    return RFALSE;
}

// Helper function to convert egg to broken egg (BAD-EGG from ZIL)
void badEgg() {
    auto& g = Globals::instance();
    
    ZObject* egg = g.getObject(ObjectIds::EGG);
    ZObject* brokenEgg = g.getObject(ObjectIds::BROKEN_EGG);
    ZObject* canary = g.getObject(ObjectIds::CANARY);
    ZObject* brokenCanary = g.getObject(ObjectIds::BROKEN_CANARY);
    
    if (!egg || !brokenEgg) return;
    
    // Get egg's location
    ZObject* eggLocation = egg->getLocation();
    
    // If canary was in the egg, show the broken canary description
    if (canary && canary->getLocation() == egg) {
        // Move broken canary to broken egg
        if (brokenCanary) {
            brokenCanary->moveTo(brokenEgg);
            print(" ");
            printLine("There is a golden clockwork canary nestled in the egg. It seems to have recently had a bad experience. The mountings for its jewel-like eyes are empty, and its silver beak is crumpled.");
        }
        // Remove the good canary
        canary->moveTo(nullptr);
    } else {
        // No canary in egg, remove broken canary from game
        if (brokenCanary) {
            brokenCanary->moveTo(nullptr);
        }
    }
    
    // Move broken egg to egg's location
    if (eggLocation) {
        brokenEgg->moveTo(eggLocation);
    }
    
    // Remove the good egg from the game
    egg->moveTo(nullptr);
}

// Canary action - can be wound to produce bauble
// Based on CANARY-OBJECT from 1actions.zil
bool canaryAction() {
    auto& g = Globals::instance();
    
    // Handle both good and broken canary
    bool isCanary = g.prso && g.prso->getId() == ObjectIds::CANARY;
    bool isBrokenCanary = g.prso && g.prso->getId() == ObjectIds::BROKEN_CANARY;
    
    if (!isCanary && !isBrokenCanary) {
        return RFALSE;
    }
    
    // Handle WIND verb (winding the canary)
    // Note: V_WIND would need to be added to verbs.h, using V_TURN as substitute
    if (g.prsa == V_TURN) {
        if (isCanary) {
            // Check if in forest area - produces bauble
            ZRoom* room = dynamic_cast<ZRoom*>(g.here);
            bool inForest = false;
            if (room) {
                ObjectId roomId = room->getId();
                inForest = (roomId == RoomIds::FOREST_1 || 
                           roomId == RoomIds::FOREST_2 || 
                           roomId == RoomIds::FOREST_3 ||
                           roomId == RoomIds::FOREST_PATH ||
                           roomId == RoomIds::UP_A_TREE);
            }
            
            // Check if bauble has already been produced
            ZObject* bauble = g.getObject(ObjectIds::BAUBLE);
            bool baubleProduced = bauble && bauble->getLocation() != nullptr;
            
            if (inForest && !baubleProduced) {
                printLine("The canary chirps, slightly off-key, an aria from a forgotten opera. From out of the greenery flies a lovely songbird. It perches on a limb just over your head and opens its beak to sing. As it does so a beautiful brass bauble drops from its mouth, bounces off the top of your head, and lands glimmering in the grass. As the canary winds down, the songbird flies away.");
                
                // Move bauble to current room (or forest path if in tree)
                if (bauble) {
                    if (g.here->getId() == RoomIds::UP_A_TREE) {
                        bauble->moveTo(g.getObject(RoomIds::FOREST_PATH));
                    } else {
                        bauble->moveTo(g.here);
                    }
                }
            } else {
                printLine("The canary chirps blithely, if somewhat tinnily, for a short time.");
            }
        } else {
            // Broken canary
            printLine("There is an unpleasant grinding noise from inside the canary.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// ============================================================================
// PUZZLE OBJECT ACTION HANDLERS (Task 45)
// ============================================================================

// Boat action - inflatable boat for water navigation
// Based on BOAT-OBJECT from 1actions.zil
// Requirements: 42, 83
bool boatAction() {
    auto& g = Globals::instance();
    
    // Handle all three boat states: inflatable, inflated, punctured
    bool isInflatable = g.prso && g.prso->getId() == ObjectIds::BOAT_INFLATABLE;
    bool isInflated = g.prso && g.prso->getId() == ObjectIds::BOAT_INFLATED;
    bool isPunctured = g.prso && g.prso->getId() == ObjectIds::BOAT_PUNCTURED;
    
    if (!isInflatable && !isInflated && !isPunctured) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        if (isInflatable) {
            printLine("It's a small rubber boat, currently deflated.");
        } else if (isInflated) {
            printLine("It's a small rubber boat, inflated and ready for use.");
        } else if (isPunctured) {
            printLine("It's a small rubber boat with a puncture. It's useless now.");
        }
        return RTRUE;
    }
    
    // Handle INFLATE
    if (g.prsa == V_INFLATE) {
        if (isInflatable) {
            // Check if player has pump
            ZObject* pump = g.getObject(ObjectIds::PUMP);
            if (!pump || pump->getLocation() != g.winner) {
                printLine("You need a pump to inflate the boat.");
                return RTRUE;
            }
            
            // Transform inflatable boat to inflated boat
            ZObject* inflatedBoat = g.getObject(ObjectIds::BOAT_INFLATED);
            if (inflatedBoat) {
                ZObject* location = g.prso->getLocation();
                inflatedBoat->moveTo(location);
                g.prso->moveTo(nullptr);  // Remove inflatable boat
                printLine("The boat inflates and appears seaworthy.");
            }
            return RTRUE;
        } else if (isInflated) {
            printLine("The boat is already inflated.");
            return RTRUE;
        } else if (isPunctured) {
            printLine("The boat has a puncture and won't hold air.");
            return RTRUE;
        }
    }
    
    // Handle DEFLATE
    if (g.prsa == V_DEFLATE) {
        if (isInflated) {
            // Check if player has pump
            ZObject* pump = g.getObject(ObjectIds::PUMP);
            if (!pump || pump->getLocation() != g.winner) {
                printLine("You need a pump to deflate the boat.");
                return RTRUE;
            }
            
            // Check if player is in the boat
            if (g.winner->getLocation() == g.prso) {
                printLine("You can't deflate the boat while you're in it!");
                return RTRUE;
            }
            
            // Transform inflated boat back to inflatable boat
            ZObject* inflatableBoat = g.getObject(ObjectIds::BOAT_INFLATABLE);
            if (inflatableBoat) {
                ZObject* location = g.prso->getLocation();
                inflatableBoat->moveTo(location);
                g.prso->moveTo(nullptr);  // Remove inflated boat
                printLine("The boat deflates.");
            }
            return RTRUE;
        } else if (isInflatable) {
            printLine("The boat is already deflated.");
            return RTRUE;
        } else if (isPunctured) {
            printLine("The boat is already deflated and punctured.");
            return RTRUE;
        }
    }
    
    // Handle BOARD/ENTER - get into the boat
    if (g.prsa == V_BOARD || g.prsa == V_ENTER) {
        if (isInflated) {
            // Check if boat is in a water room (reservoir, river, etc.)
            ZRoom* room = dynamic_cast<ZRoom*>(g.prso->getLocation());
            if (!room) {
                printLine("The boat is not in water.");
                return RTRUE;
            }
            
            // Check if room is a water location by ID
            ObjectId roomId = room->getId();
            bool isWaterRoom = (roomId == RoomIds::RESERVOIR_NORTH || 
                               roomId == RoomIds::RESERVOIR_SOUTH ||
                               roomId == RoomIds::RESERVOIR ||
                               roomId == RoomIds::STREAM_VIEW ||
                               roomId == RoomIds::IN_STREAM ||
                               roomId == RoomIds::RIVER_1 ||
                               roomId == RoomIds::RIVER_2 ||
                               roomId == RoomIds::RIVER_3 ||
                               roomId == RoomIds::RIVER_4 ||
                               roomId == RoomIds::RIVER_5);
            
            if (!isWaterRoom) {
                printLine("The boat is not in water.");
                return RTRUE;
            }
            
            // Move player into boat
            g.winner->moveTo(g.prso);
            printLine("You are now in the boat.");
            return RTRUE;
        } else if (isInflatable) {
            printLine("You need to inflate the boat first.");
            return RTRUE;
        } else if (isPunctured) {
            printLine("The boat is punctured and won't float.");
            return RTRUE;
        }
    }
    
    // Handle DISEMBARK/EXIT - get out of the boat
    if (g.prsa == V_DISEMBARK || g.prsa == V_EXIT) {
        if (g.winner->getLocation() == g.prso) {
            // Move player to boat's location (the room)
            ZObject* room = g.prso->getLocation();
            if (room) {
                g.winner->moveTo(room);
                printLine("You climb out of the boat.");
            }
            return RTRUE;
        } else {
            printLine("You're not in the boat.");
            return RTRUE;
        }
    }
    
    // Handle TAKE - can only take deflated boat
    if (g.prsa == V_TAKE) {
        if (isInflatable) {
            // Default take behavior is fine
            return RFALSE;
        } else if (isInflated) {
            printLine("The boat is too large to carry when inflated.");
            return RTRUE;
        } else if (isPunctured) {
            printLine("The punctured boat is useless. Leave it.");
            return RTRUE;
        }
    }
    
    return RFALSE;
}

// Pump action - air pump for inflating/deflating boat
// Based on PUMP-OBJECT from 1actions.zil
// Requirements: 42, 83
bool pumpAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::PUMP) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("It's a small hand-held air pump, suitable for inflating rubber objects.");
        return RTRUE;
    }
    
    // Handle INFLATE verb with pump
    if (g.prsa == V_INFLATE) {
        // Check if there's a boat to inflate
        ZObject* inflatableBoat = g.getObject(ObjectIds::BOAT_INFLATABLE);
        if (!inflatableBoat) {
            printLine("There's nothing here to inflate.");
            return RTRUE;
        }
        
        // Check if boat is accessible
        ZObject* boatLocation = inflatableBoat->getLocation();
        if (boatLocation != g.here && boatLocation != g.winner) {
            printLine("There's nothing here to inflate.");
            return RTRUE;
        }
        
        // Delegate to boat action
        g.prso = inflatableBoat;
        return boatAction();
    }
    
    // Handle DEFLATE verb with pump
    if (g.prsa == V_DEFLATE) {
        // Check if there's an inflated boat to deflate
        ZObject* inflatedBoat = g.getObject(ObjectIds::BOAT_INFLATED);
        if (!inflatedBoat) {
            printLine("There's nothing here to deflate.");
            return RTRUE;
        }
        
        // Check if boat is accessible
        ZObject* boatLocation = inflatedBoat->getLocation();
        if (boatLocation != g.here && boatLocation != g.winner) {
            printLine("There's nothing here to deflate.");
            return RTRUE;
        }
        
        // Delegate to boat action
        g.prso = inflatedBoat;
        return boatAction();
    }
    
    return RFALSE;
}

// Machine action - complex button/switch puzzle
// Based on MACHINE-OBJECT from 1actions.zil
// Requirements: 42
bool machineAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::MACHINE) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("The machine is a large metal contraption with various buttons, switches, and levers. It appears to be some kind of control panel.");
        return RTRUE;
    }
    
    // Handle TURN/PUSH/PULL - interact with machine
    if (g.prsa == V_TURN || g.prsa == V_PUSH || g.prsa == V_PULL) {
        printLine("The machine makes a grinding noise but nothing else happens.");
        return RTRUE;
    }
    
    // Handle OPEN - machine can be opened to reveal contents
    if (g.prsa == V_OPEN) {
        if (g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The machine is already open.");
        } else {
            g.prso->setFlag(ObjectFlag::OPENBIT);
            printLine("You open the machine's access panel.");
        }
        return RTRUE;
    }
    
    // Handle CLOSE
    if (g.prsa == V_CLOSE) {
        if (!g.prso->hasFlag(ObjectFlag::OPENBIT)) {
            printLine("The machine is already closed.");
        } else {
            g.prso->clearFlag(ObjectFlag::OPENBIT);
            printLine("You close the machine's access panel.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Mirror action - shows different reflections
// Based on MIRROR-OBJECT from 1actions.zil
// Requirements: 42
bool mirrorAction() {
    auto& g = Globals::instance();
    
    // Handle both mirror objects
    bool isMirror1 = g.prso && g.prso->getId() == ObjectIds::MIRROR_1;
    bool isMirror2 = g.prso && g.prso->getId() == ObjectIds::MIRROR_2;
    
    if (!isMirror1 && !isMirror2) {
        return RFALSE;
    }
    
    // Handle EXAMINE - show reflection
    if (g.prsa == V_EXAMINE) {
        // Check if player has lamp on
        ZObject* lamp = g.getObject(ObjectIds::LAMP);
        bool hasLight = lamp && lamp->hasFlag(ObjectFlag::ONBIT) && 
                       (lamp->getLocation() == g.winner || lamp->getLocation() == g.here);
        
        if (!hasLight && !g.lit) {
            printLine("It's too dark to see your reflection.");
            return RTRUE;
        }
        
        // Different reflections for different mirrors
        if (isMirror1) {
            printLine("You see yourself in the mirror. You look tired and dirty from your adventures.");
        } else {
            printLine("You see yourself in the mirror, but something seems odd about the reflection.");
        }
        return RTRUE;
    }
    
    // Handle TAKE - mirrors are anchored
    if (g.prsa == V_TAKE) {
        printLine("The mirror is firmly attached to the wall.");
        return RTRUE;
    }
    
    // Handle ATTACK - breaking the mirror
    if (g.prsa == V_ATTACK) {
        printLine("You have a nagging feeling that breaking the mirror would be bad luck.");
        return RTRUE;
    }
    
    return RFALSE;
}

// Dam action - controls water flow
// Based on DAM-OBJECT from 1actions.zil
// Requirements: 42
bool damAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::DAM) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("The dam is a massive concrete structure. There is a control panel with various switches and buttons.");
        return RTRUE;
    }
    
    // Handle OPEN - opening the dam gates
    if (g.prsa == V_OPEN) {
        printLine("The dam gates are controlled by the panel in the maintenance room.");
        return RTRUE;
    }
    
    return RFALSE;
}

// Bolt action - part of dam puzzle
// Requirements: 42
bool boltAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::BOLT) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("It's a large metal bolt, part of the dam's control mechanism.");
        return RTRUE;
    }
    
    // Handle TURN - turning the bolt with wrench
    if (g.prsa == V_TURN) {
        // Check if player has wrench
        ZObject* wrench = g.getObject(ObjectIds::WRENCH);
        if (!wrench || wrench->getLocation() != g.winner) {
            printLine("You need a wrench to turn the bolt.");
            return RTRUE;
        }
        
        printLine("You turn the bolt with the wrench. Something clicks inside the mechanism.");
        return RTRUE;
    }
    
    return RFALSE;
}

// Bubble action - green bubble puzzle
// Requirements: 42
bool bubbleAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::BUBBLE) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("It's a large green bubble, shimmering and translucent.");
        return RTRUE;
    }
    
    // Handle TAKE
    if (g.prsa == V_TAKE) {
        printLine("The bubble is too fragile to take.");
        return RTRUE;
    }
    
    // Handle ATTACK/BREAK - popping the bubble
    if (g.prsa == V_ATTACK) {
        printLine("The bubble pops with a soft sound and disappears.");
        g.prso->moveTo(nullptr);  // Remove bubble from game
        return RTRUE;
    }
    
    return RFALSE;
}

// Button actions - machine control buttons
// Requirements: 42
bool buttonAction() {
    auto& g = Globals::instance();
    
    bool isYellow = g.prso && g.prso->getId() == ObjectIds::YELLOW_BUTTON;
    bool isBrown = g.prso && g.prso->getId() == ObjectIds::BROWN_BUTTON;
    bool isRed = g.prso && g.prso->getId() == ObjectIds::RED_BUTTON;
    bool isBlue = g.prso && g.prso->getId() == ObjectIds::BLUE_BUTTON;
    
    if (!isYellow && !isBrown && !isRed && !isBlue) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        if (isYellow) {
            printLine("A yellow button on the control panel.");
        } else if (isBrown) {
            printLine("A brown button on the control panel.");
        } else if (isRed) {
            printLine("A red button on the control panel.");
        } else if (isBlue) {
            printLine("A blue button on the control panel.");
        }
        return RTRUE;
    }
    
    // Handle PUSH - pressing the button
    if (g.prsa == V_PUSH) {
        if (isYellow) {
            printLine("You press the yellow button. A light flashes.");
        } else if (isBrown) {
            printLine("You press the brown button. You hear a grinding noise.");
        } else if (isRed) {
            printLine("You press the red button. A siren sounds briefly.");
        } else if (isBlue) {
            printLine("You press the blue button. Nothing obvious happens.");
        }
        return RTRUE;
    }
    
    return RFALSE;
}

// Gunk action - vitreous slag
// Requirements: 42
bool gunkAction() {
    auto& g = Globals::instance();
    
    if (!g.prso || g.prso->getId() != ObjectIds::GUNK) {
        return RFALSE;
    }
    
    // Handle EXAMINE
    if (g.prsa == V_EXAMINE) {
        printLine("It's a pile of vitreous slag, a glassy substance left over from some industrial process.");
        return RTRUE;
    }
    
    // Handle TAKE
    if (g.prsa == V_TAKE) {
        printLine("The gunk is too hot and sticky to handle.");
        return RTRUE;
    }
    
    return RFALSE;
}
