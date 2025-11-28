#pragma once
#include "core/types.h"
#include "core/object.h"
#include <string>
#include <map>
#include <functional>

// Room ID Constants
// Organized by area with numeric ranges to group related rooms
namespace RoomIds {
    // Exterior Area (1000-1009)
    constexpr ObjectId WEST_OF_HOUSE = 1000;
    constexpr ObjectId NORTH_OF_HOUSE = 1001;
    constexpr ObjectId SOUTH_OF_HOUSE = 1002;
    constexpr ObjectId BEHIND_HOUSE = 1003;  // EAST_OF_HOUSE in ZIL
    constexpr ObjectId STONE_BARROW = 1004;
    constexpr ObjectId CLEARING = 1005;
    constexpr ObjectId FOREST_PATH = 1006;  // PATH in ZIL
    constexpr ObjectId GRATING_CLEARING = 1007;
    constexpr ObjectId CANYON_VIEW = 1008;
    
    // Forest Area (1010-1019)
    constexpr ObjectId FOREST_1 = 1010;
    constexpr ObjectId FOREST_2 = 1011;
    constexpr ObjectId FOREST_3 = 1012;
    constexpr ObjectId MOUNTAINS = 1013;
    constexpr ObjectId UP_A_TREE = 1014;
    
    // House Interior (1020-1039)
    constexpr ObjectId LIVING_ROOM = 1020;
    constexpr ObjectId KITCHEN = 1021;
    constexpr ObjectId ATTIC = 1022;
    constexpr ObjectId CELLAR = 1023;
    constexpr ObjectId GALLERY = 1024;
    constexpr ObjectId STUDIO = 1025;
    
    // Underground Passages (1040-1069)
    constexpr ObjectId TROLL_ROOM = 1040;
    constexpr ObjectId EAST_OF_CHASM = 1041;
    constexpr ObjectId EW_PASSAGE = 1042;  // East-West Passage
    constexpr ObjectId ROUND_ROOM = 1043;
    constexpr ObjectId LOUD_ROOM = 1044;
    constexpr ObjectId NS_PASSAGE = 1045;  // North-South Passage
    constexpr ObjectId CHASM_ROOM = 1046;
    constexpr ObjectId DEEP_CANYON = 1047;
    constexpr ObjectId DAMP_CAVE = 1048;
    constexpr ObjectId COLD_PASSAGE = 1049;
    constexpr ObjectId NARROW_PASSAGE = 1050;
    constexpr ObjectId SLIDE_ROOM = 1051;
    constexpr ObjectId MINE_ENTRANCE = 1052;
    constexpr ObjectId SQUEEKY_ROOM = 1053;
    constexpr ObjectId BAT_ROOM = 1054;
    constexpr ObjectId SHAFT_ROOM = 1055;
    constexpr ObjectId SMELLY_ROOM = 1056;
    constexpr ObjectId GAS_ROOM = 1057;
    constexpr ObjectId LADDER_TOP = 1058;
    constexpr ObjectId LADDER_BOTTOM = 1059;
    constexpr ObjectId TIMBER_ROOM = 1060;
    constexpr ObjectId LOWER_SHAFT = 1061;
    constexpr ObjectId MACHINE_ROOM = 1062;
    constexpr ObjectId DEAD_END_5 = 1063;
    
    // Coal Mine (1064-1069)
    constexpr ObjectId MINE_1 = 1064;
    constexpr ObjectId MINE_2 = 1065;
    constexpr ObjectId MINE_3 = 1066;
    constexpr ObjectId MINE_4 = 1067;
    
    // Maze (1070-1099)
    constexpr ObjectId MAZE_1 = 1070;
    constexpr ObjectId MAZE_2 = 1071;
    constexpr ObjectId MAZE_3 = 1072;
    constexpr ObjectId MAZE_4 = 1073;
    constexpr ObjectId MAZE_5 = 1074;
    constexpr ObjectId MAZE_6 = 1075;
    constexpr ObjectId MAZE_7 = 1076;
    constexpr ObjectId MAZE_8 = 1077;
    constexpr ObjectId MAZE_9 = 1078;
    constexpr ObjectId MAZE_10 = 1079;
    constexpr ObjectId MAZE_11 = 1080;
    constexpr ObjectId MAZE_12 = 1081;
    constexpr ObjectId MAZE_13 = 1082;
    constexpr ObjectId MAZE_14 = 1083;
    constexpr ObjectId MAZE_15 = 1084;
    constexpr ObjectId GRATING_ROOM = 1085;
    constexpr ObjectId DEAD_END_1 = 1086;
    constexpr ObjectId DEAD_END_2 = 1087;
    constexpr ObjectId DEAD_END_3 = 1088;
    constexpr ObjectId DEAD_END_4 = 1089;
    
    // Special Areas (1100-1139)
    constexpr ObjectId CYCLOPS_ROOM = 1100;
    constexpr ObjectId STRANGE_PASSAGE = 1101;
    constexpr ObjectId TREASURE_ROOM = 1102;
    constexpr ObjectId ENTRANCE_TO_HADES = 1103;
    constexpr ObjectId LAND_OF_LIVING_DEAD = 1104;
    constexpr ObjectId ENGRAVINGS_CAVE = 1105;
    constexpr ObjectId DOME_ROOM = 1106;
    constexpr ObjectId TORCH_ROOM = 1107;
    constexpr ObjectId NORTH_TEMPLE = 1108;
    constexpr ObjectId SOUTH_TEMPLE = 1109;
    constexpr ObjectId EGYPT_ROOM = 1110;
    
    // Mirror Rooms (1111-1119)
    constexpr ObjectId MIRROR_ROOM_1 = 1111;
    constexpr ObjectId MIRROR_ROOM_2 = 1112;
    constexpr ObjectId SMALL_CAVE = 1113;
    constexpr ObjectId TINY_CAVE = 1114;
    constexpr ObjectId TWISTING_PASSAGE = 1115;
    constexpr ObjectId WINDING_PASSAGE = 1116;
    
    // Reservoir and Dam Area (1120-1139)
    constexpr ObjectId RESERVOIR_SOUTH = 1120;
    constexpr ObjectId RESERVOIR = 1121;
    constexpr ObjectId RESERVOIR_NORTH = 1122;
    constexpr ObjectId STREAM_VIEW = 1123;
    constexpr ObjectId IN_STREAM = 1124;
    constexpr ObjectId ATLANTIS_ROOM = 1125;
    constexpr ObjectId DAM_ROOM = 1126;
    constexpr ObjectId DAM_LOBBY = 1127;
    constexpr ObjectId MAINTENANCE_ROOM = 1128;
    constexpr ObjectId DAM_BASE = 1129;
    
    // River Area (1130-1149)
    constexpr ObjectId RIVER_1 = 1130;
    constexpr ObjectId RIVER_2 = 1131;
    constexpr ObjectId RIVER_3 = 1132;
    constexpr ObjectId RIVER_4 = 1133;
    constexpr ObjectId RIVER_5 = 1134;
    constexpr ObjectId WHITE_CLIFFS_NORTH = 1135;
    constexpr ObjectId WHITE_CLIFFS_SOUTH = 1136;
    constexpr ObjectId SANDY_BEACH = 1137;
    constexpr ObjectId SANDY_CAVE = 1138;
    constexpr ObjectId SHORE = 1139;
    constexpr ObjectId ARAGAIN_FALLS = 1140;
    constexpr ObjectId ON_RAINBOW = 1141;
    constexpr ObjectId END_OF_RAINBOW = 1142;
    constexpr ObjectId CANYON_BOTTOM = 1143;
    constexpr ObjectId CLIFF_MIDDLE = 1144;
}

// Legacy compatibility - map old names to new namespace
constexpr ObjectId ROOM_WEST_OF_HOUSE = RoomIds::WEST_OF_HOUSE;
constexpr ObjectId ROOM_NORTH_OF_HOUSE = RoomIds::NORTH_OF_HOUSE;
constexpr ObjectId ROOM_SOUTH_OF_HOUSE = RoomIds::SOUTH_OF_HOUSE;
constexpr ObjectId ROOM_EAST_OF_HOUSE = RoomIds::BEHIND_HOUSE;
constexpr ObjectId ROOM_STONE_BARROW = RoomIds::STONE_BARROW;
constexpr ObjectId ROOM_FOREST_1 = RoomIds::FOREST_1;

// Direction enum
enum class Direction {
    NORTH, SOUTH, EAST, WEST,
    NE, NW, SE, SW,
    UP, DOWN, IN, OUT
};

// Exit type enumeration
enum class ExitType {
    NORMAL,        // Standard directional exit
    DOOR,          // Door that can be opened/closed/locked
    SPECIAL,       // Requires special verb (CLIMB, ENTER, etc.)
    CONDITIONAL,   // Requires game state condition
    ONE_WAY        // Can only travel in one direction
};

// Room exit structure
struct RoomExit {
    ObjectId targetRoom = 0;
    std::string message;  // For blocked exits
    std::function<bool()> condition;  // Optional condition
    ExitType type = ExitType::NORMAL;
    
    // Door-specific fields
    ObjectId doorObject = 0;  // Reference to door object (if type == DOOR)
    
    // Special movement fields
    int requiredVerb = 0;  // Verb ID required for special exits (CLIMB, ENTER, etc.)
    std::string specialMessage;  // Message when wrong verb is used
    
    RoomExit() = default;
    RoomExit(ObjectId target) : targetRoom(target) {}
    RoomExit(const std::string& msg) : message(msg) {}
    
    // Door exit constructor
    static RoomExit createDoor(ObjectId target, ObjectId door) {
        RoomExit exit;
        exit.targetRoom = target;
        exit.doorObject = door;
        exit.type = ExitType::DOOR;
        return exit;
    }
    
    // Special movement exit constructor
    static RoomExit createSpecial(ObjectId target, int verb, const std::string& msg = "") {
        RoomExit exit;
        exit.targetRoom = target;
        exit.requiredVerb = verb;
        exit.type = ExitType::SPECIAL;
        exit.specialMessage = msg;
        return exit;
    }
    
    // Conditional exit constructor
    static RoomExit createConditional(ObjectId target, std::function<bool()> cond, const std::string& msg = "") {
        RoomExit exit;
        exit.targetRoom = target;
        exit.condition = cond;
        exit.type = ExitType::CONDITIONAL;
        exit.message = msg;
        return exit;
    }
    
    // One-way exit constructor
    static RoomExit createOneWay(ObjectId target) {
        RoomExit exit;
        exit.targetRoom = target;
        exit.type = ExitType::ONE_WAY;
        return exit;
    }
    
    // Helper: Create exit that requires player to have an item
    static RoomExit createRequiresItem(ObjectId target, ObjectId requiredItem, const std::string& msg = "You need something to proceed.");
    
    // Helper: Create exit that requires a flag to be set on an object
    static RoomExit createRequiresFlag(ObjectId target, ObjectId obj, ObjectFlag flag, const std::string& msg = "You can't go that way.");
    
    // Helper: Create exit that requires a puzzle to be solved (generic condition)
    static RoomExit createRequiresPuzzle(ObjectId target, std::function<bool()> puzzleSolved, const std::string& msg = "Something blocks your way.");
};

class ZRoom : public ZObject {
public:
    ZRoom(ObjectId id, const std::string& desc, const std::string& longDesc);
    
    void setExit(Direction dir, const RoomExit& exit);
    RoomExit* getExit(Direction dir);
    
    const std::string& getLongDesc() const { return longDesc_; }
    
    // Room action handler (M-LOOK, etc.)
    using RoomActionFunc = std::function<void(int)>;
    void setRoomAction(RoomActionFunc func) { roomAction_ = func; }
    void performRoomAction(int arg) { if (roomAction_) roomAction_(arg); }
    
private:
    std::string longDesc_;
    std::map<Direction, RoomExit> exits_;
    RoomActionFunc roomAction_;
};

// Room action arguments (from ZIL)
constexpr int M_LOOK = 1;
constexpr int M_ENTER = 2;
constexpr int M_END = 3;
constexpr int M_PRAY = 4;
constexpr int M_LISTEN = 5;
constexpr int M_YELL = 6;
