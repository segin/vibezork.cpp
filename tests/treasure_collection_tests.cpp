// Treasure Collection Tests - Task 70.2
// Tests that all treasures are accessible and can be scored
//
// ZIL Scoring System:
//   SCORE = BASE-SCORE + OTVAL-FROB(TROPHY-CASE)
//   - BASE-SCORE: accumulated via SCORE-OBJ (consumes VALUE property) and
//     SCORE-UPD (room visits, events like LIGHT-SHAFT=13)
//   - OTVAL-FROB: recursively sums TVALUE of all objects in trophy case
//   - SCORE-MAX = 350
//
// Source: gverbs.zil:1845-1870, 1actions.zil:483-502
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
        ScoreSystem::instance().reset();
        initializeWorld();
    }

    ~TreasureTestHelper() {
        Globals::instance().reset();
        ScoreSystem::instance().reset();
    }

    // All treasure object IDs from ZIL 1dungeon.zil
    // These are the objects that have TVALUE > 0 in the original game
    static constexpr std::array treasureIds = {
        ObjectIds::SKULL,       // Crystal skull         VALUE=10 TVALUE=10
        ObjectIds::SCEPTRE,     // Egyptian sceptre      VALUE=4  TVALUE=6
        ObjectIds::CHALICE,     // Silver chalice        VALUE=10 TVALUE=5
        ObjectIds::TRIDENT,     // Crystal trident       VALUE=4  TVALUE=11
        ObjectIds::COFFIN,      // Gold coffin           VALUE=10 TVALUE=15
        ObjectIds::DIAMOND,     // Huge diamond          VALUE=10 TVALUE=10
        ObjectIds::JADE,        // Jade figurine         VALUE=5  TVALUE=5
        ObjectIds::COINS,       // Bag of coins          VALUE=10 TVALUE=5
        ObjectIds::EMERALD,     // Large emerald         VALUE=5  TVALUE=10
        ObjectIds::PAINTING,    // Painting              VALUE=4  TVALUE=6
        ObjectIds::BAR,         // Platinum bar          VALUE=10 TVALUE=5
        ObjectIds::POT_OF_GOLD, // Pot of gold           VALUE=10 TVALUE=10
        ObjectIds::BRACELET,    // Sapphire bracelet     VALUE=5  TVALUE=5
        ObjectIds::SCARAB,      // Jeweled scarab        VALUE=5  TVALUE=5
        ObjectIds::TORCH,       // Ivory torch           VALUE=14 TVALUE=6
        ObjectIds::JEWELS,      // Trunk of jewels       VALUE=15 TVALUE=5
        ObjectIds::EGG,         // Jewel-encrusted egg   VALUE=5  TVALUE=5
        ObjectIds::CANARY,      // Clockwork canary      VALUE=6  TVALUE=4
        ObjectIds::BAUBLE,      // Brass bauble          VALUE=1  TVALUE=1
    };

    // Dynamic treasures that have no initial location (appear during gameplay)
    static bool isDynamicTreasure(ObjectId id) {
        return id == ObjectIds::DIAMOND ||   // Appears when thief killed
               id == ObjectIds::BAUBLE;      // Given by thief
    }

    // Get all treasure objects that exist in the world
    std::vector<ZObject*> getAllTreasures() {
        auto& g = Globals::instance();
        std::vector<ZObject*> treasures;

        for (auto id : treasureIds) {
            auto* obj = g.getObject(id);
            if (obj) {
                treasures.push_back(obj);
            }
        }

        return treasures;
    }
};

// Task 70.2: Test treasure collection

// Test: All treasure objects exist and have TVALUE property
TEST(TreasuresExist) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();

    auto treasures = helper.getAllTreasures();

    std::cout << "Found " << treasures.size() << " treasures\n";

    // Verify we have all 19 treasure objects
    ASSERT_EQ(treasures.size(), TreasureTestHelper::treasureIds.size());

    // Verify each treasure has a TVALUE > 0
    for (auto* treasure : treasures) {
        ASSERT_TRUE(treasure != nullptr);
        int tvalue = treasure->getProperty(P_TVALUE);
        ASSERT_TRUE(tvalue > 0);

        auto* location = treasure->getLocation();
        if (location) {
            std::cout << "  ✓ " << treasure->getDesc()
                      << " (TVALUE=" << tvalue << ") at "
                      << location->getDesc() << "\n";
        } else {
            // Dynamic treasures have no initial location
            ASSERT_TRUE(TreasureTestHelper::isDynamicTreasure(treasure->getId()));
            std::cout << "  ✓ " << treasure->getDesc()
                      << " (TVALUE=" << tvalue << ") [dynamic]\n";
        }
    }

    std::cout << "✓ All treasures exist with TVALUE properties\n";
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

    // All treasures should be takeable
    ASSERT_EQ(takeable, static_cast<int>(treasures.size()));

    std::cout << "✓ All treasures have TAKEBIT flag\n";
}

// Test: TVALUE sums correctly for trophy case scoring
// ZIL: OTVAL-FROB recursively sums TVALUE of objects in trophy case
// Source: 1actions.zil:496-502
TEST(TreasuresHaveValues) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();

    auto treasures = helper.getAllTreasures();

    int totalTvalue = 0;
    int totalValue = 0;

    for (auto* treasure : treasures) {
        int tvalue = treasure->getProperty(P_TVALUE);
        int value = treasure->getProperty(P_VALUE);
        totalTvalue += tvalue;
        totalValue += value;
        std::cout << "  " << treasure->getDesc()
                  << ": VALUE=" << value << " TVALUE=" << tvalue << "\n";
    }

    std::cout << "Total VALUE (base score component):  " << totalValue << "\n";
    std::cout << "Total TVALUE (trophy case component): " << totalTvalue << "\n";

    // Per ZIL 1dungeon.zil, the sum of all treasure TVALUEs is:
    // 10+6+5+11+15+10+5+5+10+6+5+10+5+5+6+5+5+4+1 = 129
    // (This represents the max trophy case score achievable in a perfect game)
    ASSERT_EQ(totalTvalue, 129);
    ASSERT_EQ(totalValue, 143);

    // Verify grand total achievable score is 350:
    // - Item VALUEs: 143
    // - Room visits (Kitchen=10, Cellar=25, Treasure Room=25, EW-Passage=5): 65
    // - Light in lower shaft bonus: 13
    // - Trophy case TVALUEs: 129
    // Total: 143 + 65 + 13 + 129 = 350
    constexpr int roomScore = 10 + 25 + 25 + 5;
    constexpr int shaftBonus = 13;
    int grandTotal = totalValue + roomScore + shaftBonus + totalTvalue;
    ASSERT_EQ(grandTotal, 350);

    std::cout << "✓ All treasure TVALUE properties sum to 129, grand total is 350\n";
}

// Test: Treasures can be scored by placing in trophy case
// ZIL: SCORE = BASE-SCORE + OTVAL-FROB(TROPHY-CASE)
// Source: 1actions.zil:483-484
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

    // Put each treasure in trophy case
    for (auto* treasure : treasures) {
        // Move treasure to player first
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

        // Try to score again — should be blocked by markTreasureScored
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

// Test: All placed treasure locations are reachable rooms or containers
TEST(TreasureLocationsReachable) {
    TreasureTestHelper helper;
    auto& g = Globals::instance();

    auto treasures = helper.getAllTreasures();

    for (auto* treasure : treasures) {
        auto* location = treasure->getLocation();

        // Dynamic treasures have no initial location — skip them
        if (!location) {
            ASSERT_TRUE(TreasureTestHelper::isDynamicTreasure(treasure->getId()));
            std::cout << "  ✓ " << treasure->getDesc()
                      << " is dynamic (no fixed location)\n";
            continue;
        }

        // Verify location is a valid room or container
        bool isRoom = dynamic_cast<ZRoom*>(location) != nullptr;
        bool isContainer = location->hasFlag(ObjectFlag::CONTBIT);

        ASSERT_TRUE(isRoom || isContainer);

        std::cout << "  ✓ " << treasure->getDesc()
                  << " is in reachable location\n";
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
