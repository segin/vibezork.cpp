#include "test_framework.h"
#include "core/object.h"
#include "core/globals.h"
#include "world/rooms.h"
#include "verbs/verbs.h"
#include <memory>

// Test basic directional movement
TEST(BasicDirectionalMovement) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Set up exit from room1 to room2
    room1->setExit(Direction::NORTH, RoomExit(2));
    
    // Register rooms
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    // Start in room 1
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Move north
    Verbs::vWalkDir(Direction::NORTH);
    
    // Should now be in room 2
    ASSERT_EQ(g.here, r2);
    ASSERT_EQ(g.winner->getLocation(), r2);
}

// Test blocked exits with messages
TEST(BlockedExits) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create room with blocked exit
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    room1->setExit(Direction::NORTH, RoomExit("A wall blocks your path."));
    
    ZRoom* r1 = room1.get();
    g.registerObject(1, std::move(room1));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Try to move north - should stay in room 1
    Verbs::vWalkDir(Direction::NORTH);
    
    ASSERT_EQ(g.here, r1);
}

// Test door exits
TEST(DoorExits) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Create door object
    auto door = std::make_unique<ZObject>(50, "door");
    door->setFlag(ObjectFlag::DOORBIT);
    door->setFlag(ObjectFlag::OPENBIT);  // Door starts open
    
    ZObject* doorPtr = door.get();
    g.registerObject(50, std::move(door));
    
    // Set up door exit
    room1->setExit(Direction::NORTH, RoomExit::createDoor(2, 50));
    
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Move through open door - should succeed
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r2);
    
    // Go back
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Close the door
    doorPtr->clearFlag(ObjectFlag::OPENBIT);
    
    // Try to move through closed door - should fail
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r1);
}

// Test locked door exits
TEST(LockedDoorExits) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Create locked door
    auto door = std::make_unique<ZObject>(50, "door");
    door->setFlag(ObjectFlag::DOORBIT);
    door->setFlag(ObjectFlag::LOCKEDBIT);  // Door is locked
    
    g.registerObject(50, std::move(door));
    
    // Set up door exit
    room1->setExit(Direction::NORTH, RoomExit::createDoor(2, 50));
    
    ZRoom* r1 = room1.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Try to move through locked door - should fail
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r1);
}

// Test conditional exits
TEST(ConditionalExits) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Create a flag to control the condition
    bool puzzleSolved = false;
    
    // Set up conditional exit
    room1->setExit(Direction::NORTH, RoomExit::createConditional(
        2,
        [&puzzleSolved]() { return puzzleSolved; },
        "You need to solve the puzzle first."
    ));
    
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Try to move before puzzle is solved - should fail
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r1);
    
    // Solve the puzzle
    puzzleSolved = true;
    
    // Try to move after puzzle is solved - should succeed
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r2);
}

// Test exits requiring items
TEST(ExitsRequiringItems) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Create key item
    auto key = std::make_unique<ZObject>(50, "key");
    key->setFlag(ObjectFlag::TAKEBIT);
    
    ZObject* keyPtr = key.get();
    g.registerObject(50, std::move(key));
    
    // Set up exit requiring key
    room1->setExit(Direction::NORTH, RoomExit::createRequiresItem(
        2, 50, "You need a key to proceed."
    ));
    
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Try to move without key - should fail
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r1);
    
    // Pick up the key
    keyPtr->moveTo(g.winner);
    
    // Try to move with key - should succeed
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r2);
}

// Test special movement (CLIMB)
TEST(SpecialMovementClimb) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Ground", "You are on the ground.");
    auto room2 = std::make_unique<ZRoom>(2, "Tree Top", "You are in a tree.");
    
    // Set up special exit requiring CLIMB
    room1->setExit(Direction::UP, RoomExit::createSpecial(
        2, V_CLIMB_UP, "You need to climb to go up."
    ));
    
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Try normal movement - should fail
    Verbs::vWalkDir(Direction::UP);
    ASSERT_EQ(g.here, r1);
    
    // Use CLIMB verb - should succeed
    Verbs::vClimbUp();
    ASSERT_EQ(g.here, r2);
}

// Test one-way exits
TEST(OneWayExits) {
    auto& g = Globals::instance();
    g.reset();
    
    // Create two rooms
    auto room1 = std::make_unique<ZRoom>(1, "Room 1", "You are in room 1.");
    auto room2 = std::make_unique<ZRoom>(2, "Room 2", "You are in room 2.");
    
    // Set up one-way exit from room1 to room2
    room1->setExit(Direction::NORTH, RoomExit::createOneWay(2));
    // No exit back from room2 to room1
    
    ZRoom* r1 = room1.get();
    ZRoom* r2 = room2.get();
    g.registerObject(1, std::move(room1));
    g.registerObject(2, std::move(room2));
    
    // Create player
    auto player = std::make_unique<ZObject>(100, "player");
    g.winner = player.get();
    g.registerObject(100, std::move(player));
    
    g.here = r1;
    g.winner->moveTo(r1);
    
    // Move north - should succeed
    Verbs::vWalkDir(Direction::NORTH);
    ASSERT_EQ(g.here, r2);
    
    // Try to move south - should fail (no exit)
    Verbs::vWalkDir(Direction::SOUTH);
    ASSERT_EQ(g.here, r2);  // Still in room 2
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
    
    std::cout << "\n" << passed << " tests passed, " << failed << " tests failed\n";
    
    return failed > 0 ? 1 : 0;
}
