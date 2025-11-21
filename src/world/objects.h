#pragma once
#include "core/types.h"

// Object ID Constants
// Organized by type with numeric ranges to group related objects
// Based on 1dungeon.zil from Zork I

namespace ObjectIds {

// ============================================================================
// TREASURES (100-199)
// Objects with VALUE/TVALUE properties that can be scored
// ============================================================================

constexpr ObjectId TROPHY = 100;
constexpr ObjectId EGG = 101;
constexpr ObjectId BROKEN_EGG = 102;
constexpr ObjectId CHALICE = 103;
constexpr ObjectId TRIDENT = 104;
constexpr ObjectId PAINTING = 105;
constexpr ObjectId COFFIN = 106;  // Also a container
constexpr ObjectId JEWELS = 107;  // Trunk of jewels
constexpr ObjectId COINS = 108;   // Bag of coins
constexpr ObjectId DIAMOND = 109;
constexpr ObjectId JADE = 110;    // Jade figurine
constexpr ObjectId EMERALD = 111;
constexpr ObjectId BRACELET = 112; // Sapphire bracelet
constexpr ObjectId BAR = 113;      // Platinum bar
constexpr ObjectId POT_OF_GOLD = 114;
constexpr ObjectId SCARAB = 115;
constexpr ObjectId TORCH = 116;    // Ivory torch (also light source)
constexpr ObjectId SKULL = 117;    // Crystal skull
constexpr ObjectId SCEPTRE = 118;  // Egyptian sceptre (also weapon)
constexpr ObjectId CANARY = 119;   // Clockwork canary
constexpr ObjectId BROKEN_CANARY = 120;
constexpr ObjectId BAUBLE = 121;   // Brass bauble

// ============================================================================
// TOOLS (200-249)
// Useful items for solving puzzles
// ============================================================================

constexpr ObjectId SWORD = 200;
constexpr ObjectId LAMP = 201;
constexpr ObjectId ROPE = 202;
constexpr ObjectId KNIFE = 203;
constexpr ObjectId RUSTY_KNIFE = 204;
constexpr ObjectId WRENCH = 205;
constexpr ObjectId SCREWDRIVER = 206;
constexpr ObjectId SHOVEL = 207;
constexpr ObjectId PUMP = 208;        // Air pump
constexpr ObjectId KEYS = 209;        // Skeleton key
constexpr ObjectId MATCH = 210;       // Matchbook
constexpr ObjectId CANDLES = 211;     // Pair of candles (also light)
constexpr ObjectId PUTTY = 212;       // Viscous material
constexpr ObjectId TUBE = 213;        // Tube containing putty

// ============================================================================
// CONTAINERS (300-349)
// Objects that can hold other objects
// ============================================================================

constexpr ObjectId MAILBOX = 300;
constexpr ObjectId TROPHY_CASE = 301;
constexpr ObjectId BASKET = 302;      // Lowered basket
constexpr ObjectId RAISED_BASKET = 303;
constexpr ObjectId SACK = 304;        // Brown sack/sandwich bag
constexpr ObjectId BAG = 305;         // Large bag (thief's)
constexpr ObjectId BOTTLE = 306;
constexpr ObjectId BUOY = 307;        // Red buoy
constexpr ObjectId NEST = 308;        // Bird's nest
constexpr ObjectId TOOL_CHEST = 309;
constexpr ObjectId ALTAR = 310;
constexpr ObjectId MACHINE = 311;     // Also special puzzle object
constexpr ObjectId PEDESTAL = 312;

// ============================================================================
// NPCs (400-449)
// Non-player characters and creatures
// ============================================================================

constexpr ObjectId THIEF = 400;
constexpr ObjectId TROLL = 401;
constexpr ObjectId CYCLOPS = 402;
constexpr ObjectId GRUE = 403;        // For darkness attacks
constexpr ObjectId BAT = 404;
constexpr ObjectId GHOSTS = 405;

// ============================================================================
// SCENERY (500-599)
// Non-takeable environmental objects
// ============================================================================

constexpr ObjectId WHITE_HOUSE = 500;
constexpr ObjectId BOARD = 501;
constexpr ObjectId WINDOW = 502;
constexpr ObjectId BOARDED_WINDOW = 503;
constexpr ObjectId KITCHEN_WINDOW = 504;
constexpr ObjectId FRONT_DOOR = 505;
constexpr ObjectId WOODEN_DOOR = 506;
constexpr ObjectId BARROW_DOOR = 507;
constexpr ObjectId TRAP_DOOR = 508;
constexpr ObjectId TREE = 509;
constexpr ObjectId FOREST = 510;
constexpr ObjectId MOUNTAIN_RANGE = 511;
constexpr ObjectId CHIMNEY = 512;
constexpr ObjectId SLIDE = 513;
constexpr ObjectId LADDER = 514;
constexpr ObjectId STAIRS = 515;
constexpr ObjectId RUG = 516;
constexpr ObjectId KITCHEN_TABLE = 517;
constexpr ObjectId ATTIC_TABLE = 518;
constexpr ObjectId RAILING = 519;
constexpr ObjectId WALL = 520;
constexpr ObjectId GRANITE_WALL = 521;
constexpr ObjectId CRACK = 522;
constexpr ObjectId GRATE = 523;
constexpr ObjectId BARROW = 524;      // Stone barrow
constexpr ObjectId SONGBIRD = 525;
constexpr ObjectId TEETH = 526;       // Overboard teeth
constexpr ObjectId RAINBOW = 527;
constexpr ObjectId RIVER = 528;
constexpr ObjectId GLOBAL_WATER = 529;
constexpr ObjectId SAND = 530;
constexpr ObjectId BODIES = 531;      // Pile of bodies
constexpr ObjectId BONES = 532;       // Skeleton
constexpr ObjectId LEAVES = 533;      // Pile of leaves
constexpr ObjectId COAL = 534;        // Pile of coal
constexpr ObjectId TIMBERS = 535;     // Broken timbers
constexpr ObjectId ENGRAVINGS = 536;
constexpr ObjectId PRAYER = 537;
constexpr ObjectId CLIMBABLE_CLIFF = 538;
constexpr ObjectId WHITE_CLIFF = 539;
constexpr ObjectId CONTROL_PANEL = 540;

// ============================================================================
// SPECIAL PUZZLE OBJECTS (600-649)
// Objects with unique puzzle mechanics
// ============================================================================

constexpr ObjectId BOAT_INFLATED = 600;
constexpr ObjectId BOAT_PUNCTURED = 601;
constexpr ObjectId BOAT_INFLATABLE = 602;
constexpr ObjectId MIRROR_1 = 603;
constexpr ObjectId MIRROR_2 = 604;
constexpr ObjectId DAM = 605;
constexpr ObjectId BOLT = 606;        // Metal bolt
constexpr ObjectId BUBBLE = 607;      // Green bubble
constexpr ObjectId LEAK = 608;
constexpr ObjectId MACHINE_SWITCH = 609;
constexpr ObjectId YELLOW_BUTTON = 610;
constexpr ObjectId BROWN_BUTTON = 611;
constexpr ObjectId RED_BUTTON = 612;
constexpr ObjectId BLUE_BUTTON = 613;
constexpr ObjectId GUNK = 614;        // Vitreous slag

// ============================================================================
// WEAPONS (700-719)
// Objects used in combat (some overlap with tools/treasures)
// ============================================================================

constexpr ObjectId AXE = 700;         // Bloody axe (troll's)
constexpr ObjectId STILETTO = 701;    // Thief's weapon

// ============================================================================
// LIGHT SOURCES (720-739)
// Objects that provide illumination
// ============================================================================

constexpr ObjectId BROKEN_LAMP = 720;
constexpr ObjectId BURNED_OUT_LANTERN = 721;

// ============================================================================
// FOOD AND DRINK (740-759)
// Consumable items
// ============================================================================

constexpr ObjectId LUNCH = 740;       // Hot pepper sandwich
constexpr ObjectId GARLIC = 741;
constexpr ObjectId WATER = 742;       // Water in bottle

// ============================================================================
// READABLE OBJECTS (760-779)
// Objects with text that can be read
// ============================================================================

constexpr ObjectId ADVERTISEMENT = 760; // Leaflet
constexpr ObjectId BOOK = 761;          // Black book
constexpr ObjectId GUIDE = 762;         // Tour guidebook
constexpr ObjectId OWNERS_MANUAL = 763;
constexpr ObjectId MAP = 764;
constexpr ObjectId BOAT_LABEL = 765;

// ============================================================================
// BELLS (780-789)
// Bell objects with special states
// ============================================================================

constexpr ObjectId BELL = 780;
constexpr ObjectId HOT_BELL = 781;

// ============================================================================
// SPECIAL/MISC (800-899)
// Objects that don't fit other categories
// ============================================================================

constexpr ObjectId ADVENTURER = 800;  // Player object

} // namespace ObjectIds

// Legacy compatibility - keep old names for existing code
constexpr ObjectId OBJ_MAILBOX = ObjectIds::MAILBOX;
constexpr ObjectId OBJ_WHITE_HOUSE = ObjectIds::WHITE_HOUSE;
constexpr ObjectId OBJ_BOARD = ObjectIds::BOARD;
constexpr ObjectId OBJ_FOREST = ObjectIds::FOREST;
constexpr ObjectId OBJ_BOARDED_WINDOW = ObjectIds::BOARDED_WINDOW;
constexpr ObjectId OBJ_FRONT_DOOR = ObjectIds::FRONT_DOOR;
constexpr ObjectId OBJ_ADVENTURER = ObjectIds::ADVENTURER;
constexpr ObjectId OBJ_KITCHEN_WINDOW = ObjectIds::KITCHEN_WINDOW;

// Property IDs
constexpr PropertyId P_SIZE = 1;
constexpr PropertyId P_CAPACITY = 2;
constexpr PropertyId P_VALUE = 3;
constexpr PropertyId P_TVALUE = 4;
