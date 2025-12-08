#include "test_framework.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include <sstream>

// Output capture helpers
static std::streambuf* oldCoutPtr;
static std::stringstream captureStream;

static void startCapture() {
    oldCoutPtr = std::cout.rdbuf();
    captureStream.str("");
    captureStream.clear();
    std::cout.rdbuf(captureStream.rdbuf());
}

static std::string stopCapture() {
    std::cout.rdbuf(oldCoutPtr);
    return captureStream.str();
}

TEST(UnlockSuccessWithHandler) {
    auto& g = Globals::instance();
    g.reset();
    
    // Room
    auto room = std::make_unique<ZRoom>(100, "Cell", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    // Player
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    // Locked Box
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::LOCKEDBIT);
    box->moveTo(g.here);
    // Mock handler: Says "Click!" and returns true
    box->setAction([]() {
        std::cout << "Click! The box is unlocked." << std::endl;
        return true;
    });
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    // Key
    auto key = std::make_unique<ZObject>(300, "key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->setFlag(ObjectFlag::TAKEBIT);
    key->moveTo(g.player);
    ZObject* keyPtr = key.get();
    g.registerObject(300, std::move(key));
    
    // "unlock box with key"
    g.prso = boxPtr;
    g.prsi = keyPtr;
    
    startCapture();
    bool res = Verbs::vUnlock();
    std::string output = stopCapture();
    
    ASSERT_TRUE(res);
    ASSERT_CONTAINS(output, "Click!");
    ASSERT_NOT_CONTAINS(output, "doesn't seem to work");
}

TEST(UnlockFailureDefault) {
    auto& g = Globals::instance();
    g.reset();
    
    // Setup identical to above but NO action handler
    auto room = std::make_unique<ZRoom>(100, "Cell", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::LOCKEDBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    auto key = std::make_unique<ZObject>(300, "key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.player);
    ZObject* keyPtr = key.get();
    g.registerObject(300, std::move(key));
    
    g.prso = boxPtr;
    g.prsi = keyPtr;
    
    startCapture();
    Verbs::vUnlock();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "doesn't seem to work");
}

TEST(UnlockNotLockedFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Cell", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto box = std::make_unique<ZObject>(200, "box");
    // Missing LOCKEDBIT
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    auto key = std::make_unique<ZObject>(300, "key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.player);
    ZObject* keyPtr = key.get();
    g.registerObject(300, std::move(key));
    
    g.prso = boxPtr;
    g.prsi = keyPtr;
    
    startCapture();
    Verbs::vUnlock();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "not locked");
}

TEST(UnlockNotToolFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Cell", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::LOCKEDBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    auto sandwich = std::make_unique<ZObject>(300, "sandwich");
    // Missing TOOLBIT
    sandwich->moveTo(g.player);
    ZObject* swPtr = sandwich.get();
    g.registerObject(300, std::move(sandwich));
    
    g.prso = boxPtr;
    g.prsi = swPtr;
    
    startCapture();
    Verbs::vUnlock();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "can't unlock anything with that");
}

TEST(UnlockNotHeldFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Cell", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::LOCKEDBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    // Create a closed container
    auto bag = std::make_unique<ZObject>(400, "bag");
    bag->setFlag(ObjectFlag::CONTBIT);
    // Not OPENBIT
    bag->moveTo(g.here);
    ZObject* bagPtr = bag.get();
    g.registerObject(400, std::move(bag));

    // Put key inside closed bag
    auto key = std::make_unique<ZObject>(300, "key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(bagPtr); // Inside bag
    ZObject* keyPtr = key.get();
    g.registerObject(300, std::move(key));
    
    g.prso = boxPtr;
    g.prsi = keyPtr;
    
    startCapture();
    Verbs::vUnlock();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "don't have that");
}

int main() {
    std::cout << "Running Unlock Syntax Tests\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    return failed > 0 ? 1 : 0;
}
