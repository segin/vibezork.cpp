// GENERATED FILE - DO NOT EDIT.
// Produced by tools/worldgen/melee_tables.py from zil/1actions.zil, zil/gverbs.zil, zil/1dungeon.zil (GO fix-ups).
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.

#pragma once
#include <cstdint>
#include <span>
#include <string_view>

namespace zork::zil {

// ---- fight-state slot indexes (F-*) and other constants, values as in 1actions.zil
inline constexpr int kF_BUSY_Q = 1;
inline constexpr int kF_DEAD = 2;
inline constexpr int kF_UNCONSCIOUS = 3;
inline constexpr int kF_CONSCIOUS = 4;
inline constexpr int kF_FIRST_Q = 5;
inline constexpr int kSTRENGTH_MAX = 7;
inline constexpr int kSTRENGTH_MIN = 2;
inline constexpr int kCURE_WAIT = 30;
inline constexpr int kF_WEP = 0;
inline constexpr int kF_DEF = 1;
inline constexpr int kV_VILLAIN = 0;
inline constexpr int kV_BEST = 1;
inline constexpr int kV_BEST_ADV = 2;
inline constexpr int kV_PROB = 3;
inline constexpr int kV_MSGS = 4;

// ---- blow results (1actions.zil "blow results")
enum class BlowResult : std::uint8_t {
    MISSED = 1,
    UNCONSCIOUS = 2,
    KILLED = 3,
    LIGHT_WOUND = 4,
    SERIOUS_WOUND = 5,
    STAGGER = 6,
    LOSE_WEAPON = 7,
    HESITATE = 8,
    SITTING_DUCK = 9,
};

// ---- DEFn tables: TABLE (PURE) of blow results, indexed by the melee dice roll.
inline constexpr BlowResult kDEF1[] = {BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::UNCONSCIOUS, BlowResult::UNCONSCIOUS, BlowResult::KILLED, BlowResult::KILLED, BlowResult::KILLED, BlowResult::KILLED, BlowResult::KILLED};  // 1actions.zil:3258
inline constexpr BlowResult kDEF2A[] = {BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::UNCONSCIOUS};  // 1actions.zil:3265
inline constexpr BlowResult kDEF2B[] = {BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::UNCONSCIOUS, BlowResult::KILLED, BlowResult::KILLED, BlowResult::KILLED};  // 1actions.zil:3272
inline constexpr BlowResult kDEF3A[] = {BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::SERIOUS_WOUND, BlowResult::SERIOUS_WOUND};  // 1actions.zil:3280
inline constexpr BlowResult kDEF3B[] = {BlowResult::MISSED, BlowResult::MISSED, BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::SERIOUS_WOUND, BlowResult::SERIOUS_WOUND, BlowResult::SERIOUS_WOUND};  // 1actions.zil:3287
inline constexpr BlowResult kDEF3C[] = {BlowResult::MISSED, BlowResult::STAGGER, BlowResult::STAGGER, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::LIGHT_WOUND, BlowResult::SERIOUS_WOUND, BlowResult::SERIOUS_WOUND, BlowResult::SERIOUS_WOUND};  // 1actions.zil:3294

// ---- DEFn-RES: rows of table slices selected by defender strength.  The ZIL source
// holds 0 placeholders that GO (1dungeon.zil) patches with <REST ,DEFx n>; the
// resolved form is given here: {table name, first element index}.  An empty name
// is a slot GO never fills (stays 0 in the story).
struct DefSlice { std::string_view table; int offset; };
inline constexpr DefSlice kDEF1_RES[] = {{"DEF1", 0}, {"DEF1", 1}, {"DEF1", 2}};  // 1actions.zil:3301
inline constexpr DefSlice kDEF2_RES[] = {{"DEF2A", 0}, {"DEF2B", 0}, {"DEF2B", 1}, {"DEF2B", 2}};  // 1actions.zil:3306
inline constexpr DefSlice kDEF3_RES[] = {{"DEF3A", 0}, {"DEF3A", 1}, {"DEF3B", 0}, {"DEF3B", 1}, {"DEF3C", 0}};  // 1actions.zil:3312

// ---- melee message tables: TABLE (PURE) of result rows (element index i = ZIL
// <GET tbl i>, i.e. blow result - 1); each row is an LTABLE of messages; each
// message is an LTABLE of parts printed in order: text, F-WEP (the weapon name),
// or F-DEF (the defender name).
struct MeleePart { enum Kind : std::uint8_t { Text, Weapon, Defender }; std::string_view text; Kind kind; };
using MeleeMsg = std::span<const MeleePart>;
using MeleeResult = std::span<const MeleeMsg>;
extern const std::span<const MeleeResult> HERO_MELEE;  // 1actions.zil:3611, 7 result rows
extern const std::span<const MeleeResult> CYCLOPS_MELEE;  // 1actions.zil:3654, 9 result rows
extern const std::span<const MeleeResult> TROLL_MELEE;  // 1actions.zil:3689, 9 result rows
extern const std::span<const MeleeResult> THIEF_MELEE;  // 1actions.zil:3735, 9 result rows

// ---- VILLAINS: LTABLE of <TABLE villain best-weapon best-adv prob msgs> (1actions.zil)
struct VillainDef { std::string_view villain; std::string_view best_weapon; int best_adv; int prob; std::string_view msgs; };
inline constexpr VillainDef kVILLAINS[] = {{"TROLL", "SWORD", 1, 0, "TROLL-MELEE"}, {"THIEF", "KNIFE", 1, 0, "THIEF-MELEE"}, {"CYCLOPS", {}, 0, 0, "CYCLOPS-MELEE"}};  // 1actions.zil:3801

// ---- message tables.  For <LTABLE 0 ...> (PICK-ONE tables) the leading 0 cursor
// slot is omitted; only the messages are listed, in ZIL order.
inline constexpr std::string_view kDROWNINGS[] = {"up to your ankles.", "up to your shin.", "up to your knees.", "up to your hips.", "up to your waist.", "up to your chest.", "up to your neck.", "over your head.", "high in your lungs."};  // 1actions.zil:1284 TABLE
inline constexpr std::string_view kCYCLOMAD[] = {"The cyclops seems somewhat agitated.", "The cyclops appears to be getting more agitated.", "The cyclops is moving about the room, looking for something.", "The cyclops was looking for salt and pepper. No doubt they are condiments for his upcoming snack.", "The cyclops is moving toward you in an unfriendly manner.", "You have two choices: 1. Leave  2. Become dinner."};  // 1actions.zil:1644 TABLE
inline constexpr std::string_view kBDIGS[] = {"You seem to be digging a hole here.", "The hole is getting deeper, but that's about it.", "You are surrounded by a wall of sand on all sides."};  // 1actions.zil:2871 TABLE
inline constexpr std::string_view kJUMPLOSS[] = {"You should have looked before you leaped.", "In the movies, your life would be passing before your eyes.", "Geronimo..."};  // gverbs.zil:844 LTABLE (PICK-ONE)
inline constexpr std::string_view kWHEEEEE[] = {"Very good. Now you can go to the second grade.", "Are you enjoying yourself?", "Wheeeeeeeeee!!!!!", "Do you expect me to applaud?"};  // gverbs.zil:1272 LTABLE (PICK-ONE)
inline constexpr std::string_view kHO_HUM[] = {" doesn't seem to work.", " isn't notably helpful.", " has no effect."};  // gverbs.zil:2031 LTABLE (PICK-ONE)
inline constexpr std::string_view kHELLOS[] = {"Hello.", "Good day.", "Nice weather we've been having lately.", "Goodbye."};  // gverbs.zil:2198 LTABLE (PICK-ONE)
inline constexpr std::string_view kYUKS[] = {"A valiant attempt.", "You can't be serious.", "An interesting idea...", "What a concept!"};  // gverbs.zil:2204 LTABLE (PICK-ONE)
inline constexpr std::string_view kDUMMY[] = {"Look around.", "Too late for that.", "Have your eyes checked."};  // gverbs.zil:2213 LTABLE (PICK-ONE)
inline constexpr std::string_view kSWIMYUKS[] = {"You can't swim in the dungeon."};  // gverbs.zil:2195 LTABLE (PICK-ONE)

// ---- room lists (object names)
inline constexpr std::string_view kBAT_DROPS[] = {"MINE-1", "MINE-2", "MINE-3", "MINE-4", "LADDER-TOP", "LADDER-BOTTOM", "SQUEEKY-ROOM", "MINE-ENTRANCE"};  // 1actions.zil:332 LTABLE (leading 0 slot omitted)
inline constexpr std::string_view kLOUD_RUNS[] = {"DAMP-CAVE", "ROUND-ROOM", "DEEP-CANYON"};  // 1actions.zil:1261 LTABLE (leading 0 slot omitted)
inline constexpr std::string_view kRIVER_NEXT[] = {"RIVER-1", "RIVER-2", "RIVER-3", "RIVER-4", "RIVER-5"};  // 1actions.zil:2695 LTABLE

// ---- light-source timers: (turns-remaining, message) steps, 0 terminated as in ZIL
struct TimerStep { int turns; std::string_view text; };
inline constexpr TimerStep kLAMP_TABLE[] = {{100, "The lamp appears a bit dimmer."}, {70, "The lamp is definitely dimmer now."}, {15, "The lamp is nearly out."}, {0, {}}};  // 1actions.zil:2220
inline constexpr TimerStep kCANDLE_TABLE[] = {{20, "The candles grow shorter."}, {10, "The candles are becoming quite short."}, {5, "The candles won't last long now."}, {0, {}}};  // 1actions.zil:2406

// ---- river tables
struct RoomInt { std::string_view room; int value; };
struct RoomPair { std::string_view from; std::string_view to; };
inline constexpr RoomInt kRIVER_SPEEDS[] = {{"RIVER-1", 4}, {"RIVER-2", 4}, {"RIVER-3", 3}, {"RIVER-4", 2}, {"RIVER-5", 1}};  // 1actions.zil:2692
inline constexpr RoomPair kRIVER_LAUNCH[] = {{"DAM-BASE", "RIVER-1"}, {"WHITE-CLIFFS-NORTH", "RIVER-3"}, {"WHITE-CLIFFS-SOUTH", "RIVER-4"}, {"SHORE", "RIVER-5"}, {"SANDY-BEACH", "RIVER-4"}, {"RESERVOIR-SOUTH", "RESERVOIR"}, {"RESERVOIR-NORTH", "RESERVOIR"}, {"STREAM-VIEW", "IN-STREAM"}};  // 1actions.zil:2698

}  // namespace zork::zil
