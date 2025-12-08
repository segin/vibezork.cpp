#include "test_framework.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include "../src/parser/verb_registry.h"
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

TEST(GiveToObjectSuccess) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Test Room", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    // Create Player
    auto player = std::make_unique<ZObject>(1, "player");
    player->setFlag(ObjectFlag::ACTORBIT);
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));

    // Create Recipient (Troll)
    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->setFlag(ObjectFlag::ACTORBIT);
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));
    
    // Create Gift (Sword)
    auto sword = std::make_unique<ZObject>(300, "sword");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(g.player); // Must be held to give
    ZObject* swordPtr = sword.get();
    g.registerObject(300, std::move(sword));
    
    // "give sword to troll"
    g.prso = swordPtr;
    g.prsi = trollPtr;
    
    startCapture();
    bool result = Verbs::vGive(); // vGive checks logic
    std::string output = stopCapture();
    
    // Expect success (or at least refusal message if no handler)
    // "The troll refuses it politely." is default behavior implemented in vGive
    ASSERT_TRUE(result);
    ASSERT_CONTAINS(output, "refuses it politely");
}

TEST(GiveNotHoldingFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Test Room", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player");
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));

    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));
    
    auto sword = std::make_unique<ZObject>(300, "sword");
    sword->moveTo(g.here); // On floor, NOT held
    ZObject* swordPtr = sword.get();
    g.registerObject(300, std::move(sword));
    
    g.prso = swordPtr;
    g.prsi = trollPtr;
    
    startCapture();
    Verbs::vGive();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "not holding that");
}

int main() {
    std::cout << "Running Give Syntax Tests\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    return failed > 0 ? 1 : 0;
}
