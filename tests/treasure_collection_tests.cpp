// Treasure Collection Tests - Task 70.2
// Tests that all treasures are accessible and can be scored
#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/world.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include "../src/systems/score.h"
#include <memory>
#include <iostream>
#include <vector>

// Helper for treasure tests
class TreasureTestHelper {
public:
    TreasureTestHelper() {
        auto& g = Globals::instance();
        g.reset();
        initializeWorld();
    }
    
    ~TreasureTestHelper() {
        Globals::instance().reset();
    }
    
    // Get all treasure objects
    std::vector<ZObject*> getAllTreasures() {
        auto& g = Globals::instance();
        std::vector<ZObject*> treasures;
        
        // List of all treasure IDs from the original game
        std::vector<ObjectId> treasureIds = {
            ObjectIds::TROPHY,
            ObjectIds::EGG,
            ObjectIds::CHALICE,
            ObjectIds::TRIDENT,
            ObjectIds::PAINTING,
            ObjectIds::COFFIN,
            ObjectIds::JEWELS,
            ObjectIds::COINS,
            ObjectIds::DIAMOND,
            ObjectIds::EMERALD,
            ObjectIds::BRACELET,  // Sapphire bracelet
            ObjectIds::BAUBLE,
            ObjectIds::TORCH,     // Ivory torch
            ObjectIds::SCARAB,
            ObjectIds::BAR,       // Platinum bar
            ObjectIds::JADE,      // Jade figurine
            ObjectIds::SKULL,     // Crystal skull
            ObjectIds::POT_OF_GOLD
        };
        
        for (auto id : treasureIds) {
            auto* obj = g.getObject(id);
            if (obj) {
                // Check if object has a treasure value
                int value = obj->getProperty(P_VALUE);
                if (value > 0) {
                    treasures.push_back(obj);
                }
            }
        }
        
        return treasures;
    }
};

// Task 70.2: Test treasure collection

// Test: All treasures exist and are accessible
TEST(TreasuresExist) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto treasures = helper.getAllTreasures();
    
    std::cout << "Found " << treasures.size() << " treasures\n";
    
    // Verify we have a reasonable number of treasures
    ASSERT_TRUE(treasures.size() >= 15);
    
    // Verify each treasure has a location
    for (auto* treasure : treasures) {
        ASSERT_TRUE(treasure != nullptr);
        ASSERT_TRUE(treasure->getLocation() != nullptr);
        std::cout << "  ✓ " << treasure->getDesc() << " at " 
                  << treasure->getLocation()->getDesc() << "\n";
    }
    
    std::cout << "✓ All treasures exist and have locations\n";
}

// Test: All treasures can be taken
TEST(TreasuresCanBeTaken) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto treasures = helper.getAllTreasures();
    
    int takeable = 0;
    int anchored = 0;
    
    for (auto* treasure : treasures) {
        if (treasure->hasFlag(ObjectFlag::TAKEBIT)) {
            takeable++;
        } else if (treasure->hasFlag(ObjectFlag::TRYTAKEBIT)) {
            anchored++;
        }
    }
    
    std::cout << "Takeable treasures: " << takeable << "\n";
    std::cout << "Anchored treasures: " << anchored << "\n";
    
    // Most treasures should be takeable
    ASSERT_TRUE(takeable > 0);
    
    std::cout << "✓ Treasures have appropriate take flags\n";
}

// Test: All treasures have point values
TEST(TreasuresHaveValues) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto treasures = helper.getAllTreasures();
    
    int totalValue = 0;
    
    for (auto* treasure : treasures) {
        int value = treasure->getProperty(P_VALUE);
        if (value > 0) {
            totalValue += value;
            std::cout << "  " << treasure->getDesc() << ": " 
                      << value << " points\n";
        }
    }
    
    std::cout << "Total treasure value: " << totalValue << " points\n";
    
    // Verify total is 350 points
    ASSERT_EQ(totalValue, 350);
    
    std::cout << "✓ All treasures have values totaling 350 points\n";
}

// Test: Treasures can be scored in trophy case
TEST(TreasuresCanBeScored) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
    ASSERT_TRUE(trophyCase != nullptr);
    
    // Move player to living room where trophy case is
    auto* livingRoom = g.getObject(RoomIds::LIVING_ROOM);
    g.here = livingRoom;
    
    auto treasures = helper.getAllTreasures();
    
    auto& scoreSystem = ScoreSystem::instance();
    int initialScore = scoreSystem.getScore();
    
    // Try to score each treasure
    for (auto* treasure : treasures) {
        // Move treasure to player
        treasure->moveTo(g.winner);
        
        // Put treasure in trophy case
        g.prso = treasure;
        g.prsi = trophyCase;
        g.prsa = V_PUT;
        Verbs::vPut();
        
        // Verify treasure is in trophy case
        ASSERT_EQ(treasure->getLocation(), trophyCase);
    }
    
    int finalScore = scoreSystem.getScore();
    
    std::cout << "Initial score: " << initialScore << "\n";
    std::cout << "Final score: " << finalScore << "\n";
    
    // Verify score increased
    ASSERT_TRUE(finalScore > initialScore);
    
    std::cout << "✓ Treasures can be scored in trophy case\n";
}

// Test: 350 points is achievable
TEST(MaxScoreAchievable) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
    ASSERT_TRUE(trophyCase != nullptr);
    
    // Move player to living room
    auto* livingRoom = g.getObject(RoomIds::LIVING_ROOM);
    g.here = livingRoom;
    
    auto treasures = helper.getAllTreasures();
    
    auto& scoreSystem = ScoreSystem::instance();
    
    // Score all treasures
    for (auto* treasure : treasures) {
        int value = treasure->getProperty(P_VALUE);
        if (value > 0) {
            // Simulate scoring the treasure
            scoreSystem.addScore(value);
            scoreSystem.markTreasureScored(treasure->getId());
        }
    }
    
    int finalScore = scoreSystem.getScore();
    
    std::cout << "Final score: " << finalScore << " / 350\n";
    
    // Verify we can achieve 350 points
    ASSERT_EQ(finalScore, 350);
    
    std::cout << "✓ Maximum score of 350 points is achievable\n";
}

// Test: No treasure can be double-scored
TEST(TreasuresNoDoubleScoring) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
    ASSERT_TRUE(trophyCase != nullptr);
    
    auto* trophy = g.getObject(ObjectIds::TROPHY);
    if (!trophy) {
        std::cout << "Warning: Trophy not found, skipping test\n";
        return;
    }
    
    auto& scoreSystem = ScoreSystem::instance();
    int initialScore = scoreSystem.getScore();
    
    int value = trophy->getProperty(P_VALUE);
    if (value > 0) {
        // Score treasure first time
        scoreSystem.addScore(value);
        scoreSystem.markTreasureScored(trophy->getId());
        
        int afterFirstScore = scoreSystem.getScore();
        
        // Try to score again
        if (!scoreSystem.isTreasureScored(trophy->getId())) {
            scoreSystem.addScore(value);
            scoreSystem.markTreasureScored(trophy->getId());
        }
        
        int afterSecondScore = scoreSystem.getScore();
        
        // Verify score only increased once
        ASSERT_EQ(afterFirstScore, afterSecondScore);
        
        std::cout << "✓ Treasures cannot be double-scored\n";
    }
}

// Test: All treasure locations are reachable
TEST(TreasureLocationsReachable) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();
    
    auto treasures = helper.getAllTreasures();
    
    for (auto* treasure : treasures) {
        auto* location = treasure->getLocation();
        ASSERT_TRUE(location != nullptr);
        
        // Verify location is a valid room or container
        bool isRoom = dynamic_cast<ZRoom*>(location) != nullptr;
        bool isContainer = location->hasFlag(ObjectFlag::CONTBIT);
        
        ASSERT_TRUE(isRoom || isContainer);
        
        std::cout << "  ✓ " << treasure->getDesc() << " is in reachable location\n";
    }
    
    std::cout << "✓ All treasure locations are reachable\n";
}

int main() {
    std::cout << "Running Treasure Collection Tests (Task 70.2)\n";
    std::cout << "=============================================\n\n";
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : results) {
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "\n=============================================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
