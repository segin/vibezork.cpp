#include "rooms.h"
#include "core/globals.h"

ZRoom::ZRoom(ObjectId id, std::string_view desc, std::string_view longDesc)
    : ZObject(id, desc), longDesc_(longDesc) {}

void ZRoom::setExit(Direction dir, const RoomExit& exit) {
    exits_[dir] = exit;
}

RoomExit* ZRoom::getExit(Direction dir) {
    auto it = exits_.find(dir);
    return it != exits_.end() ? &it->second : nullptr;
}

const RoomExit* ZRoom::getExit(Direction dir) const {
    auto it = exits_.find(dir);
    return it != exits_.end() ? &it->second : nullptr;
}

RoomExit RoomExit::createRequiresItem(ObjectId target, ObjectId requiredItem, std::string_view msg) {
    RoomExit exit;
    exit.targetRoom = target;
    exit.type = ExitType::CONDITIONAL;
    exit.message = msg;
    exit.condition = [requiredItem]() {
        auto& g = Globals::instance();
        if (!g.winner) return false;
        
        // Check if player has the required item
        const auto& contents = g.winner->getContents();
        for (const auto* obj : contents) {
            if (obj->getId() == requiredItem) {
                return true;
            }
        }
        return false;
    };
    return exit;
}

RoomExit RoomExit::createRequiresFlag(ObjectId target, ObjectId objId, ObjectFlag flag, std::string_view msg) {
    RoomExit exit;
    exit.targetRoom = target;
    exit.type = ExitType::CONDITIONAL;
    exit.message = msg;
    exit.condition = [objId, flag]() {
        auto& g = Globals::instance();
        ZObject* obj = g.getObject(objId);
        return obj && obj->hasFlag(flag);
    };
    return exit;
}

RoomExit RoomExit::createRequiresPuzzle(ObjectId target, std::function<bool()> puzzleSolved, std::string_view msg) {
    RoomExit exit;
    exit.targetRoom = target;
    exit.type = ExitType::CONDITIONAL;
    exit.message = msg;
    exit.condition = std::move(puzzleSolved);
    return exit;
}
