#pragma once
#include "types.h"
#include "object.h"
#include <memory>
#include <map>

// Global game state
class Globals {
public:
    static Globals& instance();
    
    // Core objects
    ZObject* here = nullptr;           // Current room
    ZObject* winner = nullptr;         // Current actor (usually ADVENTURER)
    ZObject* player = nullptr;         // Player object
    ZObject* prso = nullptr;           // Direct object
    ZObject* prsi = nullptr;           // Indirect object
    VerbId prsa = 0;                   // Current verb
    
    // Game state
    bool lit = false;                  // Is current room lit?
    int score = 0;
    int moves = 0;
    int loadMax = 100;
    int loadAllowed = 100;
    
    // Parser state
    bool pCont = false;
    bool quoteFlag = false;
    
    // Object registry
    void registerObject(ObjectId id, std::unique_ptr<ZObject> obj);
    ZObject* getObject(ObjectId id);
    const std::map<ObjectId, std::unique_ptr<ZObject>>& getAllObjects() const { return objects_; }
    
private:
    Globals() = default;
    std::map<ObjectId, std::unique_ptr<ZObject>> objects_;
};
