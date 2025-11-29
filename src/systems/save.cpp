#include "save.h"
#include "../core/globals.h"
#include "../core/object.h"
#include "timer.h"
#include "score.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace SaveSystem {

// File format version
constexpr const char* SAVE_VERSION = "ZORK_SAVE_V1";

std::string_view errorToString(SaveError error) {
    switch (error) {
        case SaveError::SUCCESS:
            return "Success";
        case SaveError::FILE_NOT_FOUND:
            return "File not found";
        case SaveError::PERMISSION_DENIED:
            return "Permission denied";
        case SaveError::CORRUPT_FILE:
            return "Corrupt save file";
        case SaveError::VERSION_MISMATCH:
            return "Save file version mismatch";
        case SaveError::WRITE_ERROR:
            return "Error writing to file";
        case SaveError::READ_ERROR:
            return "Error reading from file";
        case SaveError::INVALID_FORMAT:
            return "Invalid file format";
        default:
            return "Unknown error";
    }
}

// Requirement 60: Save game state to file
SaveError save(std::string_view filename) {
    std::ofstream out(std::string(filename), std::ios::out | std::ios::trunc);
    
    if (!out.is_open()) {
        return SaveError::PERMISSION_DENIED;
    }
    
    // Write version header
    out << SAVE_VERSION << "\n";
    
    // Serialize game state
    if (!detail::serializeState(out)) {
        out.close();
        return SaveError::WRITE_ERROR;
    }
    
    out.close();
    
    if (!out.good()) {
        return SaveError::WRITE_ERROR;
    }
    
    return SaveError::SUCCESS;
}

// Requirement 61: Restore game state from file
SaveError restore(std::string_view filename) {
    std::ifstream in(std::string(filename), std::ios::in);
    
    if (!in.is_open()) {
        return SaveError::FILE_NOT_FOUND;
    }
    
    // Read and verify version header
    std::string version;
    std::getline(in, version);
    
    if (version != SAVE_VERSION) {
        in.close();
        return SaveError::VERSION_MISMATCH;
    }
    
    // Deserialize game state
    if (!detail::deserializeState(in)) {
        in.close();
        return SaveError::CORRUPT_FILE;
    }
    
    in.close();
    
    return SaveError::SUCCESS;
}

namespace detail {

// Serialize game state to output stream
bool serializeState(std::ofstream& out) {
    auto& g = Globals::instance();
    auto& score = ScoreSystem::instance();
    
    // Requirement 60.2: Save player location and score
    if (g.here) {
        out << "PLAYER_LOCATION=" << g.here->getId() << "\n";
    }
    
    // Requirement 60.2: Save score and moves
    out << "SCORE=" << score.getScore() << "\n";
    out << "MOVES=" << score.getMoves() << "\n";
    
    // Save display mode settings
    out << "VERBOSE_MODE=" << (g.verboseMode ? 1 : 0) << "\n";
    out << "BRIEF_MODE=" << (g.briefMode ? 1 : 0) << "\n";
    out << "SUPERBRIEF_MODE=" << (g.superbriefMode ? 1 : 0) << "\n";
    
    // Save lamp state
    out << "LAMP_BATTERY=" << g.lampBattery << "\n";
    out << "LAMP_WARNED=" << (g.lampWarned ? 1 : 0) << "\n";
    
    // Save scored treasures
    // Note: We need to add a method to ScoreSystem to get scored treasures
    // For now, we'll skip this and rely on the treasure flags
    
    // Requirement 60.3: Save all object locations and states
    for (const auto& [id, obj] : g.getAllObjects()) {
        serializeObject(out, id);
    }
    
    // Requirement 60.4: Save all flag and property values (done in serializeObject)
    
    // Requirement 60.5: Save timer states
    auto& timerMgr = TimerSystem::TimerManager::instance();
    for (const auto& [name, timer] : timerMgr.getAllTimers()) {
        out << "TIMER=" << name << ","
            << (timer.enabled ? 1 : 0) << ","
            << timer.counter << "\n";
    }
    
    return out.good();
}

// Deserialize game state from input stream
bool deserializeState(std::ifstream& in) {
    auto& g = Globals::instance();
    auto& score = ScoreSystem::instance();
    
    std::string line;
    ObjectId playerLocation = 0;
    
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        
        // Parse key=value format
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Requirement 61.2: Restore player location and score
        if (key == "PLAYER_LOCATION") {
            playerLocation = std::stoi(value);
        }
        else if (key == "SCORE") {
            score.reset();
            score.addScore(std::stoi(value));
        }
        else if (key == "MOVES") {
            // Set moves directly (need to add setter to ScoreSystem)
            for (int i = 0; i < std::stoi(value); i++) {
                score.incrementMoves();
            }
        }
        else if (key == "VERBOSE_MODE") {
            g.verboseMode = (std::stoi(value) != 0);
        }
        else if (key == "BRIEF_MODE") {
            g.briefMode = (std::stoi(value) != 0);
        }
        else if (key == "SUPERBRIEF_MODE") {
            g.superbriefMode = (std::stoi(value) != 0);
        }
        else if (key == "LAMP_BATTERY") {
            g.lampBattery = std::stoi(value);
        }
        else if (key == "LAMP_WARNED") {
            g.lampWarned = (std::stoi(value) != 0);
        }
        // Requirement 61.3: Restore all object locations and states
        else if (key == "OBJECT") {
            if (!deserializeObject(in, line)) {
                return false;
            }
        }
        // Requirement 61.5: Restore timer states
        else if (key == "TIMER") {
            if (!deserializeTimer(in, line)) {
                return false;
            }
        }
    }
    
    // Set player location
    if (playerLocation != 0) {
        g.here = g.getObject(playerLocation);
    }
    
    return true;
}

// Serialize a single object
void serializeObject(std::ofstream& out, ObjectId id) {
    auto& g = Globals::instance();
    ZObject* obj = g.getObject(id);
    
    if (!obj) return;
    
    // Format: OBJECT=<id>,<location_id>,<flags>,<properties>
    out << "OBJECT=" << id << ",";
    
    // Save location
    if (obj->getLocation()) {
        out << obj->getLocation()->getId();
    } else {
        out << "0";
    }
    out << ",";
    
    // Save flags as hex value
    out << std::hex << obj->getAllFlags() << std::dec << ",";
    
    // Save properties
    // Format: prop1:val1;prop2:val2;...
    const auto& props = obj->getAllProperties();
    bool first = true;
    for (const auto& [propId, value] : props) {
        if (!first) out << ";";
        out << propId << ":" << value;
        first = false;
    }
    
    // Save text property if present
    if (obj->hasText()) {
        if (!first) out << ";";
        out << "TEXT:" << obj->getText();
    }
    
    out << "\n";
}

// Deserialize a single object
bool deserializeObject(std::ifstream& in, const std::string& line) {
    // Parse: OBJECT=<id>,<location_id>,<flags>,<properties>
    size_t eqPos = line.find('=');
    if (eqPos == std::string::npos) return false;
    
    std::string data = line.substr(eqPos + 1);
    std::istringstream ss(data);
    std::string token;
    
    // Parse object ID
    if (!std::getline(ss, token, ',')) return false;
    ObjectId objId = std::stoi(token);
    
    // Parse location ID
    if (!std::getline(ss, token, ',')) return false;
    ObjectId locId = std::stoi(token);
    
    // Get objects
    auto& g = Globals::instance();
    ZObject* obj = g.getObject(objId);
    ZObject* loc = (locId != 0) ? g.getObject(locId) : nullptr;
    
    if (!obj) return false;
    
    // Restore location
    if (loc) {
        obj->moveTo(loc);
    }
    
    // Parse and restore flags
    if (!std::getline(ss, token, ',')) return false;
    uint32_t flags = std::stoul(token, nullptr, 16);
    obj->setAllFlags(flags);
    
    // Parse and restore properties
    if (std::getline(ss, token)) {
        // Parse properties: prop1:val1;prop2:val2;...
        if (!token.empty()) {
            std::istringstream propStream(token);
            std::string propPair;
            
            while (std::getline(propStream, propPair, ';')) {
                size_t colonPos = propPair.find(':');
                if (colonPos != std::string::npos) {
                    std::string propKey = propPair.substr(0, colonPos);
                    std::string propValue = propPair.substr(colonPos + 1);
                    
                    // Handle TEXT property specially
                    if (propKey == "TEXT") {
                        obj->setText(propValue);
                    } else {
                        PropertyId propId = std::stoi(propKey);
                        int value = std::stoi(propValue);
                        obj->setProperty(propId, value);
                    }
                }
            }
        }
    }
    
    return true;
}

// Serialize timer state (now handled in serializeState)
void serializeTimer(std::ofstream& out, std::string_view name) {
    // This function is no longer needed as timers are serialized in serializeState
}

// Deserialize timer state
bool deserializeTimer(std::ifstream& in, const std::string& line) {
    // Parse: TIMER=<name>,<enabled>,<counter>
    size_t eqPos = line.find('=');
    if (eqPos == std::string::npos) return false;
    
    std::string data = line.substr(eqPos + 1);
    std::istringstream ss(data);
    std::string token;
    
    // Parse timer name
    if (!std::getline(ss, token, ',')) return false;
    std::string name = token;
    
    // Parse enabled state
    if (!std::getline(ss, token, ',')) return false;
    bool enabled = (std::stoi(token) != 0);
    
    // Parse counter
    if (!std::getline(ss, token, ',')) return false;
    int counter = std::stoi(token);
    
    // Restore timer state
    auto& timerMgr = TimerSystem::TimerManager::instance();
    timerMgr.setTimerState(name, enabled, counter);
    
    return true;
}

} // namespace detail

} // namespace SaveSystem
