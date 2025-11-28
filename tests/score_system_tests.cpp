#include "test_framework.h"
#include "systems/score.h"
#include "core/globals.h"
#include <iostream>
#include <stdexcept>

// Helper macro for assertions with messages
#define TEST_ASSERT(condition, message) \
    if (!(condition)) throw std::runtime_error(message)

// Test score tracking (Requirement 52, 85)
void testScoreTracking() {
    auto& score = ScoreSystem::instance();
    score.reset();
    
    // Initial score should be 0
    TEST_ASSERT(score.getScore() == 0, "Initial score should be 0");
    
    // Add some points
    score.addScore(10);
    TEST_ASSERT(score.getScore() == 10, "Score should be 10 after adding 10 points");
    
    // Add more points
    score.addScore(25);
    TEST_ASSERT(score.getScore() == 35, "Score should be 35 after adding 25 more points");
    
    // Test maximum score cap
    score.addScore(400);
    TEST_ASSERT(score.getScore() == ScoreSystem::MAX_SCORE, 
                "Score should be capped at MAX_SCORE");
    
    std::cout << "✓ Score tracking test passed" << std::endl;
}

// Test move counting (Requirement 53)
void testMoveCounting() {
    auto& score = ScoreSystem::instance();
    score.reset();
    
    // Initial moves should be 0
    TEST_ASSERT(score.getMoves() == 0, "Initial moves should be 0");
    
    // Increment moves
    score.incrementMoves();
    TEST_ASSERT(score.getMoves() == 1, "Moves should be 1 after incrementing once");
    
    // Increment multiple times
    for (int i = 0; i < 10; i++) {
        score.incrementMoves();
    }
    TEST_ASSERT(score.getMoves() == 11, "Moves should be 11 after incrementing 10 more times");
    
    std::cout << "✓ Move counting test passed" << std::endl;
}

// Test rank calculation (Requirement 54)
void testRankCalculation() {
    auto& score = ScoreSystem::instance();
    score.reset();
    
    // Test Beginner rank (0 points)
    TEST_ASSERT(std::string(score.getRank()) == "Beginner", 
                "Rank should be Beginner at 0 points");
    
    // Test Amateur Adventurer rank (25 points)
    score.addScore(25);
    TEST_ASSERT(std::string(score.getRank()) == "Amateur Adventurer", 
                "Rank should be Amateur Adventurer at 25 points");
    
    // Test Novice Adventurer rank (50 points)
    score.addScore(25);
    TEST_ASSERT(std::string(score.getRank()) == "Novice Adventurer", 
                "Rank should be Novice Adventurer at 50 points");
    
    // Test Junior Adventurer rank (100 points)
    score.addScore(50);
    TEST_ASSERT(std::string(score.getRank()) == "Junior Adventurer", 
                "Rank should be Junior Adventurer at 100 points");
    
    // Test Adventurer rank (200 points)
    score.addScore(100);
    TEST_ASSERT(std::string(score.getRank()) == "Adventurer", 
                "Rank should be Adventurer at 200 points");
    
    // Test Master rank (300 points)
    score.addScore(100);
    TEST_ASSERT(std::string(score.getRank()) == "Master", 
                "Rank should be Master at 300 points");
    
    // Test Wizard rank (330 points)
    score.addScore(30);
    TEST_ASSERT(std::string(score.getRank()) == "Wizard", 
                "Rank should be Wizard at 330 points");
    
    // Test Master Adventurer rank (350 points)
    score.addScore(20);
    TEST_ASSERT(std::string(score.getRank()) == "Master Adventurer", 
                "Rank should be Master Adventurer at 350 points");
    
    std::cout << "✓ Rank calculation test passed" << std::endl;
}

// Test treasure scoring (Requirement 85)
void testTreasureScoring() {
    auto& score = ScoreSystem::instance();
    score.reset();
    
    // Test marking treasures as scored
    ObjectId trophy = 100;
    ObjectId egg = 101;
    
    // Initially, no treasures should be scored
    TEST_ASSERT(!score.isTreasureScored(trophy), 
                "Trophy should not be scored initially");
    TEST_ASSERT(!score.isTreasureScored(egg), 
                "Egg should not be scored initially");
    
    // Mark trophy as scored
    score.markTreasureScored(trophy);
    TEST_ASSERT(score.isTreasureScored(trophy), 
                "Trophy should be marked as scored");
    TEST_ASSERT(!score.isTreasureScored(egg), 
                "Egg should still not be scored");
    
    // Mark egg as scored
    score.markTreasureScored(egg);
    TEST_ASSERT(score.isTreasureScored(trophy), 
                "Trophy should still be scored");
    TEST_ASSERT(score.isTreasureScored(egg), 
                "Egg should now be scored");
    
    // Test that marking the same treasure twice doesn't cause issues
    score.markTreasureScored(trophy);
    TEST_ASSERT(score.isTreasureScored(trophy), 
                "Trophy should still be scored after marking twice");
    
    std::cout << "✓ Treasure scoring test passed" << std::endl;
}

// Test preventing double-scoring (Requirement 85)
void testPreventDoubleScoring() {
    auto& score = ScoreSystem::instance();
    score.reset();
    
    ObjectId treasure = 100;
    int treasureValue = 10;
    
    // Score treasure first time
    if (!score.isTreasureScored(treasure)) {
        score.addScore(treasureValue);
        score.markTreasureScored(treasure);
    }
    TEST_ASSERT(score.getScore() == treasureValue, 
                "Score should be 10 after scoring treasure once");
    
    // Try to score same treasure again - should not add points
    if (!score.isTreasureScored(treasure)) {
        score.addScore(treasureValue);
        score.markTreasureScored(treasure);
    }
    TEST_ASSERT(score.getScore() == treasureValue, 
                "Score should still be 10 after trying to score same treasure again");
    
    std::cout << "✓ Prevent double scoring test passed" << std::endl;
}

// Test reset functionality
void testScoreReset() {
    auto& score = ScoreSystem::instance();
    
    // First reset to ensure clean state
    score.reset();
    
    // Set up some state
    score.addScore(100);
    score.incrementMoves();
    score.incrementMoves();
    score.markTreasureScored(100);
    
    TEST_ASSERT(score.getScore() == 100, "Score should be 100 before reset");
    TEST_ASSERT(score.getMoves() == 2, "Moves should be 2 before reset");
    TEST_ASSERT(score.isTreasureScored(100), "Treasure 100 should be scored before reset");
    
    // Reset
    score.reset();
    
    // Everything should be back to initial state
    TEST_ASSERT(score.getScore() == 0, "Score should be 0 after reset");
    TEST_ASSERT(score.getMoves() == 0, "Moves should be 0 after reset");
    TEST_ASSERT(!score.isTreasureScored(100), "Treasure 100 should not be scored after reset");
    
    std::cout << "✓ Score reset test passed" << std::endl;
}

int main() {
    std::cout << "Running Score System Tests...\n\n";
    
    testScoreTracking();
    testMoveCounting();
    testRankCalculation();
    testTreasureScoring();
    testPreventDoubleScoring();
    testScoreReset();
    
    std::cout << "\nAll Score System Tests Passed!\n";
    return 0;
}
