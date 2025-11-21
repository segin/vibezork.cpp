#pragma once
#include <string>
#include <sstream>
#include <vector>

// Helper class for testing game commands
class GameTester {
public:
    GameTester() {
        // Initialize game state
        initializeGame();
    }
    
    // Execute a command and capture output
    std::string execute(const std::string& command) {
        // Redirect cout to capture output
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        
        // Execute command through parser
        Parser parser;
        auto cmd = parser.parse(command);
        
        // Process command (simplified - would call actual game loop)
        // This needs to be integrated with the actual game engine
        
        std::cout.rdbuf(old);
        return buffer.str();
    }
    
    // Check if output contains expected text
    bool outputContains(const std::string& output, const std::string& expected) {
        return output.find(expected) != std::string::npos;
    }
    
    // Execute multiple commands
    std::vector<std::string> executeSequence(const std::vector<std::string>& commands) {
        std::vector<std::string> outputs;
        for (const auto& cmd : commands) {
            outputs.push_back(execute(cmd));
        }
        return outputs;
    }
    
private:
    void initializeGame();
};
