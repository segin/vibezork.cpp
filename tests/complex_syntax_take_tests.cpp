#include "test_framework.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include "../src/parser/verb_registry.h"
#include <sstream>

// Output capture helpers (need to be cleaner, maybe move to framework later)
extern std::stringstream captureSS; 
// Defining them here again for now to avoid linker issues if not shared properly
// Actually, using the ones I saw in game_command_tests.cpp might duplicate if I link all together.
// But checking the CMakeLists.txt, I link specific files.
// complex_syntax_tests did NOT use capture.
// game_command_tests DEFINED them.
// I will define them here locally.

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

TEST(TakeFromContainerSuccess) {
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
    
    // Create Box
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    // Create Ball inside Box
    auto ball = std::make_unique<ZObject>(201, "ball");
    ball->setFlag(ObjectFlag::TAKEBIT);
    ball->moveTo(boxPtr);
    ZObject* ballPtr = ball.get();
    g.registerObject(201, std::move(ball));
    
    VerbRegistry registry;
    Parser parser(&registry);
    
    // "take ball from box"
    // We need to simulate the PARSED command execution or just run vTake?
    // vTake relies on g.prso and g.prsi.
    
    g.prso = ballPtr;
    g.prsi = boxPtr;
    
    startCapture();
    bool result = Verbs::vTake();
    std::string output = stopCapture();
    
    // Should succeed
    ASSERT_TRUE(result); // RTRUE usually
    ASSERT_CONTAINS(output, "Taken");
    ASSERT_EQ(ballPtr->getLocation(), g.winner);
}

TEST(TakeFromWrongSourceFailure) {
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
    
    // Create Box
    auto box = std::make_unique<ZObject>(200, "box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(200, std::move(box));
    
    // Create Ball on FLOOR (not in box)
    auto ball = std::make_unique<ZObject>(201, "ball");
    ball->setFlag(ObjectFlag::TAKEBIT);
    ball->moveTo(g.here);
    ZObject* ballPtr = ball.get();
    g.registerObject(201, std::move(ball));
    
    // "take ball from box"
    g.prso = ballPtr;
    g.prsi = boxPtr;
    
    startCapture();
    Verbs::vTake();
    std::string output = stopCapture();
    
    // Should FAIL because ball is not in box
    // But currently vTake ignores PRSI, so it will likely SUCCEED in taking it from floor
    // This assertion expects CORRECT behavior (Failure).
    // So this test should FAIL currently.
    
    ASSERT_CONTAINS(output, "isn't in the box");
    ASSERT_EQ(ballPtr->getLocation(), g.here); // Should remain in room
}

TEST(TakeFromClosedTransparentFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create Player
    auto player = std::make_unique<ZObject>(1, "player");
    player->setFlag(ObjectFlag::ACTORBIT);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto room = std::make_unique<ZRoom>(100, "Test Room", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    // Create Glass Box (Transparent, Closed)
    auto glassBox = std::make_unique<ZObject>(300, "glass_box");
    glassBox->setFlag(ObjectFlag::CONTBIT);
    // OPENBIT NOT set
    glassBox->setFlag(ObjectFlag::TRANSBIT);
    glassBox->moveTo(g.here);
    ZObject* boxPtr = glassBox.get();
    g.registerObject(300, std::move(glassBox));
    
    // Create Diamond inside Box
    auto diamond = std::make_unique<ZObject>(301, "diamond");
    diamond->setFlag(ObjectFlag::TAKEBIT);
    diamond->moveTo(boxPtr);
    ZObject* diamondPtr = diamond.get();
    g.registerObject(301, std::move(diamond));
    
    // "take diamond from glass_box"
    g.prso = diamondPtr;
    g.prsi = boxPtr;
    
    startCapture();
    Verbs::vTake();
    std::string output = stopCapture();
    
    // Should FAIL because box is closed (even if transparent)
    ASSERT_CONTAINS(output, "isn't open");
    ASSERT_EQ(diamondPtr->getLocation(), boxPtr); // Should remain in box
}

int main() {
    std::cout << "Running Take Syntax Tests\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    return failed > 0 ? 1 : 0;
}
