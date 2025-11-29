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

// Test PUT verb - Task 28.3

TEST(PutVerbBasic) {
    // Test putting an object in a container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create container (box) in room
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->setProperty(2, 100);  // P_CAPACITY = 2
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create coin object in player inventory
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(1, 5);  // P_SIZE = 1
    coin->moveTo(g.winner);
    ZObject* coinPtr = coin.get();
    g.registerObject(2, std::move(coin));
    
    // Set up verb context
    g.prso = coinPtr;
    g.prsi = boxPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify coin is now in the box
    ASSERT_EQ(coinPtr->getLocation(), boxPtr);
    
    // Cleanup
    g.reset();
}

TEST(PutVerbClosedContainer) {
    // Test putting in a closed container (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Note: OPENBIT is NOT set (closed)
    chest->setProperty(2, 100);  // P_CAPACITY
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Create gem object in player inventory
    auto gem = std::make_unique<ZObject>(2, "gem");
    gem->addSynonym("gem");
    gem->setFlag(ObjectFlag::TAKEBIT);
    gem->setProperty(1, 5);  // P_SIZE
    gem->moveTo(g.winner);
    ZObject* gemPtr = gem.get();
    g.registerObject(2, std::move(gem));
    
    // Set up verb context
    g.prso = gemPtr;
    g.prsi = chestPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb on closed container
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify gem is still in player inventory (not moved)
    ASSERT_EQ(gemPtr->getLocation(), g.winner);
    
    // Should display "The chest is closed." message
    
    // Cleanup
    g.reset();
}

TEST(PutVerbCapacityLimit) {
    // Test capacity limits (Requirement 64)
    // ZIL formula: (WEIGHT(container) + WEIGHT(object) - SIZE(container)) > CAPACITY
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create small container (bag) with limited capacity
    auto bag = std::make_unique<ZObject>(1, "bag");
    bag->addSynonym("bag");
    bag->setFlag(ObjectFlag::CONTBIT);
    bag->setFlag(ObjectFlag::OPENBIT);
    bag->setProperty(P_CAPACITY, 10);  // Capacity = 10
    bag->setProperty(P_SIZE, 3);       // Bag itself weighs 3
    bag->moveTo(g.here);
    ZObject* bagPtr = bag.get();
    g.registerObject(1, std::move(bag));
    
    // Create small item already in bag
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(P_SIZE, 5);  // Coin weighs 5
    coin->moveTo(bagPtr);
    g.registerObject(2, std::move(coin));
    
    // Create large item in player inventory
    // Formula: (bagWeight + rockWeight - bagSize) > capacity
    // bagWeight = 3 (bag) + 5 (coin) = 8
    // rockWeight = 8
    // bagSize = 3
    // (8 + 8 - 3) = 13 > 10, so this should fail
    auto rock = std::make_unique<ZObject>(3, "rock");
    rock->addSynonym("rock");
    rock->setFlag(ObjectFlag::TAKEBIT);
    rock->setProperty(P_SIZE, 8);  // Rock weighs 8
    rock->moveTo(g.winner);
    ZObject* rockPtr = rock.get();
    g.registerObject(3, std::move(rock));
    
    // Set up verb context
    g.prso = rockPtr;
    g.prsi = bagPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb with capacity exceeded
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify rock is still in player inventory (not moved)
    ASSERT_EQ(rockPtr->getLocation(), g.winner);
    
    // Should display "There's no room." message
    
    // Cleanup
    g.reset();
}

TEST(PutVerbCapacityWithNestedContainers) {
    // Test capacity calculation with nested containers (Requirement 64)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create outer container (sack)
    auto sack = std::make_unique<ZObject>(1, "sack");
    sack->addSynonym("sack");
    sack->setFlag(ObjectFlag::CONTBIT);
    sack->setFlag(ObjectFlag::OPENBIT);
    sack->setProperty(P_CAPACITY, 20);
    sack->setProperty(P_SIZE, 5);
    sack->moveTo(g.here);
    ZObject* sackPtr = sack.get();
    g.registerObject(1, std::move(sack));
    
    // Create inner container (box) inside sack
    auto box = std::make_unique<ZObject>(2, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->setFlag(ObjectFlag::OPENBIT);
    box->setFlag(ObjectFlag::TAKEBIT);
    box->setProperty(P_CAPACITY, 10);
    box->setProperty(P_SIZE, 3);
    box->moveTo(sackPtr);
    ZObject* boxPtr = box.get();
    g.registerObject(2, std::move(box));
    
    // Create item inside box
    auto gem = std::make_unique<ZObject>(3, "gem");
    gem->addSynonym("gem");
    gem->setFlag(ObjectFlag::TAKEBIT);
    gem->setProperty(P_SIZE, 2);
    gem->moveTo(boxPtr);
    g.registerObject(3, std::move(gem));
    
    // Create item to add to sack
    // sackWeight = 5 (sack) + 3 (box) + 2 (gem) = 10
    // coinWeight = 12
    // sackSize = 5
    // (10 + 12 - 5) = 17 <= 20, so this should succeed
    auto coin = std::make_unique<ZObject>(4, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(P_SIZE, 12);
    coin->moveTo(g.winner);
    ZObject* coinPtr = coin.get();
    g.registerObject(4, std::move(coin));
    
    // Set up verb context
    g.prso = coinPtr;
    g.prsi = sackPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb - should succeed
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify coin was moved to sack
    ASSERT_EQ(coinPtr->getLocation(), sackPtr);
    
    // Cleanup
    g.reset();
}

TEST(PutVerbCapacityExactFit) {
    // Test putting an object that exactly fills remaining capacity (Requirement 64)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create container
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    chest->setFlag(ObjectFlag::OPENBIT);
    chest->setProperty(P_CAPACITY, 10);
    chest->setProperty(P_SIZE, 5);
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Create item that exactly fits
    // chestWeight = 5
    // itemWeight = 10
    // chestSize = 5
    // (5 + 10 - 5) = 10 <= 10, so this should succeed (exact fit)
    auto item = std::make_unique<ZObject>(2, "item");
    item->addSynonym("item");
    item->setFlag(ObjectFlag::TAKEBIT);
    item->setProperty(P_SIZE, 10);
    item->moveTo(g.winner);
    ZObject* itemPtr = item.get();
    g.registerObject(2, std::move(item));
    
    // Set up verb context
    g.prso = itemPtr;
    g.prsi = chestPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb - should succeed
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify item was moved to chest
    ASSERT_EQ(itemPtr->getLocation(), chestPtr);
    
    // Cleanup
    g.reset();
}

TEST(PutVerbNonContainer) {
    // Test putting in a non-container (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-container object (lamp) in room
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    // Note: CONTBIT is NOT set (not a container)
    lamp->moveTo(g.here);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create coin object in player inventory
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(1, 5);  // P_SIZE
    coin->moveTo(g.winner);
    ZObject* coinPtr = coin.get();
    g.registerObject(2, std::move(coin));
    
    // Set up verb context
    g.prso = coinPtr;
    g.prsi = lampPtr;
    g.prsa = V_PUT;
    
    // Test PUT verb on non-container
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify coin is still in player inventory (not moved)
    ASSERT_EQ(coinPtr->getLocation(), g.winner);
    
    // Should display "You can't put something in that." message
    
    // Cleanup
    g.reset();
}

TEST(PutVerbNoObject) {
    // Test PUT verb without specifying an object
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
    g.prsa = V_PUT;
    
    // Test PUT verb without object
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Should display "Put what?" message
    
    // Cleanup
    g.reset();
}

TEST(PutVerbNoContainer) {
    // Test PUT verb without specifying a container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create coin object in player inventory
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->moveTo(g.winner);
    ZObject* coinPtr = coin.get();
    g.registerObject(2, std::move(coin));
    
    // Set up verb context with object but no container
    g.prso = coinPtr;
    g.prsi = nullptr;
    g.prsa = V_PUT;
    
    // Test PUT verb without container
    bool result = Verbs::vPut();
    ASSERT_TRUE(result);
    
    // Verify coin is still in player inventory
    ASSERT_EQ(coinPtr->getLocation(), g.winner);
    
    // Should display "Put it in what?" message
    
    // Cleanup
    g.reset();
}

TEST(PutVerbMultipleObjects) {
    // Test putting multiple objects in a container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create container (sack) in room
    auto sack = std::make_unique<ZObject>(1, "sack");
    sack->addSynonym("sack");
    sack->setFlag(ObjectFlag::CONTBIT);
    sack->setFlag(ObjectFlag::OPENBIT);
    sack->setProperty(2, 100);  // P_CAPACITY
    sack->moveTo(g.here);
    ZObject* sackPtr = sack.get();
    g.registerObject(1, std::move(sack));
    
    // Create first object in player inventory
    auto coin = std::make_unique<ZObject>(2, "coin");
    coin->addSynonym("coin");
    coin->setFlag(ObjectFlag::TAKEBIT);
    coin->setProperty(1, 5);  // P_SIZE
    coin->moveTo(g.winner);
    ZObject* coinPtr = coin.get();
    g.registerObject(2, std::move(coin));
    
    // Create second object in player inventory
    auto gem = std::make_unique<ZObject>(3, "gem");
    gem->addSynonym("gem");
    gem->setFlag(ObjectFlag::TAKEBIT);
    gem->setProperty(1, 5);  // P_SIZE
    gem->moveTo(g.winner);
    ZObject* gemPtr = gem.get();
    g.registerObject(3, std::move(gem));
    
    // Put coin in sack
    g.prso = coinPtr;
    g.prsi = sackPtr;
    g.prsa = V_PUT;
    bool result1 = Verbs::vPut();
    ASSERT_TRUE(result1);
    ASSERT_EQ(coinPtr->getLocation(), sackPtr);
    
    // Put gem in sack
    g.prso = gemPtr;
    g.prsi = sackPtr;
    g.prsa = V_PUT;
    bool result2 = Verbs::vPut();
    ASSERT_TRUE(result2);
    ASSERT_EQ(gemPtr->getLocation(), sackPtr);
    
    // Verify both objects are in the sack
    const auto& sackContents = sackPtr->getContents();
    ASSERT_EQ(sackContents.size(), 2);
    
    // Cleanup
    g.reset();
}

// Test OPEN and CLOSE verbs - Task 29.5

TEST(OpenVerbBasic) {
    // Test opening a closed container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Note: OPENBIT is NOT set (closed)
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set up verb context
    g.prso = chestPtr;
    g.prsa = V_OPEN;
    
    // Test OPEN verb
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Verify chest is now open
    ASSERT_TRUE(chestPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Cleanup
    g.reset();
}

TEST(OpenVerbWithContents) {
    // Test opening a container with contents
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container (box) in room
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create item inside the box
    auto gem = std::make_unique<ZObject>(2, "gem");
    gem->addSynonym("gem");
    gem->moveTo(boxPtr);
    g.registerObject(2, std::move(gem));
    
    // Set up verb context
    g.prso = boxPtr;
    g.prsa = V_OPEN;
    
    // Test OPEN verb
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Verify box is now open
    ASSERT_TRUE(boxPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Should display contents
    
    // Cleanup
    g.reset();
}

TEST(OpenVerbAlreadyOpen) {
    // Test opening an already open container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container (sack) in room
    auto sack = std::make_unique<ZObject>(1, "sack");
    sack->addSynonym("sack");
    sack->setFlag(ObjectFlag::CONTBIT);
    sack->setFlag(ObjectFlag::OPENBIT);  // Already open
    sack->moveTo(g.here);
    ZObject* sackPtr = sack.get();
    g.registerObject(1, std::move(sack));
    
    // Set up verb context
    g.prso = sackPtr;
    g.prsa = V_OPEN;
    
    // Test OPEN verb on already open container
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Verify sack is still open
    ASSERT_TRUE(sackPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Should display "It's already open." message
    
    // Cleanup
    g.reset();
}

TEST(OpenVerbLockedContainer) {
    // Test opening a locked container (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container (safe) in room
    auto safe = std::make_unique<ZObject>(1, "safe");
    safe->addSynonym("safe");
    safe->setFlag(ObjectFlag::CONTBIT);
    safe->setFlag(ObjectFlag::LOCKEDBIT);  // Locked
    safe->moveTo(g.here);
    ZObject* safePtr = safe.get();
    g.registerObject(1, std::move(safe));
    
    // Set up verb context
    g.prso = safePtr;
    g.prsa = V_OPEN;
    
    // Test OPEN verb on locked container
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Verify safe is still closed (OPENBIT not set)
    ASSERT_FALSE(safePtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Should display "The safe is locked." message
    
    // Cleanup
    g.reset();
}

TEST(OpenVerbNonContainer) {
    // Test opening a non-container (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-container object (lamp) in room
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    // Note: CONTBIT is NOT set (not a container)
    lamp->moveTo(g.here);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsa = V_OPEN;
    
    // Test OPEN verb on non-container
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Should display "You can't open that." message
    
    // Cleanup
    g.reset();
}

TEST(OpenVerbNoObject) {
    // Test OPEN verb without specifying an object
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
    g.prsa = V_OPEN;
    
    // Test OPEN verb without object
    bool result = Verbs::vOpen();
    ASSERT_TRUE(result);
    
    // Should display "Open what?" message
    
    // Cleanup
    g.reset();
}

TEST(CloseVerbBasic) {
    // Test closing an open container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create open container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    chest->setFlag(ObjectFlag::OPENBIT);  // Open
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set up verb context
    g.prso = chestPtr;
    g.prsa = V_CLOSE;
    
    // Test CLOSE verb
    bool result = Verbs::vClose();
    ASSERT_TRUE(result);
    
    // Verify chest is now closed
    ASSERT_FALSE(chestPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Cleanup
    g.reset();
}

TEST(CloseVerbAlreadyClosed) {
    // Test closing an already closed container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container (box) in room
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    // Note: OPENBIT is NOT set (closed)
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Set up verb context
    g.prso = boxPtr;
    g.prsa = V_CLOSE;
    
    // Test CLOSE verb on already closed container
    bool result = Verbs::vClose();
    ASSERT_TRUE(result);
    
    // Verify box is still closed
    ASSERT_FALSE(boxPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Should display "It's already closed." message
    
    // Cleanup
    g.reset();
}

TEST(CloseVerbNonContainer) {
    // Test closing a non-container (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-container object (sword) in room
    auto sword = std::make_unique<ZObject>(1, "sword");
    sword->addSynonym("sword");
    // Note: CONTBIT is NOT set (not a container)
    sword->moveTo(g.here);
    ZObject* swordPtr = sword.get();
    g.registerObject(1, std::move(sword));
    
    // Set up verb context
    g.prso = swordPtr;
    g.prsa = V_CLOSE;
    
    // Test CLOSE verb on non-container
    bool result = Verbs::vClose();
    ASSERT_TRUE(result);
    
    // Should display "You can't close that." message
    
    // Cleanup
    g.reset();
}

TEST(CloseVerbNoObject) {
    // Test CLOSE verb without specifying an object
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
    g.prsa = V_CLOSE;
    
    // Test CLOSE verb without object
    bool result = Verbs::vClose();
    ASSERT_TRUE(result);
    
    // Should display "Close what?" message
    
    // Cleanup
    g.reset();
}

TEST(OpenCloseSequence) {
    // Test opening and closing a container in sequence
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create closed container (trunk) in room
    auto trunk = std::make_unique<ZObject>(1, "trunk");
    trunk->addSynonym("trunk");
    trunk->setFlag(ObjectFlag::CONTBIT);
    trunk->moveTo(g.here);
    ZObject* trunkPtr = trunk.get();
    g.registerObject(1, std::move(trunk));
    
    // Verify initially closed
    ASSERT_FALSE(trunkPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Open the trunk
    g.prso = trunkPtr;
    g.prsa = V_OPEN;
    bool result1 = Verbs::vOpen();
    ASSERT_TRUE(result1);
    ASSERT_TRUE(trunkPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Close the trunk
    g.prso = trunkPtr;
    g.prsa = V_CLOSE;
    bool result2 = Verbs::vClose();
    ASSERT_TRUE(result2);
    ASSERT_FALSE(trunkPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Open again
    g.prso = trunkPtr;
    g.prsa = V_OPEN;
    bool result3 = Verbs::vOpen();
    ASSERT_TRUE(result3);
    ASSERT_TRUE(trunkPtr->hasFlag(ObjectFlag::OPENBIT));
    
    // Cleanup
    g.reset();
}

// Test LOCK and UNLOCK verbs - Task 30.5

TEST(LockVerbBasic) {
    // Test locking an unlocked container with a key
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lockable container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    // Note: LOCKEDBIT is NOT set (unlocked)
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Set up verb context
    g.prso = chestPtr;
    g.prsi = keyPtr;
    g.prsa = V_LOCK;
    
    // Test LOCK verb
    bool result = Verbs::vLock();
    ASSERT_TRUE(result);
    
    // Verify chest is now locked
    ASSERT_TRUE(chestPtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Cleanup
    g.reset();
}

TEST(LockVerbAlreadyLocked) {
    // Test locking an already locked container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container (safe) in room
    auto safe = std::make_unique<ZObject>(1, "safe");
    safe->addSynonym("safe");
    safe->setFlag(ObjectFlag::CONTBIT);
    safe->setFlag(ObjectFlag::LOCKEDBIT);  // Already locked
    safe->moveTo(g.here);
    ZObject* safePtr = safe.get();
    g.registerObject(1, std::move(safe));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Set up verb context
    g.prso = safePtr;
    g.prsi = keyPtr;
    g.prsa = V_LOCK;
    
    // Test LOCK verb on already locked container
    bool result = Verbs::vLock();
    ASSERT_TRUE(result);
    
    // Verify safe is still locked
    ASSERT_TRUE(safePtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Should display "It's already locked." message
    
    // Cleanup
    g.reset();
}

TEST(LockVerbWithoutKey) {
    // Test locking without a key (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lockable container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set up verb context without key
    g.prso = chestPtr;
    g.prsi = nullptr;
    g.prsa = V_LOCK;
    
    // Test LOCK verb without key
    bool result = Verbs::vLock();
    ASSERT_TRUE(result);
    
    // Verify chest is still unlocked
    ASSERT_FALSE(chestPtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Should display "Lock it with what?" message
    
    // Cleanup
    g.reset();
}

TEST(LockVerbNonLockable) {
    // Test locking a non-lockable object (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create non-lockable object (lamp) in room
    auto lamp = std::make_unique<ZObject>(1, "lamp");
    lamp->addSynonym("lamp");
    // Note: Neither DOORBIT nor CONTBIT is set
    lamp->moveTo(g.here);
    ZObject* lampPtr = lamp.get();
    g.registerObject(1, std::move(lamp));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Set up verb context
    g.prso = lampPtr;
    g.prsi = keyPtr;
    g.prsa = V_LOCK;
    
    // Test LOCK verb on non-lockable object
    bool result = Verbs::vLock();
    ASSERT_TRUE(result);
    
    // Should display "You can't lock that." message
    
    // Cleanup
    g.reset();
}

TEST(UnlockVerbBasic) {
    // Test unlocking a locked container with a key
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    chest->setFlag(ObjectFlag::LOCKEDBIT);  // Locked
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Set up verb context
    g.prso = chestPtr;
    g.prsi = keyPtr;
    g.prsa = V_UNLOCK;
    
    // Test UNLOCK verb
    bool result = Verbs::vUnlock();
    ASSERT_TRUE(result);
    
    // Verify chest is now unlocked
    ASSERT_FALSE(chestPtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Cleanup
    g.reset();
}

TEST(UnlockVerbNotLocked) {
    // Test unlocking an already unlocked container
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create unlocked container (box) in room
    auto box = std::make_unique<ZObject>(1, "box");
    box->addSynonym("box");
    box->setFlag(ObjectFlag::CONTBIT);
    // Note: LOCKEDBIT is NOT set (unlocked)
    box->moveTo(g.here);
    ZObject* boxPtr = box.get();
    g.registerObject(1, std::move(box));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Set up verb context
    g.prso = boxPtr;
    g.prsi = keyPtr;
    g.prsa = V_UNLOCK;
    
    // Test UNLOCK verb on unlocked container
    bool result = Verbs::vUnlock();
    ASSERT_TRUE(result);
    
    // Verify box is still unlocked
    ASSERT_FALSE(boxPtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Should display "It's not locked." message
    
    // Cleanup
    g.reset();
}

TEST(UnlockVerbWithoutKey) {
    // Test unlocking without a key (should fail)
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create locked container (chest) in room
    auto chest = std::make_unique<ZObject>(1, "chest");
    chest->addSynonym("chest");
    chest->setFlag(ObjectFlag::CONTBIT);
    chest->setFlag(ObjectFlag::LOCKEDBIT);
    chest->moveTo(g.here);
    ZObject* chestPtr = chest.get();
    g.registerObject(1, std::move(chest));
    
    // Set up verb context without key
    g.prso = chestPtr;
    g.prsi = nullptr;
    g.prsa = V_UNLOCK;
    
    // Test UNLOCK verb without key
    bool result = Verbs::vUnlock();
    ASSERT_TRUE(result);
    
    // Verify chest is still locked
    ASSERT_TRUE(chestPtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Should display "Unlock it with what?" message
    
    // Cleanup
    g.reset();
}

TEST(LockUnlockSequence) {
    // Test locking and unlocking a container in sequence
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create lockable container (safe) in room
    auto safe = std::make_unique<ZObject>(1, "safe");
    safe->addSynonym("safe");
    safe->setFlag(ObjectFlag::CONTBIT);
    safe->moveTo(g.here);
    ZObject* safePtr = safe.get();
    g.registerObject(1, std::move(safe));
    
    // Create key in player inventory
    auto key = std::make_unique<ZObject>(2, "key");
    key->addSynonym("key");
    key->setFlag(ObjectFlag::TOOLBIT);
    key->moveTo(g.winner);
    ZObject* keyPtr = key.get();
    g.registerObject(2, std::move(key));
    
    // Verify initially unlocked
    ASSERT_FALSE(safePtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Lock the safe
    g.prso = safePtr;
    g.prsi = keyPtr;
    g.prsa = V_LOCK;
    bool result1 = Verbs::vLock();
    ASSERT_TRUE(result1);
    ASSERT_TRUE(safePtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Unlock the safe
    g.prso = safePtr;
    g.prsi = keyPtr;
    g.prsa = V_UNLOCK;
    bool result2 = Verbs::vUnlock();
    ASSERT_TRUE(result2);
    ASSERT_FALSE(safePtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Lock again
    g.prso = safePtr;
    g.prsi = keyPtr;
    g.prsa = V_LOCK;
    bool result3 = Verbs::vLock();
    ASSERT_TRUE(result3);
    ASSERT_TRUE(safePtr->hasFlag(ObjectFlag::LOCKEDBIT));
    
    // Cleanup
    g.reset();
}

// Test manipulation verbs (TURN, PUSH, PULL, MOVE) - Task 31.5

TEST(TurnVerbBasic) {
    // Test TURN verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create object (dial) in room
    auto dial = std::make_unique<ZObject>(1, "dial");
    dial->addSynonym("dial");
    dial->moveTo(g.here);
    ZObject* dialPtr = dial.get();
    g.registerObject(1, std::move(dial));
    
    // Set up verb context
    g.prso = dialPtr;
    g.prsa = V_TURN;
    
    // Test TURN verb
    bool result = Verbs::vTurn();
    ASSERT_TRUE(result);
    
    // Should display "Nothing obvious happens." message
    
    // Cleanup
    g.reset();
}

TEST(TurnVerbNoObject) {
    // Test TURN verb without specifying an object
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
    g.prsa = V_TURN;
    
    // Test TURN verb without object
    bool result = Verbs::vTurn();
    ASSERT_TRUE(result);
    
    // Should display "Turn what?" message
    
    // Cleanup
    g.reset();
}

TEST(PushVerbBasic) {
    // Test PUSH verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create object (button) in room
    auto button = std::make_unique<ZObject>(1, "button");
    button->addSynonym("button");
    button->moveTo(g.here);
    ZObject* buttonPtr = button.get();
    g.registerObject(1, std::move(button));
    
    // Set up verb context
    g.prso = buttonPtr;
    g.prsa = V_PUSH;
    
    // Test PUSH verb
    bool result = Verbs::vPush();
    ASSERT_TRUE(result);
    
    // Should display "Nothing obvious happens." message
    
    // Cleanup
    g.reset();
}

TEST(PushVerbNoObject) {
    // Test PUSH verb without specifying an object
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
    g.prsa = V_PUSH;
    
    // Test PUSH verb without object
    bool result = Verbs::vPush();
    ASSERT_TRUE(result);
    
    // Should display "Push what?" message
    
    // Cleanup
    g.reset();
}

TEST(PullVerbBasic) {
    // Test PULL verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create object (lever) in room
    auto lever = std::make_unique<ZObject>(1, "lever");
    lever->addSynonym("lever");
    lever->moveTo(g.here);
    ZObject* leverPtr = lever.get();
    g.registerObject(1, std::move(lever));
    
    // Set up verb context
    g.prso = leverPtr;
    g.prsa = V_PULL;
    
    // Test PULL verb
    bool result = Verbs::vPull();
    ASSERT_TRUE(result);
    
    // Should display "Nothing obvious happens." message
    
    // Cleanup
    g.reset();
}

TEST(PullVerbNoObject) {
    // Test PULL verb without specifying an object
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
    g.prsa = V_PULL;
    
    // Test PULL verb without object
    bool result = Verbs::vPull();
    ASSERT_TRUE(result);
    
    // Should display "Pull what?" message
    
    // Cleanup
    g.reset();
}

TEST(MoveVerbBasic) {
    // Test MOVE verb on an object
    auto& g = Globals::instance();
    
    // Create test room
    auto testRoom = std::make_unique<ZRoom>(100, "Test Room", "A test room.");
    g.here = testRoom.get();
    g.registerObject(100, std::move(testRoom));
    
    // Create player
    auto player = std::make_unique<ZObject>(999, "player");
    g.winner = player.get();
    g.registerObject(999, std::move(player));
    
    // Create object (rug) in room
    auto rug = std::make_unique<ZObject>(1, "rug");
    rug->addSynonym("rug");
    rug->moveTo(g.here);
    ZObject* rugPtr = rug.get();
    g.registerObject(1, std::move(rug));
    
    // Set up verb context
    g.prso = rugPtr;
    g.prsa = V_MOVE;
    
    // Test MOVE verb
    bool result = Verbs::vMove();
    ASSERT_TRUE(result);
    
    // Should display "Nothing obvious happens." message
    
    // Cleanup
    g.reset();
}

TEST(MoveVerbNoObject) {
    // Test MOVE verb without specifying an object
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
    g.prsa = V_MOVE;
    
    // Test MOVE verb without object
    bool result = Verbs::vMove();
    ASSERT_TRUE(result);
    
    // Should display "Move what?" message
    
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
