#include "globals.h"
#include "systems/combat.h"
#include "systems/timer.h"
#include "systems/death.h"

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
    // End any active combat
    if (CombatSystem::isInCombat()) {
        CombatSystem::endCombat();
    }
    
    // Clear all timers
    TimerSystem::clear();
    
    // Reset death system
    DeathSystem::reset();
    
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
    lampBattery = 330;
    lampWarned = false;
}
