#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/verbs/verbs.h"

// Test light source verbs (LAMP-ON, LAMP-OFF) - Task 34.3

TEST(LampOnVerbWithLightSource) {
    // Test LAMP-ON verb on a light source
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object with LIGHTBIT flag
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->addSynonym("lantern");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setProperty(P_CAPACITY, 100);  // Has battery
    lamp->moveTo(g.winner);  // In player's inventory
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is off initially
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_LAMP_ON;
    
    // Test LAMP-ON verb
    bool result = Verbs::vLampOn();
    ASSERT_TRUE(result);
    
    // Verify lamp is now on
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Cleanup
    g.reset();
}

TEST(LampOnVerbAlreadyOn) {
    // Test LAMP-ON verb on a lamp that's already on
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp that's already on
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);  // Already on
    lamp->setProperty(P_CAPACITY, 100);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is on
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_LAMP_ON;
    
    // Test LAMP-ON verb on already-on lamp
    bool result = Verbs::vLampOn();
    ASSERT_TRUE(result);
    
    // Verify lamp is still on
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Should display "It's already on." message
    
    // Cleanup
    g.reset();
}

TEST(LampOnVerbWithDepletedBattery) {
    // Test LAMP-ON verb on a lamp with depleted battery
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp with depleted battery
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setProperty(P_CAPACITY, 0);  // Depleted battery
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is off
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_LAMP_ON;
    
    // Test LAMP-ON verb on depleted lamp
    bool result = Verbs::vLampOn();
    ASSERT_TRUE(result);
    
    // Verify lamp is still off (can't turn on)
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Should display "The lamp has no more power." message
    
    // Cleanup
    g.reset();
}

TEST(LampOnVerbOnNonLightObject) {
    // Test LAMP-ON verb on a non-light object (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-light object (no LIGHTBIT flag)
    auto rock = std::make_unique<ZObject>(1, "rock");
    rock->addSynonym("rock");
    rock->moveTo(g.winner);
    ZObject* rockPtr = rock.get();
    g.registerObject(1, std::move(rock));
    
    // Set up verb context
    g.prso = rockPtr;
    g.prsa = V_LAMP_ON;
    
    // Test LAMP-ON verb on non-light object
    bool result = Verbs::vLampOn();
    ASSERT_TRUE(result);
    
    // Verify rock doesn't have ONBIT flag
    ASSERT_FALSE(rockPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Should display "You can't turn that on." message
    
    // Cleanup
    g.reset();
}

TEST(LampOnVerbNoObject) {
    // Test LAMP-ON verb without specifying an object
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
    g.prsa = V_LAMP_ON;
    
    // Test LAMP-ON verb without object
    bool result = Verbs::vLampOn();
    ASSERT_TRUE(result);
    
    // Should display "Turn on what?" message
    
    // Cleanup
    g.reset();
}

TEST(LampOffVerbWithLightSource) {
    // Test LAMP-OFF verb on a light source that's on
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp that's on
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);  // On
    lamp->setProperty(P_CAPACITY, 100);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is on
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_LAMP_OFF;
    
    // Test LAMP-OFF verb
    bool result = Verbs::vLampOff();
    ASSERT_TRUE(result);
    
    // Verify lamp is now off
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Cleanup
    g.reset();
}

TEST(LampOffVerbAlreadyOff) {
    // Test LAMP-OFF verb on a lamp that's already off
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp that's off
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setProperty(P_CAPACITY, 100);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is off
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_LAMP_OFF;
    
    // Test LAMP-OFF verb on already-off lamp
    bool result = Verbs::vLampOff();
    ASSERT_TRUE(result);
    
    // Verify lamp is still off
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Should display "It's already off." message
    
    // Cleanup
    g.reset();
}

TEST(LampOffVerbOnNonLightObject) {
    // Test LAMP-OFF verb on a non-light object (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-light object (no LIGHTBIT flag)
    auto book = std::make_unique<ZObject>(1, "book");
    book->addSynonym("book");
    book->moveTo(g.winner);
    ZObject* bookPtr = book.get();
    g.registerObject(1, std::move(book));
    
    // Set up verb context
    g.prso = bookPtr;
    g.prsa = V_LAMP_OFF;
    
    // Test LAMP-OFF verb on non-light object
    bool result = Verbs::vLampOff();
    ASSERT_TRUE(result);
    
    // Should display "You can't turn that off." message
    
    // Cleanup
    g.reset();
}

TEST(LampOffVerbNoObject) {
    // Test LAMP-OFF verb without specifying an object
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
    g.prsa = V_LAMP_OFF;
    
    // Test LAMP-OFF verb without object
    bool result = Verbs::vLampOff();
    ASSERT_TRUE(result);
    
    // Should display "Turn off what?" message
    
    // Cleanup
    g.reset();
}

TEST(LampOnOffCycle) {
    // Test turning lamp on and off multiple times
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp
    auto lamp = std::make_unique<ZObject>(1, "brass lantern");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setProperty(P_CAPACITY, 100);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Initially off
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Turn on
    g.prso = lampPtr;
    g.prsa = V_LAMP_ON;
    Verbs::vLampOn();
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Turn off
    g.prsa = V_LAMP_OFF;
    Verbs::vLampOff();
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Turn on again
    g.prsa = V_LAMP_ON;
    Verbs::vLampOn();
    ASSERT_TRUE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
    // Turn off again
    g.prsa = V_LAMP_OFF;
    Verbs::vLampOff();
    ASSERT_FALSE(lampPtr->hasFlag(ObjectFlag::ONBIT));
    
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
