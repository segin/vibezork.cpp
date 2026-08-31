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

// Test that all 110 ZIL rooms are registered and properly formed in initializeWorld()
TEST(All110RoomsInitialized) {
    extern void initializeWorld();
    initializeWorld();
    auto& g = Globals::instance();

    std::vector<ObjectId> allRoomIds = {
        RoomIds::ARAGAIN_FALLS, RoomIds::ATLANTIS_ROOM, RoomIds::ATTIC,
        RoomIds::BAT_ROOM, RoomIds::CANYON_BOTTOM, RoomIds::CANYON_VIEW,
        RoomIds::CELLAR, RoomIds::CHASM_ROOM, RoomIds::CLEARING,
        RoomIds::CLIFF_MIDDLE, RoomIds::COLD_PASSAGE, RoomIds::CYCLOPS_ROOM,
        RoomIds::DAMP_CAVE, RoomIds::DAM_BASE, RoomIds::DAM_LOBBY,
        RoomIds::DAM_ROOM, RoomIds::DEAD_END_1, RoomIds::DEAD_END_2,
        RoomIds::DEAD_END_3, RoomIds::DEAD_END_4, RoomIds::DEAD_END_5,
        RoomIds::DEEP_CANYON, RoomIds::DOME_ROOM, RoomIds::EAST_OF_CHASM,
        RoomIds::EAST_OF_HOUSE, RoomIds::EGYPT_ROOM, RoomIds::END_OF_RAINBOW,
        RoomIds::ENGRAVINGS_CAVE, RoomIds::ENTRANCE_TO_HADES, RoomIds::EW_PASSAGE,
        RoomIds::FOREST_1, RoomIds::FOREST_2, RoomIds::FOREST_3,
        RoomIds::GALLERY, RoomIds::GAS_ROOM, RoomIds::GRATING_CLEARING,
        RoomIds::GRATING_ROOM, RoomIds::IN_STREAM, RoomIds::KITCHEN,
        RoomIds::LADDER_BOTTOM, RoomIds::LADDER_TOP, RoomIds::LAND_OF_LIVING_DEAD,
        RoomIds::LIVING_ROOM, RoomIds::LOUD_ROOM, RoomIds::LOWER_SHAFT,
        RoomIds::MACHINE_ROOM, RoomIds::MAINTENANCE_ROOM, RoomIds::MAZE_1,
        RoomIds::MAZE_10, RoomIds::MAZE_11, RoomIds::MAZE_12,
        RoomIds::MAZE_13, RoomIds::MAZE_14, RoomIds::MAZE_15,
        RoomIds::MAZE_2, RoomIds::MAZE_3, RoomIds::MAZE_4,
        RoomIds::MAZE_5, RoomIds::MAZE_6, RoomIds::MAZE_7,
        RoomIds::MAZE_8, RoomIds::MAZE_9, RoomIds::MINE_1,
        RoomIds::MINE_2, RoomIds::MINE_3, RoomIds::MINE_4,
        RoomIds::MINE_ENTRANCE, RoomIds::MIRROR_ROOM_1, RoomIds::MIRROR_ROOM_2,
        RoomIds::MOUNTAINS, RoomIds::NARROW_PASSAGE, RoomIds::NORTH_OF_HOUSE,
        RoomIds::NORTH_TEMPLE, RoomIds::NS_PASSAGE, RoomIds::ON_RAINBOW,
        RoomIds::PATH, RoomIds::RESERVOIR, RoomIds::RESERVOIR_NORTH,
        RoomIds::RESERVOIR_SOUTH, RoomIds::RIVER_1, RoomIds::RIVER_2,
        RoomIds::RIVER_3, RoomIds::RIVER_4, RoomIds::RIVER_5,
        RoomIds::ROUND_ROOM, RoomIds::SANDY_BEACH, RoomIds::SANDY_CAVE,
        RoomIds::SHAFT_ROOM, RoomIds::SHORE, RoomIds::SLIDE_ROOM,
        RoomIds::SMALL_CAVE, RoomIds::SMELLY_ROOM, RoomIds::SOUTH_OF_HOUSE,
        RoomIds::SOUTH_TEMPLE, RoomIds::SQUEEKY_ROOM, RoomIds::STONE_BARROW,
        RoomIds::STRANGE_PASSAGE, RoomIds::STREAM_VIEW, RoomIds::STUDIO,
        RoomIds::TIMBER_ROOM, RoomIds::TINY_CAVE, RoomIds::TORCH_ROOM,
        RoomIds::TREASURE_ROOM, RoomIds::TROLL_ROOM, RoomIds::TWISTING_PASSAGE,
        RoomIds::UP_A_TREE, RoomIds::WEST_OF_HOUSE, RoomIds::WHITE_CLIFFS_NORTH,
        RoomIds::WHITE_CLIFFS_SOUTH, RoomIds::WINDING_PASSAGE
    };

    ASSERT_EQ(allRoomIds.size(), 110);

    for (ObjectId id : allRoomIds) {
        ZObject* obj = g.getObject(id);
        ASSERT_TRUE(obj != nullptr);
        ZRoom* room = dynamic_cast<ZRoom*>(obj);
        ASSERT_TRUE(room != nullptr);
        ASSERT_FALSE(room->getDesc().empty());
    }
}

// Test River downstream and land connections
TEST(RiverNavigationFlow) {
    extern void initializeWorld();
    initializeWorld();
    auto& g = Globals::instance();

    auto* r1 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RIVER_1));
    auto* r2 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RIVER_2));
    auto* r3 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RIVER_3));
    auto* r4 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RIVER_4));
    auto* r5 = dynamic_cast<ZRoom*>(g.getObject(RoomIds::RIVER_5));

    ASSERT_TRUE(r1 != nullptr);
    ASSERT_TRUE(r2 != nullptr);
    ASSERT_TRUE(r3 != nullptr);
    ASSERT_TRUE(r4 != nullptr);
    ASSERT_TRUE(r5 != nullptr);

    // Downstream exits
    const auto* ex1 = r1->getExit(Direction::DOWN);
    ASSERT_TRUE(ex1 != nullptr);
    ASSERT_EQ(ex1->targetRoom, RoomIds::RIVER_2);

    const auto* ex2 = r2->getExit(Direction::DOWN);
    ASSERT_TRUE(ex2 != nullptr);
    ASSERT_EQ(ex2->targetRoom, RoomIds::RIVER_3);

    const auto* ex3 = r3->getExit(Direction::DOWN);
    ASSERT_TRUE(ex3 != nullptr);
    ASSERT_EQ(ex3->targetRoom, RoomIds::RIVER_4);

    const auto* ex4 = r4->getExit(Direction::DOWN);
    ASSERT_TRUE(ex4 != nullptr);
    ASSERT_EQ(ex4->targetRoom, RoomIds::RIVER_5);
}

// Test Canyon & Rainbow connection
TEST(CanyonAndRainbowFlow) {
    extern void initializeWorld();
    initializeWorld();
    auto& g = Globals::instance();

    auto* canyonView = dynamic_cast<ZRoom*>(g.getObject(RoomIds::CANYON_VIEW));
    auto* cliffMid = dynamic_cast<ZRoom*>(g.getObject(RoomIds::CLIFF_MIDDLE));
    auto* canyonBot = dynamic_cast<ZRoom*>(g.getObject(RoomIds::CANYON_BOTTOM));
    auto* endRainbow = dynamic_cast<ZRoom*>(g.getObject(RoomIds::END_OF_RAINBOW));
    auto* onRainbow = dynamic_cast<ZRoom*>(g.getObject(RoomIds::ON_RAINBOW));
    auto* falls = dynamic_cast<ZRoom*>(g.getObject(RoomIds::ARAGAIN_FALLS));

    ASSERT_TRUE(canyonView != nullptr);
    ASSERT_TRUE(cliffMid != nullptr);
    ASSERT_TRUE(canyonBot != nullptr);
    ASSERT_TRUE(endRainbow != nullptr);
    ASSERT_TRUE(onRainbow != nullptr);
    ASSERT_TRUE(falls != nullptr);

    // CanyonView DOWN -> CliffMiddle
    const auto* exDown = canyonView->getExit(Direction::DOWN);
    ASSERT_TRUE(exDown != nullptr);
    ASSERT_EQ(exDown->targetRoom, RoomIds::CLIFF_MIDDLE);

    // CliffMiddle DOWN -> CanyonBottom
    const auto* exClfBot = cliffMid->getExit(Direction::DOWN);
    ASSERT_TRUE(exClfBot != nullptr);
    ASSERT_EQ(exClfBot->targetRoom, RoomIds::CANYON_BOTTOM);

    // CanyonBottom NORTH -> EndOfRainbow
    const auto* exEndRb = canyonBot->getExit(Direction::NORTH);
    ASSERT_TRUE(exEndRb != nullptr);
    ASSERT_EQ(exEndRb->targetRoom, RoomIds::END_OF_RAINBOW);

    // EndOfRainbow UP -> OnRainbow
    const auto* exOnRb = endRainbow->getExit(Direction::UP);
    ASSERT_TRUE(exOnRb != nullptr);
    ASSERT_EQ(exOnRb->targetRoom, RoomIds::ON_RAINBOW);

    // OnRainbow EAST -> AragainFalls
    const auto* exFalls = onRainbow->getExit(Direction::EAST);
    ASSERT_TRUE(exFalls != nullptr);
    ASSERT_EQ(exFalls->targetRoom, RoomIds::ARAGAIN_FALLS);
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
