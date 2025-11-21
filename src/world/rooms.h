#pragma once
#include "core/types.h"
#include "core/object.h"
#include <string>
#include <map>
#include <functional>

// Room IDs
constexpr ObjectId ROOM_WEST_OF_HOUSE = 1000;
constexpr ObjectId ROOM_NORTH_OF_HOUSE = 1001;
constexpr ObjectId ROOM_SOUTH_OF_HOUSE = 1002;
constexpr ObjectId ROOM_EAST_OF_HOUSE = 1003;
constexpr ObjectId ROOM_STONE_BARROW = 1004;
constexpr ObjectId ROOM_FOREST_1 = 1005;

// Direction enum
enum class Direction {
    NORTH, SOUTH, EAST, WEST,
    NE, NW, SE, SW,
    UP, DOWN, IN, OUT
};

// Room exit structure
struct RoomExit {
    ObjectId targetRoom = 0;
    std::string message;  // For blocked exits
    std::function<bool()> condition;  // Optional condition
    
    RoomExit() = default;
    RoomExit(ObjectId target) : targetRoom(target) {}
    RoomExit(const std::string& msg) : message(msg) {}
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
