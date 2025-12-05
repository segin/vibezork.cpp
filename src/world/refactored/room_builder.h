// Room builder - fluent API for creating ZRooms with correct initial values
// Eliminates boilerplate and ensures rooms are created correctly from the start
#pragma once

#include "../rooms.h"
#include "core/globals.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <functional>

// Simple exit: direction -> target room
struct SimpleExit {
    Direction dir;
    ObjectId target;
};

// Room configuration using designated initializers (C++20)
struct RoomDef {
    ObjectId id;
    std::string_view name;
    std::string_view longDesc;
    std::vector<SimpleExit> exits = {};
    std::vector<ObjectFlag> flags = {};
    ZRoom::RoomActionFunc action = nullptr;
};


// Create and register a room from definition
inline ZRoom* createRoom(const RoomDef& def) {
    auto& g = Globals::instance();
    
    auto room = std::make_unique<ZRoom>(def.id, std::string(def.name), 
                                         std::string(def.longDesc));
    
    for (const auto& [dir, target] : def.exits) {
        room->setExit(dir, RoomExit(target));
    }
    
    for (auto flag : def.flags) {
        room->setFlag(flag);
    }
    
    if (def.action) {
        room->setRoomAction(def.action);
    }
    
    ZRoom* ptr = room.get();
    g.registerRoom(def.id, std::move(room));
    return ptr;
}
