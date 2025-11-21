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

void Globals::reset() {
    objects_.clear();
    here = nullptr;
    winner = nullptr;
    player = nullptr;
    prso = nullptr;
    prsi = nullptr;
    prsa = 0;
    lit = false;
    score = 0;
    moves = 0;
    pCont = false;
    quoteFlag = false;
}
