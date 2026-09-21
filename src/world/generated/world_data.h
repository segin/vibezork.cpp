// GENERATED FILE - DO NOT EDIT.
// Produced by scratchpad/worldgen/emit_cpp.py from zil/1dungeon.zil, zil/gglobals.zil.
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.
#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace zork::zil {

// One clause of a ROOM's exit list.  Field use per kind:
//   UEXIT  (DIR TO room)                                 to
//   NEXIT  (DIR "text")                                  text
//   CEXIT  (DIR TO room IF flag [ELSE "text"])           to, flag, text
//   DEXIT  (DIR TO room IF door IS OPEN [ELSE "text"])   to, door, text
//   FEXIT  (DIR PER routine)                             routine
enum class ExitKind : std::uint8_t { UEXIT, NEXIT, CEXIT, DEXIT, FEXIT };

struct ExitDef {
    std::string_view dir;       // DIRECTIONS name (NORTH ... LAND)
    ExitKind kind;
    std::string_view to{};      // target room
    std::string_view text{};    // NEXIT message or CEXIT/DEXIT ELSE message; empty = none
    std::string_view flag{};    // CEXIT global flag name
    std::string_view door{};    // DEXIT door object name
    std::string_view routine{}; // FEXIT routine name
    std::uint16_t line{};       // source line in 1dungeon.zil
};

// A vocabulary word: `text` exactly as written in the ZIL SYNONYM/ADJECTIVE
// clause, `key` the Z-machine v3 dictionary form (lower case, cut at six
// z-characters; A2 characters such as '#', '-' and digits cost two).  Register
// both: the key is what player input matches after the same truncation.
struct Word { std::string_view text; std::string_view key; };

struct PseudoDef { std::string_view word; std::string_view routine; };

// Game: a real room/object.  CompilerArtifact: GLOBAL-OBJECTS, LOCAL-GLOBALS,
// ROOMS - exist to allocate flags/properties and to parent globals; never
// visible in play.  Parser: IT, INTNUM, NOT-HERE-OBJECT, PSEUDO-OBJECT.
enum class ObjRole : std::uint8_t { Game, CompilerArtifact, Parser };

using OptInt = std::optional<std::int16_t>;   // nullopt = property absent in the source

struct RoomDef {
    std::uint16_t def_index;                    // position in ZIL definition order (shared with ObjectDef)
    std::string_view name;
    std::string_view desc{};
    std::string_view ldesc{};
    std::span<const std::string_view> flags{};
    OptInt value{};
    std::string_view action{};
    std::span<const std::string_view> globals{};
    std::span<const PseudoDef> pseudo{};
    std::span<const ExitDef> exits{};
    std::uint16_t line{};
};

struct ObjectDef {
    std::uint16_t def_index;
    std::string_view name;
    ObjRole role;
    std::string_view in{};                      // parent object name; empty = no IN clause
    std::span<const Word> synonyms{};
    std::span<const Word> adjectives{};
    std::string_view desc{};
    std::string_view fdesc{};
    std::string_view ldesc{};
    std::string_view text{};
    std::span<const std::string_view> flags{};
    OptInt size{};                              // absent -> PROPDEF default (kPropDefaults)
    OptInt capacity{};
    OptInt value{};
    OptInt tvalue{};
    OptInt strength{};
    std::string_view vtype{};                   // vehicle flag name, e.g. NONLANDBIT (LOCAL-GLOBALS writes the literal 1)
    std::string_view action{};                  // empty for absent and for "(ACTION 0)"
    std::string_view descfcn{};
    std::string_view contfcn{};                 // (ADVFCN is omitted: it occurs once, as 0, on LOCAL-GLOBALS)
    std::span<const std::string_view> globals{};    // only LOCAL-GLOBALS uses it
    std::span<const PseudoDef> pseudo{};            // only LOCAL-GLOBALS uses it
    std::span<const ExitDef> exits{};               // only ROOMS "(IN TO ROOMS)" uses it
    std::string_view file{};
    std::uint16_t line{};
};

struct GlobalDef { std::string_view name; std::int16_t value; };          // <> is stored as 0
struct WalkTable { std::string_view name; std::span<const std::string_view> rooms; };
struct PropDefault { std::string_view prop; std::int16_t value; };

extern const std::span<const std::string_view> kDirections;    // <DIRECTIONS ...> order
extern const std::span<const RoomDef> kRooms;                  // definition order
extern const std::span<const ObjectDef> kObjects;              // definition order, 1dungeon.zil then gglobals.zil
// Object tree order: zork1.z3 links each parent's children in REVERSE definition
// order (verified for every multi-child parent, ROOMS/GLOBAL-OBJECTS/LOCAL-GLOBALS
// included).  To reproduce FIRST?/NEXT? walks ("take all", thief, room
// descriptions) insert each definition at the FRONT of its parent's chain while
// walking kRooms/kObjects merged by def_index.
extern const std::span<const GlobalDef> kGlobals;              // scalar GLOBALs of 1dungeon.zil (flags, SCORE-MAX)
extern const std::span<const WalkTable> kWalkTables;           // HOUSE-AROUND, FOREST-AROUND, IN-HOUSE-AROUND, ABOVE-GROUND
extern const std::span<const PropDefault> kPropDefaults;       // zork1.zil PROPDEFs
extern const std::span<const Word> kVocabulary;                // every distinct SYNONYM/ADJECTIVE word
extern const std::span<const std::string_view> kFlagNames;     // every flag name used, first-use order
extern const std::span<const std::string_view> kRoutineNames;  // every routine name referenced by the data

inline constexpr std::size_t kRoomCount = 110;
inline constexpr std::size_t kObjectCount = 140;
inline constexpr std::size_t kDefinitionCount = kRoomCount + kObjectCount;

}  // namespace zork::zil
