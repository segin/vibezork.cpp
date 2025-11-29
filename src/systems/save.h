#pragma once
#include "../core/types.h"
#include <string>
#include <string_view>
#include <optional>
#include <fstream>

// Save/Restore System for game state persistence
// Requirements: 60, 61
//
// The save system serializes all game state to a file:
// - Player location and inventory
// - All object locations and states (flags, properties)
// - Score and move count
// - Timer states
// - Display mode settings
//
// File format is a simple text-based format for portability and debugging:
// ZORK_SAVE_V1
// PLAYER_LOCATION=<id>
// SCORE=<value>
// MOVES=<value>
// OBJECT=<id>,<location>,<flags>,<properties>
// TIMER=<name>,<enabled>,<counter>
// ...

namespace SaveSystem {

// Error codes for save/restore operations
enum class SaveError {
    SUCCESS,
    FILE_NOT_FOUND,
    PERMISSION_DENIED,
    CORRUPT_FILE,
    VERSION_MISMATCH,
    WRITE_ERROR,
    READ_ERROR,
    INVALID_FORMAT
};

// Convert error code to human-readable message
std::string_view errorToString(SaveError error);

// Save game state to file
// Returns SaveError::SUCCESS on success, or an error code on failure
SaveError save(std::string_view filename);

// Restore game state from file
// Returns SaveError::SUCCESS on success, or an error code on failure
SaveError restore(std::string_view filename);

// Internal serialization functions
namespace detail {
    // Serialize game state to output stream
    bool serializeState(std::ofstream& out);
    
    // Deserialize game state from input stream
    bool deserializeState(std::ifstream& in);
    
    // Serialize a single object
    void serializeObject(std::ofstream& out, ObjectId id);
    
    // Deserialize a single object
    bool deserializeObject(std::ifstream& in, const std::string& line);
    
    // Serialize timer state
    void serializeTimer(std::ofstream& out, std::string_view name);
    
    // Deserialize timer state
    bool deserializeTimer(std::ifstream& in, const std::string& line);
}

} // namespace SaveSystem
