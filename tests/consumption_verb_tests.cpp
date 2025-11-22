#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/verbs/verbs.h"

// Test consumption verbs (EAT, DRINK) - Task 33.3

TEST(EatVerbWithFood) {
    // Test EAT verb on a food item
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create food object with FOODBIT flag
    auto sandwich = std::make_unique<ZObject>(1, "sandwich");
    sandwich->addSynonym("sandwich");
    sandwich->setFlag(ObjectFlag::FOODBIT);
    sandwich->moveTo(g.winner);  // In player's inventory
    ZObject* sandwichPtr = sandwich.get();
    g.registerObject(1, std::move(sandwich));
    
    // Verify sandwich is in inventory
    ASSERT_TRUE(sandwichPtr->getLocation() == g.winner);
    
    // Set up verb context
    g.prso = sandwichPtr;
    g.prsa = V_EAT;
    
    // Test EAT verb
    bool result = Verbs::vEat();
    ASSERT_TRUE(result);
    
    // Verify sandwich was removed from game (consumed)
    ASSERT_TRUE(sandwichPtr->getLocation() == nullptr);
    
    // Cleanup
    g.reset();
}

TEST(EatVerbWithNonFood) {
    // Test EAT verb on a non-food item (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-food object (no FOODBIT flag)
    auto rock = std::make_unique<ZObject>(1, "rock");
    rock->addSynonym("rock");
    rock->moveTo(g.winner);
    ZObject* rockPtr = rock.get();
    g.registerObject(1, std::move(rock));
    
    // Verify rock is in inventory
    ASSERT_TRUE(rockPtr->getLocation() == g.winner);
    
    // Set up verb context
    g.prso = rockPtr;
    g.prsa = V_EAT;
    
    // Test EAT verb on non-food
    bool result = Verbs::vEat();
    ASSERT_TRUE(result);
    
    // Verify rock was NOT removed (can't eat it)
    ASSERT_TRUE(rockPtr->getLocation() == g.winner);
    
    // Should display "That's not edible." message
    
    // Cleanup
    g.reset();
}

TEST(EatVerbNoObject) {
    // Test EAT verb without specifying an object
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
    g.prsa = V_EAT;
    
    // Test EAT verb without object
    bool result = Verbs::vEat();
    ASSERT_TRUE(result);
    
    // Should display "Eat what?" message
    
    // Cleanup
    g.reset();
}

TEST(DrinkVerbWithLiquid) {
    // Test DRINK verb on a drinkable item
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create drinkable object with DRINKBIT flag
    auto water = std::make_unique<ZObject>(1, "water");
    water->addSynonym("water");
    water->setFlag(ObjectFlag::DRINKBIT);
    water->moveTo(g.winner);  // In player's inventory
    ZObject* waterPtr = water.get();
    g.registerObject(1, std::move(water));
    
    // Verify water is in inventory
    ASSERT_TRUE(waterPtr->getLocation() == g.winner);
    
    // Set up verb context
    g.prso = waterPtr;
    g.prsa = V_DRINK;
    
    // Test DRINK verb
    bool result = Verbs::vDrink();
    ASSERT_TRUE(result);
    
    // Verify water was removed from game (consumed)
    ASSERT_TRUE(waterPtr->getLocation() == nullptr);
    
    // Cleanup
    g.reset();
}

TEST(DrinkVerbWithNonLiquid) {
    // Test DRINK verb on a non-drinkable item (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-drinkable object (no DRINKBIT flag)
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    lamp->moveTo(g.winner);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Verify lamp is in inventory
    ASSERT_TRUE(lampPtr->getLocation() == g.winner);
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_DRINK;
    
    // Test DRINK verb on non-drinkable
    bool result = Verbs::vDrink();
    ASSERT_TRUE(result);
    
    // Verify lamp was NOT removed (can't drink it)
    ASSERT_TRUE(lampPtr->getLocation() == g.winner);
    
    // Should display "That's not drinkable." message
    
    // Cleanup
    g.reset();
}

TEST(DrinkVerbNoObject) {
    // Test DRINK verb without specifying an object
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
    g.prsa = V_DRINK;
    
    // Test DRINK verb without object
    bool result = Verbs::vDrink();
    ASSERT_TRUE(result);
    
    // Should display "Drink what?" message
    
    // Cleanup
    g.reset();
}

TEST(EatVerbFoodInRoom) {
    // Test EAT verb on food in the room (not in inventory)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create food object in room
    auto apple = std::make_unique<ZObject>(1, "apple");
    apple->addSynonym("apple");
    apple->setFlag(ObjectFlag::FOODBIT);
    apple->moveTo(g.here);  // In room, not inventory
    ZObject* applePtr = apple.get();
    g.registerObject(1, std::move(apple));
    
    // Verify apple is in room
    ASSERT_TRUE(applePtr->getLocation() == g.here);
    
    // Set up verb context
    g.prso = applePtr;
    g.prsa = V_EAT;
    
    // Test EAT verb
    bool result = Verbs::vEat();
    ASSERT_TRUE(result);
    
    // Verify apple was removed from game (consumed)
    ASSERT_TRUE(applePtr->getLocation() == nullptr);
    
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
