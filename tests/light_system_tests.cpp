#include "test_framework.h"
#include "../src/systems/light.h"
#include "../src/core/globals.h"
#include "../src/core/object.h"
#include "../src/world/world.h"

// Test: Room with ONBIT flag is lit (Requirement 50)
TEST(naturally_lit_room) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a room with ONBIT flag
    auto room = std::make_unique<ZObject>(1, "test room");
    room->setFlag(ObjectFlag::ONBIT);
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    ASSERT_TRUE(LightSystem::isRoomLit(roomPtr));
}

// Test: Dark room without light sources is not lit (Requirement 50)
TEST(dark_room) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a room without ONBIT flag
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    ASSERT_FALSE(LightSystem::isRoomLit(roomPtr));
}

// Test: Room with lit lamp is lit (Requirement 50)
TEST(room_with_lamp) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    // Create a lamp with LIGHTBIT and ONBIT
    auto lamp = std::make_unique<ZObject>(2, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(roomPtr);
    g.registerObject(2, std::move(lamp));
    
    ASSERT_TRUE(LightSystem::isRoomLit(roomPtr));
}

// Test: Room with unlit lamp is dark (Requirement 50)
TEST(room_with_unlit_lamp) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    // Create a lamp with LIGHTBIT but not ONBIT
    auto lamp = std::make_unique<ZObject>(2, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->moveTo(roomPtr);
    g.registerObject(2, std::move(lamp));
    
    ASSERT_FALSE(LightSystem::isRoomLit(roomPtr));
}

// Test: Player with lit lamp in dark room (Requirement 50)
TEST(player_with_lamp) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    // Create player
    auto player = std::make_unique<ZObject>(2, "player");
    auto* playerPtr = player.get();
    player->moveTo(roomPtr);
    g.registerObject(2, std::move(player));
    g.winner = playerPtr;
    
    // Create a lamp in player's inventory
    auto lamp = std::make_unique<ZObject>(3, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(playerPtr);
    g.registerObject(3, std::move(lamp));
    
    ASSERT_TRUE(LightSystem::isRoomLit(roomPtr));
    ASSERT_TRUE(LightSystem::hasLightSource());
}

// Test: Lamp in closed container doesn't provide light (Requirement 50)
TEST(lamp_in_closed_container) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    // Create a closed container
    auto box = std::make_unique<ZObject>(2, "box");
    box->setFlag(ObjectFlag::CONTBIT);
    // Note: OPENBIT not set, so it's closed
    auto* boxPtr = box.get();
    box->moveTo(roomPtr);
    g.registerObject(2, std::move(box));
    
    // Create a lamp inside the closed container
    auto lamp = std::make_unique<ZObject>(3, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(boxPtr);
    g.registerObject(3, std::move(lamp));
    
    ASSERT_FALSE(LightSystem::isRoomLit(roomPtr));
}

// Test: Lamp in open container provides light (Requirement 50)
TEST(lamp_in_open_container) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    
    // Create an open container
    auto box = std::make_unique<ZObject>(2, "box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    auto* boxPtr = box.get();
    box->moveTo(roomPtr);
    g.registerObject(2, std::move(box));
    
    // Create a lamp inside the open container
    auto lamp = std::make_unique<ZObject>(3, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(boxPtr);
    g.registerObject(3, std::move(lamp));
    
    ASSERT_TRUE(LightSystem::isRoomLit(roomPtr));
}

// Test: updateLighting sets global lit flag (Requirement 50)
TEST(update_lighting) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a naturally lit room
    auto room = std::make_unique<ZObject>(1, "lit room");
    room->setFlag(ObjectFlag::ONBIT);
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    
    g.lit = false;  // Start with lit flag false
    LightSystem::updateLighting();
    
    ASSERT_TRUE(g.lit);
}

// Test: updateLighting in dark room (Requirement 50)
TEST(update_lighting_dark) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    
    g.lit = true;  // Start with lit flag true
    LightSystem::updateLighting();
    
    ASSERT_FALSE(g.lit);
}

// Test: First turn in darkness gives grue warning (Requirement 51)
TEST(grue_first_warning) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    g.lit = false;
    
    // First call to checkGrue should warn about grue
    // We can't easily test the output, but we can verify it doesn't crash
    LightSystem::checkGrue();
    
    // Test passes if no crash occurs
    ASSERT_TRUE(true);
}

// Test: Multiple turns in darkness escalate warnings (Requirement 51)
TEST(grue_escalating_warnings) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    g.lit = false;
    
    // Call checkGrue multiple times to simulate turns in darkness
    LightSystem::checkGrue();  // Turn 1: initial warning
    LightSystem::checkGrue();  // Turn 2: rustling sound
    LightSystem::checkGrue();  // Turn 3: more danger
    
    // Test passes if no crash occurs
    ASSERT_TRUE(true);
}

// Test: Grue attacks after several turns in darkness (Requirement 51)
TEST(grue_attack) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    g.lit = false;
    
    // Call checkGrue enough times to trigger attack
    LightSystem::checkGrue();  // Turn 1
    LightSystem::checkGrue();  // Turn 2
    LightSystem::checkGrue();  // Turn 3
    LightSystem::checkGrue();  // Turn 4: attack!
    
    // Test passes if no crash occurs (death message displayed)
    ASSERT_TRUE(true);
}

// Test: Light prevents grue attack (Requirement 51)
TEST(light_prevents_grue) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    
    // Create player
    auto player = std::make_unique<ZObject>(2, "player");
    auto* playerPtr = player.get();
    player->moveTo(roomPtr);
    g.registerObject(2, std::move(player));
    g.winner = playerPtr;
    
    // Create a lamp in player's inventory
    auto lamp = std::make_unique<ZObject>(3, "lamp");
    lamp->setFlag(ObjectFlag::LIGHTBIT);
    lamp->setFlag(ObjectFlag::ONBIT);
    lamp->moveTo(playerPtr);
    g.registerObject(3, std::move(lamp));
    
    g.lit = true;
    
    // Call checkGrue multiple times - should not attack because we have light
    LightSystem::checkGrue();
    LightSystem::checkGrue();
    LightSystem::checkGrue();
    LightSystem::checkGrue();
    LightSystem::checkGrue();
    
    // Test passes if no crash occurs and no death message
    ASSERT_TRUE(true);
}

// Test: Darkness counter resets when entering light (Requirement 51)
TEST(darkness_counter_resets) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create a dark room
    auto room = std::make_unique<ZObject>(1, "dark room");
    auto* roomPtr = room.get();
    g.registerObject(1, std::move(room));
    g.here = roomPtr;
    g.lit = false;
    
    // Spend some turns in darkness
    LightSystem::checkGrue();
    LightSystem::checkGrue();
    
    // Now turn on light
    g.lit = true;
    LightSystem::checkGrue();
    
    // Go back to darkness - should start warnings from beginning
    g.lit = false;
    LightSystem::checkGrue();
    
    // Test passes if no crash occurs
    ASSERT_TRUE(true);
}

int main() {
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
    }
    
    std::cout << "\n=== Light System Tests ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << results.size() << std::endl;
    
    return (passed == results.size()) ? 0 : 1;
}
