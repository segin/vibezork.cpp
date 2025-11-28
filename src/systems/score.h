#pragma once
#include "../core/types.h"
#include <unordered_set>
#include <string_view>

// Score system for tracking player progress and achievements
// Requirements: 52, 53, 54, 85
class ScoreSystem {
public:
    static ScoreSystem& instance();
    
    // Score tracking (Requirement 52, 85)
    void addScore(int points);
    int getScore() const { return score_; }
    static constexpr int MAX_SCORE = 350;
    
    // Move counting (Requirement 53)
    void incrementMoves();
    int getMoves() const { return moves_; }
    
    // Rank calculation (Requirement 54)
    std::string_view getRank() const;
    
    // Treasure scoring (Requirement 85)
    void markTreasureScored(ObjectId treasureId);
    bool isTreasureScored(ObjectId treasureId) const;
    
    // Reset for new game
    void reset();
    
private:
    ScoreSystem() = default;
    
    int score_ = 0;
    int moves_ = 0;
    std::unordered_set<ObjectId> scoredTreasures_;
    
    std::string_view calculateRank() const;
};
