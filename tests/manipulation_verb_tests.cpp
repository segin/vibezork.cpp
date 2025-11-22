#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/verbs/verbs.h"

// Test DROP verb - Task 27.3

TEST(DropVerbBasic) {
    // Test dropping a carried object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object in player inventory
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_DROP;
    
    // Test DROP verb
    bool result = Verbs::vDrop();
    ASSERT_TRUE(result);
    
    // Verify lamp is now in the room
    ASSERT_EQ(lampPtr->getLocation(), g.here);
    
    // Cleanup
    g.reset();
}

TEST(DropVerbNoObject) {
    // Test DROP verb without specifying an object
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
    g.prsa = V_DROP;
    
    // Test DROP verb without object
    bool result = Verbs::vDrop();
    ASSERT_TRUE(result);
    
    // Should display "Drop what?" message
    // (We can't easily test output, but we verify it doesn't crash)
    
    // Cleanup
    g.reset();
}

TEST(DropVerbNotInInventory) {
    // Test dropping an object not in inventory (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object in the room (not in inventory)
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.here);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_DROP;
    
    // Test DROP verb on object not in inventory
    bool result = Verbs::vDrop();
    ASSERT_TRUE(result);
    
    // Verify lamp is still in the room (not moved)
    ASSERT_EQ(lampPtr->getLocation(), g.here);
    
    // Should display "You aren't carrying that." message
    
    // Cleanup
    g.reset();
}

TEST(DropVerbMultipleObjects) {
    // Test dropping multiple objects in sequence
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object in player inventory
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create sword object in player inventory
    auto sword = std::make_unique<ZObject>(2, "sword");
    sword->addSynonym("sword");
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(g.winner);
    ZObject* swordPtr = sword.get();
    g.registerObject(2, std::move(sword));
    
    // Drop lamp
    g.prso = lampPtr;
    g.prsa = V_DROP;
    bool result1 = Verbs::vDrop();
    ASSERT_TRUE(result1);
    ASSERT_EQ(lampPtr->getLocation(), g.here);
    
    // Drop sword
    g.prso = swordPtr;
    g.prsa = V_DROP;
    bool result2 = Verbs::vDrop();
    ASSERT_TRUE(result2);
    ASSERT_EQ(swordPtr->getLocation(), g.here);
    
    // Verify both objects are in the room
    const auto& roomContents = g.here->getContents();
    ASSERT_EQ(roomContents.size(), 2);
    
    // Cleanup
    g.reset();
}

TEST(DropVerbInDifferentRoom) {
    // Test dropping objects in different rooms
    auto& g = Globals::instance();
    
    // Create two test rooms
    auto room1 = std::make_unique<ZRoom>(100, "Room 1", "First room.");
    ZObject* room1Ptr = room1.get();
    g.registerObject(100, std::move(room1));
    
    auto room2 = std::make_unique<ZRoom>(101, "Room 2", "Second room.");
    ZObject* room2Ptr = room2.get();
    g.registerObject(101, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lamp object in player inventory
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->setFlag(ObjectFlag::TAKEBIT);
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Start in room 1
    g.here = room1Ptr;
    g.winner->moveTo(room1Ptr);
    
    // Drop lamp in room 1
    g.prso = lampPtr;
    g.prsa = V_DROP;
    bool result1 = Verbs::vDrop();
    ASSERT_TRUE(result1);
    ASSERT_EQ(lampPtr->getLocation(), room1Ptr);
    
    // Move to room 2
    g.here = room2Ptr;
    g.winner->moveTo(room2Ptr);
    
    // Pick up lamp again (simulate)
    lampPtr->moveTo(g.winner);
    
    // Drop lamp in room 2
    g.prso = lampPtr;
    g.prsa = V_DROP;
    bool result2 = Verbs::vDrop();
    ASSERT_TRUE(result2);
    ASSERT_EQ(lampPtr->getLocation(), room2Ptr);
    
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
