#include "test_framework.h"
#include "core/globals.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "systems/score.h"
#include "verbs/verbs.h"

// Forward declare action handler
extern bool trophyCaseAction();

TEST(TrophyCaseStoreScore) {
    auto& g = Globals::instance();
    auto& score = ScoreSystem::instance();
    
    // Reset
    g.reset();
    score.reset(); 
    
    // Create and register required objects
    auto livingRoom = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "A living room.");
    g.registerObject(RoomIds::LIVING_ROOM, std::move(livingRoom));
    
    auto player = std::make_unique<ZObject>(ObjectIds::ADVENTURER, "player");
    g.winner = player.get();
    g.registerObject(ObjectIds::ADVENTURER, std::move(player));
    
    auto caseObj = std::make_unique<ZObject>(ObjectIds::TROPHY_CASE, "trophy case");
    caseObj->setFlag(ObjectFlag::CONTBIT);
    caseObj->setFlag(ObjectFlag::OPENBIT); // Make it open so we can put things in
    caseObj->setAction(trophyCaseAction); // Important: Set the action handler!
    g.registerObject(ObjectIds::TROPHY_CASE, std::move(caseObj));
    
    auto egg = std::make_unique<ZObject>(ObjectIds::EGG, "egg");
    egg->setFlag(ObjectFlag::TAKEBIT);
    egg->setProperty(P_VALUE, 5); // 5 points for taking
    egg->setProperty(P_TVALUE, 5); // 5 points for storing
    g.registerObject(ObjectIds::EGG, std::move(egg));
    
    // Get pointers
    ZObject* eggPtr = g.getObject(ObjectIds::EGG);
    ZObject* casePtr = g.getObject(ObjectIds::TROPHY_CASE);
    ZObject* roomPtr = g.getObject(RoomIds::LIVING_ROOM);
    
    // Use pointers
    g.winner->moveTo(roomPtr);
    casePtr->moveTo(roomPtr);
    eggPtr->moveTo(g.winner);
    g.here = roomPtr;
    g.prso = eggPtr;
    g.prsi = casePtr;
    g.prsa = V_PUT;
    
    // Initial verification
    ASSERT_EQ(score.getScore(), 0);
    ASSERT_FALSE(score.isTreasureScored(eggPtr->getId()));
    
    // Execute command - calling the generic vPut which triggers action handlers
    Verbs::vPut();
    
    // Verify egg is in case
    ASSERT_EQ(eggPtr->getLocation(), casePtr);
    
    // Verify score increased (assert > 0)
    ASSERT_TRUE(score.getScore() > 0);
    ASSERT_TRUE(score.isTreasureScored(eggPtr->getId()));
}

TEST(TrophyCaseFixed) {
    auto& g = Globals::instance();
    // Reset
    g.reset();
    
    auto livingRoom = std::make_unique<ZRoom>(RoomIds::LIVING_ROOM, "Living Room", "A living room.");
    g.registerObject(RoomIds::LIVING_ROOM, std::move(livingRoom));
    
    auto player = std::make_unique<ZObject>(ObjectIds::ADVENTURER, "player");
    g.winner = player.get();
    g.registerObject(ObjectIds::ADVENTURER, std::move(player));
    
    auto caseObj = std::make_unique<ZObject>(ObjectIds::TROPHY_CASE, "trophy case");
    caseObj->setAction(trophyCaseAction);
    g.registerObject(ObjectIds::TROPHY_CASE, std::move(caseObj));

    ZObject* casePtr = g.getObject(ObjectIds::TROPHY_CASE);
    ZObject* roomPtr = g.getObject(RoomIds::LIVING_ROOM);
    
    g.winner->moveTo(roomPtr);
    casePtr->moveTo(roomPtr);
    g.here = roomPtr;
    
    // Try to take the case
    g.prsa = V_TAKE;
    g.prso = casePtr;
    g.prsi = nullptr;
    
    // Execute action handler directly first to verify it blocks take
    bool handled = trophyCaseAction();
    ASSERT_TRUE(handled);
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
