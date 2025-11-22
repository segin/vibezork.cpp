#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/verbs/verbs.h"

// Test interaction verbs (TIE, UNTIE, LISTEN, SMELL, TOUCH) - Task 32.6

TEST(TieVerbBasic) {
    // Test TIE verb with two objects
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create rope object
    auto rope = std::make_unique<ZObject>(1, "rope");
    rope->addSynonym("rope");
    rope->moveTo(g.here);
    ZObject* ropePtr = rope.get();
    g.registerObject(1, std::move(rope));
    
    // Create hook object
    auto hook = std::make_unique<ZObject>(2, "hook");
    hook->addSynonym("hook");
    hook->moveTo(g.here);
    ZObject* hookPtr = hook.get();
    g.registerObject(2, std::move(hook));
    
    // Set up verb context
    g.prso = ropePtr;
    g.prsi = hookPtr;
    g.prsa = V_TIE;
    
    // Test TIE verb
    bool result = Verbs::vTie();
    ASSERT_TRUE(result);
    
    // Should display "You can't tie that." message (default behavior)
    
    // Cleanup
    g.reset();
}

TEST(TieVerbNoObject) {
    // Test TIE verb without specifying an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up verb context without object
    g.prso = nullptr;
    g.prsi = nullptr;
    g.prsa = V_TIE;
    
    // Test TIE verb without object
    bool result = Verbs::vTie();
    ASSERT_TRUE(result);
    
    // Should display "Tie what?" message
    
    // Cleanup
    g.reset();
}

TEST(TieVerbNoIndirectObject) {
    // Test TIE verb without specifying what to tie to
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create rope object
    auto rope = std::make_unique<ZObject>(1, "rope");
    rope->addSynonym("rope");
    rope->moveTo(g.here);
    ZObject* ropePtr = rope.get();
    g.registerObject(1, std::move(rope));
    
    // Set up verb context with object but no indirect object
    g.prso = ropePtr;
    g.prsi = nullptr;
    g.prsa = V_TIE;
    
    // Test TIE verb without indirect object
    bool result = Verbs::vTie();
    ASSERT_TRUE(result);
    
    // Should display "Tie it to what?" message
    
    // Cleanup
    g.reset();
}

TEST(UntieVerbBasic) {
    // Test UNTIE verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create rope object
    auto rope = std::make_unique<ZObject>(1, "rope");
    rope->addSynonym("rope");
    rope->moveTo(g.here);
    ZObject* ropePtr = rope.get();
    g.registerObject(1, std::move(rope));
    
    // Set up verb context
    g.prso = ropePtr;
    g.prsa = V_UNTIE;
    
    // Test UNTIE verb
    bool result = Verbs::vUntie();
    ASSERT_TRUE(result);
    
    // Should display "It's not tied." message (default behavior)
    
    // Cleanup
    g.reset();
}

TEST(UntieVerbNoObject) {
    // Test UNTIE verb without specifying an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up verb context without object
    g.prso = nullptr;
    g.prsa = V_UNTIE;
    
    // Test UNTIE verb without object
    bool result = Verbs::vUntie();
    ASSERT_TRUE(result);
    
    // Should display "Untie what?" message
    
    // Cleanup
    g.reset();
}

TEST(ListenVerbBasic) {
    // Test LISTEN verb in a room
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up verb context (no object - listen to room)
    g.prso = nullptr;
    g.prsa = V_LISTEN;
    
    // Test LISTEN verb
    bool result = Verbs::vListen();
    ASSERT_TRUE(result);
    
    // Should display "You hear nothing unusual." message
    
    // Cleanup
    g.reset();
}

TEST(ListenVerbWithObject) {
    // Test LISTEN verb on a specific object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create door object
    auto door = std::make_unique<ZObject>(1, "door");
    door->addSynonym("door");
    door->moveTo(g.here);
    ZObject* doorPtr = door.get();
    g.registerObject(1, std::move(door));
    
    // Set up verb context with object
    g.prso = doorPtr;
    g.prsa = V_LISTEN;
    
    // Test LISTEN verb on object
    bool result = Verbs::vListen();
    ASSERT_TRUE(result);
    
    // Should display "You hear nothing unusual." message (default behavior)
    
    // Cleanup
    g.reset();
}

TEST(SmellVerbBasic) {
    // Test SMELL verb in a room
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up verb context (no object - smell room)
    g.prso = nullptr;
    g.prsa = V_SMELL;
    
    // Test SMELL verb
    bool result = Verbs::vSmell();
    ASSERT_TRUE(result);
    
    // Should display "You smell nothing unusual." message
    
    // Cleanup
    g.reset();
}

TEST(SmellVerbWithObject) {
    // Test SMELL verb on a specific object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create flower object
    auto flower = std::make_unique<ZObject>(1, "flower");
    flower->addSynonym("flower");
    flower->moveTo(g.here);
    ZObject* flowerPtr = flower.get();
    g.registerObject(1, std::move(flower));
    
    // Set up verb context with object
    g.prso = flowerPtr;
    g.prsa = V_SMELL;
    
    // Test SMELL verb on object
    bool result = Verbs::vSmell();
    ASSERT_TRUE(result);
    
    // Should display "You smell nothing unusual." message (default behavior)
    
    // Cleanup
    g.reset();
}

TEST(TouchVerbBasic) {
    // Test TOUCH verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create wall object
    auto wall = std::make_unique<ZObject>(1, "wall");
    wall->addSynonym("wall");
    wall->moveTo(g.here);
    ZObject* wallPtr = wall.get();
    g.registerObject(1, std::move(wall));
    
    // Set up verb context
    g.prso = wallPtr;
    g.prsa = V_TOUCH;
    
    // Test TOUCH verb
    bool result = Verbs::vTouch();
    ASSERT_TRUE(result);
    
    // Should display "You feel nothing unusual." message
    
    // Cleanup
    g.reset();
}

TEST(TouchVerbNoObject) {
    // Test TOUCH verb without specifying an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Set up verb context without object
    g.prso = nullptr;
    g.prsa = V_TOUCH;
    
    // Test TOUCH verb without object
    bool result = Verbs::vTouch();
    ASSERT_TRUE(result);
    
    // Should display "Touch what?" message
    
    // Cleanup
    g.reset();
}

int main() {
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed > 0 ? 1 : 0;
}
