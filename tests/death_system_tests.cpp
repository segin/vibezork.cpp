#include "test_framework.h"
#include "systems/death.h"
#include "systems/score.h"
#include "core/globals.h"

// Test death system functionality
// Requirements: 58, 59

TEST(DeathCountTracking) {
    // Reset systems
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    ScoreSystem::instance().reset();
    
    // Initial death count should be 0
    ASSERT_EQ(DeathSystem::getDeathCount(), 0);
    ASSERT_FALSE(DeathSystem::isDead());
    
    // First death
    DeathSystem::jigsUp("Test death 1", DeathSystem::DeathCause::OTHER);
    ASSERT_EQ(DeathSystem::getDeathCount(), 1);
    
    // Second death
    DeathSystem::jigsUp("Test death 2", DeathSystem::DeathCause::OTHER);
    ASSERT_EQ(DeathSystem::getDeathCount(), 2);
}

TEST(ResurrectionLimit) {
    // Reset systems
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    ScoreSystem::instance().reset();
    
    // Can resurrect initially
    ASSERT_TRUE(DeathSystem::canResurrect());
    
    // First death - can still resurrect
    DeathSystem::jigsUp("Test death 1", DeathSystem::DeathCause::OTHER);
    ASSERT_TRUE(DeathSystem::canResurrect());
    
    // Second death - cannot resurrect anymore
    DeathSystem::jigsUp("Test death 2", DeathSystem::DeathCause::OTHER);
    ASSERT_FALSE(DeathSystem::canResurrect());
}

TEST(ScorePenalty) {
    // Reset systems
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    ScoreSystem::instance().reset();
    
    // Set initial score
    ScoreSystem::instance().addScore(50);
    ASSERT_EQ(ScoreSystem::instance().getScore(), 50);
    
    // Death should deduct 10 points
    DeathSystem::jigsUp("Test death", DeathSystem::DeathCause::COMBAT);
    ASSERT_EQ(ScoreSystem::instance().getScore(), 40);
}

TEST(DeathCauses) {
    // Reset systems
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    ScoreSystem::instance().reset();
    
    // Test combat death
    DeathSystem::jigsUp("Killed in combat", DeathSystem::DeathCause::COMBAT);
    ASSERT_EQ(DeathSystem::getDeathCount(), 1);
    
    // Reset for next test
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    
    // Test grue death
    DeathSystem::jigsUp("Eaten by grue", DeathSystem::DeathCause::GRUE);
    ASSERT_EQ(DeathSystem::getDeathCount(), 1);
    
    // Reset for next test
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    
    // Test falling death
    DeathSystem::jigsUp("Fell to death", DeathSystem::DeathCause::FALLING);
    ASSERT_EQ(DeathSystem::getDeathCount(), 1);
}

TEST(DeathReset) {
    // Reset systems
    DeathSystem::reset();
    DeathSystem::setTestMode(true);
    ScoreSystem::instance().reset();
    
    // Cause some deaths
    DeathSystem::jigsUp("Test death 1", DeathSystem::DeathCause::OTHER);
    DeathSystem::jigsUp("Test death 2", DeathSystem::DeathCause::OTHER);
    ASSERT_EQ(DeathSystem::getDeathCount(), 2);
    
    // Reset should clear death count
    DeathSystem::reset();
    ASSERT_EQ(DeathSystem::getDeathCount(), 0);
    ASSERT_FALSE(DeathSystem::isDead());
    ASSERT_TRUE(DeathSystem::canResurrect());
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
    
    std::cout << "\nTests: " << passed << " passed, " << failed << " failed" << std::endl;
    
    return failed > 0 ? 1 : 0;
}
