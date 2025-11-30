// Integration Tests - Task 67
#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/world.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include "../src/systems/timer.h"
#include "../src/systems/score.h"
#include "../src/systems/combat.h"
#include "../src/systems/light.h"
#include "../src/systems/save.h"
#include <memory>
#include <iostream>

// Helper for integration tests
class IntegrationHelper {
public:
    IntegrationHelper() {
        auto& g = Globals::instance();
        g.reset();
        initializeWorld();
    }
    
    ~IntegrationHelper() {
        Globals::instance().reset();
    }
};

// Task 67.1: Test command sequences
TEST(CommandSequenceTakeAndDrop) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* lamp = g.getObject(ObjectIds::LAMP);
    ASSERT_TRUE(lamp != nullptr);
    
    g.prso = lamp;
    g.prsa = V_TAKE;
    Verbs::vTake();
    ASSERT_EQ(lamp->getLocation(), g.winner);
    
    g.prso = lamp;
    g.prsa = V_DROP;
    Verbs::vDrop();
    ASSERT_EQ(lamp->getLocation(), g.here);
}


TEST(CommandSequenceOpenAndTake) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* mailbox = g.getObject(ObjectIds::MAILBOX);
    auto* leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
    ASSERT_TRUE(mailbox != nullptr);
    ASSERT_TRUE(leaflet != nullptr);
    
    g.prso = mailbox;
    g.prsa = V_OPEN;
    Verbs::vOpen();
    ASSERT_TRUE(mailbox->hasFlag(ObjectFlag::OPENBIT));
    
    g.prso = leaflet;
    g.prsa = V_TAKE;
    Verbs::vTake();
    ASSERT_EQ(leaflet->getLocation(), g.winner);
}

TEST(CommandSequencePutInContainer) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* lamp = g.getObject(ObjectIds::LAMP);
    auto* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
    ASSERT_TRUE(lamp != nullptr);
    ASSERT_TRUE(trophyCase != nullptr);
    
    g.prso = lamp;
    g.prsa = V_TAKE;
    Verbs::vTake();
    
    g.prso = lamp;
    g.prsi = trophyCase;
    g.prsa = V_PUT;
    Verbs::vPut();
    ASSERT_EQ(lamp->getLocation(), trophyCase);
}

TEST(CommandSequenceLightManagement) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* lamp = g.getObject(ObjectIds::LAMP);
    ASSERT_TRUE(lamp != nullptr);
    
    g.prso = lamp;
    g.prsa = V_TAKE;
    Verbs::vTake();
    
    g.prso = lamp;
    g.prsa = V_LAMP_ON;
    Verbs::vLampOn();
    ASSERT_TRUE(lamp->hasFlag(ObjectFlag::ONBIT));
    
    g.prso = lamp;
    g.prsa = V_LAMP_OFF;
    Verbs::vLampOff();
    ASSERT_FALSE(lamp->hasFlag(ObjectFlag::ONBIT));
}


// Task 67.2: Test puzzle solutions
TEST(PuzzleMailboxLeaflet) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* mailbox = g.getObject(ObjectIds::MAILBOX);
    auto* leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
    ASSERT_TRUE(mailbox != nullptr);
    ASSERT_TRUE(leaflet != nullptr);
    
    g.prso = mailbox;
    g.prsa = V_OPEN;
    Verbs::vOpen();
    
    g.prso = leaflet;
    g.prsa = V_TAKE;
    Verbs::vTake();
    
    g.prso = leaflet;
    g.prsa = V_READ;
    Verbs::vRead();
}

// Task 67.3: Test NPC interactions
TEST(NPCThiefEncounter) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* thief = g.getObject(ObjectIds::THIEF);
    if (thief) {
        ASSERT_TRUE(thief->hasFlag(ObjectFlag::FIGHTBIT));
        thief->moveTo(g.here);
        ASSERT_EQ(thief->getLocation(), g.here);
    }
}

// Task 67.4: Test game state persistence
TEST(GameStateSaveRestore) {
    IntegrationHelper helper;
    auto& g = Globals::instance();
    
    auto* lamp = g.getObject(ObjectIds::LAMP);
    ASSERT_TRUE(lamp != nullptr);
    
    g.prso = lamp;
    g.prsa = V_TAKE;
    Verbs::vTake();
    
    g.prso = lamp;
    g.prsa = V_LAMP_ON;
    Verbs::vLampOn();
    
    auto saveResult = SaveSystem::save("test.sav");
    ASSERT_TRUE(saveResult == SaveSystem::SaveError::SUCCESS);
    
    lamp->clearFlag(ObjectFlag::ONBIT);
    lamp->moveTo(g.here);
    
    auto restoreResult = SaveSystem::restore("test.sav");
    ASSERT_TRUE(restoreResult == SaveSystem::SaveError::SUCCESS);
    
    ASSERT_EQ(lamp->getLocation(), g.winner);
    ASSERT_TRUE(lamp->hasFlag(ObjectFlag::ONBIT));
}

int main() {
    std::cout << "Running Integration Tests\n";
    std::cout << "=========================\n\n";
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : results) {
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "\n=========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
