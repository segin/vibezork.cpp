// Room builder - fluent API for creating ZRooms with correct initial values
// Eliminates boilerplate and ensures rooms are created correctly from the start
#pragma once

#include "rooms.h"
#include "core/globals.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>

// Exit types for room definitions
struct ExitToRoom {
    Direction dir;
    ObjectId target;
};

struct BlockedExit {
    Direction dir;
    std::string_view message;
};

struct ConditionalExit {
    Direction dir;
    ObjectId target;
    std::function<bool()> condition;
    std::string_view blockedMsg = "";
};

// Room configuration using designated initializers (C++20)
struct RoomDef {
    ObjectId id;
    std::string_view name;
    std::string_view longDesc;
    std::vector<ExitToRoom> exits = {};
    std::vector<BlockedExit> blockedExits = {};
    std::vector<ConditionalExit> conditionalExits = {};
    std::vector<ObjectFlag> flags = {};
    ZRoom::RoomActionFunc action = nullptr;
};


// Create and register a room from definition
inline ZRoom* createRoom(const RoomDef& def) {
    auto& g = Globals::instance();
    
    auto room = std::make_unique<ZRoom>(def.id, std::string(def.name), 
                                         std::string(def.longDesc));
    
    // Normal exits
    for (const auto& [dir, target] : def.exits) {
        room->setExit(dir, RoomExit(target));
    }
    
    // Blocked exits (with message)
    for (const auto& [dir, message] : def.blockedExits) {
        room->setExit(dir, RoomExit(message));
    }
    
    // Conditional exits
    for (const auto& cond : def.conditionalExits) {
        room->setExit(cond.dir, RoomExit::createConditional(
            cond.target, cond.condition, cond.blockedMsg));
    }
    
    for (auto flag : def.flags) {
        room->setFlag(flag);
    }
    
    if (def.action) {
        room->setRoomAction(def.action);
    }
    
    ZRoom* ptr = room.get();
    g.registerObject(def.id, std::move(room));
    return ptr;
}
