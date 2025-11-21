#include "rooms.h"

ZRoom::ZRoom(ObjectId id, const std::string& desc, const std::string& longDesc)
    : ZObject(id, desc), longDesc_(longDesc) {}

void ZRoom::setExit(Direction dir, const RoomExit& exit) {
    exits_[dir] = exit;
}

RoomExit* ZRoom::getExit(Direction dir) {
    auto it = exits_.find(dir);
    return it != exits_.end() ? &it->second : nullptr;
}
