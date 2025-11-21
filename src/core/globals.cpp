#include "globals.h"

Globals& Globals::instance() {
    static Globals inst;
    return inst;
}

void Globals::registerObject(ObjectId id, std::unique_ptr<ZObject> obj) {
    objects_[id] = std::move(obj);
}

ZObject* Globals::getObject(ObjectId id) {
    auto it = objects_.find(id);
    return it != objects_.end() ? it->second.get() : nullptr;
}
