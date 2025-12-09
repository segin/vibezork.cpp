#include "score.h"

// External flag from actions.cpp for end game access
extern bool wonFlag;

ScoreSystem& ScoreSystem::instance() {
    static ScoreSystem instance;
    return instance;
}

// Requirement 52, 85: Add points to score
void ScoreSystem::addScore(int points) {
    score_ += points;
    if (score_ > MAX_SCORE) {
        score_ = MAX_SCORE;
    }
    
    // Trigger end game when max score reached (Requirement 70: Winnability)
    if (score_ >= MAX_SCORE && !wonFlag) {
        wonFlag = true;
    }
}

// Requirement 53: Increment move counter
void ScoreSystem::incrementMoves() {
    moves_++;
}

// Requirement 54: Get player's rank based on score
std::string_view ScoreSystem::getRank() const {
    return calculateRank();
}

// Requirement 54: Calculate rank based on score thresholds
std::string_view ScoreSystem::calculateRank() const {
    // Rank thresholds from original Zork I
    if (score_ >= 350) return "Master Adventurer";
    if (score_ >= 330) return "Wizard";
    if (score_ >= 300) return "Master";
    if (score_ >= 200) return "Adventurer";
    if (score_ >= 100) return "Junior Adventurer";
    if (score_ >= 50) return "Novice Adventurer";
    if (score_ >= 25) return "Amateur Adventurer";
    return "Beginner";
}

// Requirement 85: Mark treasure as scored to prevent double-scoring
void ScoreSystem::markTreasureScored(ObjectId treasureId) {
    scoredTreasures_.insert(treasureId);
}

// Requirement 85: Check if treasure has already been scored
bool ScoreSystem::isTreasureScored(ObjectId treasureId) const {
    return scoredTreasures_.find(treasureId) != scoredTreasures_.end();
}

// Reset score system for new game
void ScoreSystem::reset() {
    score_ = 0;
    moves_ = 0;
    scoredTreasures_.clear();
}
