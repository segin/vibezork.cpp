#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"

// Test world initialization - Task 24.4

TEST(WorldInitializationAllRoomsExist) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test that key rooms exist
    ASSERT_TRUE(g.getObject(ROOM_WEST_OF_HOUSE) != nullptr);
    ASSERT_TRUE(g.getObject(ROOM_NORTH_OF_HOUSE) != nullptr);
    ASSERT_TRUE(g.getObject(ROOM_SOUTH_OF_HOUSE) != nullptr);
    ASSERT_TRUE(g.getObject(ROOM_EAST_OF_HOUSE) != nullptr);
    ASSERT_TRUE(g.getObject(RoomIds::LIVING_ROOM) != nullptr);
    ASSERT_TRUE(g.getObject(RoomIds::KITCHEN) != nullptr);
    ASSERT_TRUE(g.getObject(RoomIds::ATTIC) != nullptr);
    ASSERT_TRUE(g.getObject(RoomIds::CELLAR) != nullptr);
    
    g.reset();
}

TEST(WorldInitializationAllObjectsExist) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test that key objects exist
    ASSERT_TRUE(g.getObject(ObjectIds::MAILBOX) != nullptr);
    ASSERT_TRUE(g.getObject(ObjectIds::LAMP) != nullptr);
    ASSERT_TRUE(g.getObject(ObjectIds::SWORD) != nullptr);
    ASSERT_TRUE(g.getObject(ObjectIds::TROPHY_CASE) != nullptr);
    ASSERT_TRUE(g.getObject(ObjectIds::ADVENTURER) != nullptr);
    
    g.reset();
}

TEST(WorldInitializationPlayerState) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test player object exists
    ASSERT_TRUE(g.player != nullptr);
    ASSERT_EQ(g.player->getId(), ObjectIds::ADVENTURER);
    
    // Test player is set as winner (current actor)
    ASSERT_TRUE(g.winner != nullptr);
    ASSERT_EQ(g.winner, g.player);
    
    // Test player location is West of House
    ASSERT_TRUE(g.player->getLocation() != nullptr);
    ASSERT_EQ(g.player->getLocation()->getId(), ROOM_WEST_OF_HOUSE);
    
    // Test player inventory is empty initially
    ASSERT_EQ(g.player->getContents().size(), 0);
    
    g.reset();
}

TEST(WorldInitializationCurrentRoom) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test current room is set to West of House
    ASSERT_TRUE(g.here != nullptr);
    ASSERT_EQ(g.here->getId(), ROOM_WEST_OF_HOUSE);
    
    g.reset();
}

TEST(WorldInitializationInitialFlags) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test West of House has correct flags
    ZObject* westOfHouse = g.getObject(ROOM_WEST_OF_HOUSE);
    ASSERT_TRUE(westOfHouse != nullptr);
    ASSERT_TRUE(westOfHouse->hasFlag(ObjectFlag::ONBIT));  // Lit
    ASSERT_TRUE(westOfHouse->hasFlag(ObjectFlag::RLANDBIT));  // On land
    ASSERT_TRUE(westOfHouse->hasFlag(ObjectFlag::SACREDBIT));  // Sacred (no fighting)
    
    // Test lamp has correct flags
    ZObject* lamp = g.getObject(ObjectIds::LAMP);
    ASSERT_TRUE(lamp != nullptr);
    ASSERT_TRUE(lamp->hasFlag(ObjectFlag::TAKEBIT));  // Can be taken
    ASSERT_TRUE(lamp->hasFlag(ObjectFlag::LIGHTBIT));  // Is a light source
    ASSERT_FALSE(lamp->hasFlag(ObjectFlag::ONBIT));  // Starts off
    
    // Test mailbox has correct flags
    ZObject* mailbox = g.getObject(ObjectIds::MAILBOX);
    ASSERT_TRUE(mailbox != nullptr);
    ASSERT_TRUE(mailbox->hasFlag(ObjectFlag::CONTBIT));  // Is a container
    ASSERT_TRUE(mailbox->hasFlag(ObjectFlag::TRYTAKEBIT));  // Cannot be taken (anchored)
    
    g.reset();
}

TEST(WorldInitializationObjectPlacements) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test lamp is in Living Room
    ZObject* lamp = g.getObject(ObjectIds::LAMP);
    ASSERT_TRUE(lamp != nullptr);
    ASSERT_TRUE(lamp->getLocation() != nullptr);
    ASSERT_EQ(lamp->getLocation()->getId(), RoomIds::LIVING_ROOM);
    
    // Test sword is in Living Room
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    ASSERT_TRUE(sword != nullptr);
    ASSERT_TRUE(sword->getLocation() != nullptr);
    ASSERT_EQ(sword->getLocation()->getId(), RoomIds::LIVING_ROOM);
    
    // Test mailbox is at West of House
    ZObject* mailbox = g.getObject(ObjectIds::MAILBOX);
    ASSERT_TRUE(mailbox != nullptr);
    ASSERT_TRUE(mailbox->getLocation() != nullptr);
    ASSERT_EQ(mailbox->getLocation()->getId(), ROOM_WEST_OF_HOUSE);
    
    g.reset();
}

TEST(WorldInitializationContainmentRelationships) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test mailbox contains leaflet (if leaflet is created)
    ZObject* mailbox = g.getObject(ObjectIds::MAILBOX);
    ASSERT_TRUE(mailbox != nullptr);
    
    // Test trophy case is empty initially
    ZObject* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
    ASSERT_TRUE(trophyCase != nullptr);
    ASSERT_EQ(trophyCase->getContents().size(), 0);
    
    g.reset();
}

TEST(WorldInitializationNoOrphanedObjects) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Check that important objects have locations
    // Some objects like GRUE, global scenery, etc. don't need locations
    
    // Check player has location
    ASSERT_TRUE(g.player != nullptr);
    ASSERT_TRUE(g.player->getLocation() != nullptr);
    
    // Check some key objects have locations
    ZObject* lamp = g.getObject(ObjectIds::LAMP);
    if (lamp != nullptr) {
        ASSERT_TRUE(lamp->getLocation() != nullptr);
    }
    
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    if (sword != nullptr) {
        ASSERT_TRUE(sword->getLocation() != nullptr);
    }
    
    ZObject* mailbox = g.getObject(ObjectIds::MAILBOX);
    if (mailbox != nullptr) {
        ASSERT_TRUE(mailbox->getLocation() != nullptr);
    }
    
    g.reset();
}

TEST(WorldInitializationGameStateVariables) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test initial game state
    ASSERT_TRUE(g.lit);  // West of House is lit
    ASSERT_EQ(g.score, 0);  // Score starts at 0
    ASSERT_EQ(g.moves, 0);  // Move count starts at 0
    
    g.reset();
}

TEST(WorldInitializationRoomExits) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test West of House has correct exits
    ZRoom* westOfHouse = dynamic_cast<ZRoom*>(g.getObject(ROOM_WEST_OF_HOUSE));
    ASSERT_TRUE(westOfHouse != nullptr);
    
    // Should have exit to North of House
    RoomExit* northExit = westOfHouse->getExit(Direction::NORTH);
    ASSERT_TRUE(northExit != nullptr);
    ASSERT_EQ(northExit->targetRoom, ROOM_NORTH_OF_HOUSE);
    
    // Should have exit to South of House
    RoomExit* southExit = westOfHouse->getExit(Direction::SOUTH);
    ASSERT_TRUE(southExit != nullptr);
    ASSERT_EQ(southExit->targetRoom, ROOM_SOUTH_OF_HOUSE);
    
    g.reset();
}

TEST(WorldInitializationTreasureValues) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test treasures have value properties set
    ZObject* trophy = g.getObject(ObjectIds::TROPHY);
    if (trophy != nullptr) {
        ASSERT_TRUE(trophy->getProperty(P_VALUE) > 0);
        ASSERT_TRUE(trophy->getProperty(P_TVALUE) > 0);
    }
    
    ZObject* chalice = g.getObject(ObjectIds::CHALICE);
    if (chalice != nullptr) {
        ASSERT_TRUE(chalice->getProperty(P_VALUE) > 0);
        ASSERT_TRUE(chalice->getProperty(P_TVALUE) > 0);
    }
    
    g.reset();
}

TEST(WorldInitializationNPCStrength) {
    auto& g = Globals::instance();
    g.reset();
    initializeWorld();
    
    // Test NPCs have strength properties
    ZObject* thief = g.getObject(ObjectIds::THIEF);
    if (thief != nullptr) {
        ASSERT_TRUE(thief->hasFlag(ObjectFlag::FIGHTBIT));
        ASSERT_TRUE(thief->getProperty(P_STRENGTH) > 0);
    }
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    if (troll != nullptr) {
        ASSERT_TRUE(troll->hasFlag(ObjectFlag::FIGHTBIT));
        ASSERT_TRUE(troll->getProperty(P_STRENGTH) > 0);
    }
    
    ZObject* cyclops = g.getObject(ObjectIds::CYCLOPS);
    if (cyclops != nullptr) {
        ASSERT_TRUE(cyclops->hasFlag(ObjectFlag::FIGHTBIT));
        ASSERT_TRUE(cyclops->getProperty(P_STRENGTH) > 0);
    }
    
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
    
    std::cout << "\n" << passed << " tests passed, " << failed << " tests failed\n";
    
    return failed > 0 ? 1 : 0;
}
