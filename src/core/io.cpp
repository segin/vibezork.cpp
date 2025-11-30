#include "io.h"
#include "object.h"
#include <iostream>
#include <string>
#include <sstream>

// Track current column position for word wrapping
static int currentColumn = 0;

void printDesc(const ZObject* obj) {
    if (obj) {
        print(obj->getDesc());
    }
}

void print(const std::string& str) {
    std::istringstream iss(str);
    std::string word;
    bool firstWord = true;
    
    while (iss >> word) {
        int wordLen = word.length();
        
        // Check if we need to wrap
        if (currentColumn > 0 && currentColumn + wordLen + 1 > WRAP_WIDTH) {
            std::cout << '\n';
            currentColumn = 0;
        }
        
        // Add space before word (except at start of line)
        if (currentColumn > 0) {
            std::cout << ' ';
            currentColumn++;
        }
        
        std::cout << word;
        currentColumn += wordLen;
    }
    
    // Handle trailing whitespace and newlines in original string
    if (!str.empty() && str.back() == '\n') {
        std::cout << '\n';
        currentColumn = 0;
    }
}

void printLine(const std::string& str) {
    print(str);
    std::cout << std::endl;
    currentColumn = 0;
}

std::string readLine() {
    std::string line;
    
    // Handle EOF gracefully (Requirement 72.4)
    if (!std::getline(std::cin, line)) {
        if (std::cin.eof()) {
            // EOF encountered (Ctrl+D on Unix, Ctrl+Z on Windows)
            // Return empty string to signal end
            return "";
        }
        // Other input error
        std::cin.clear();
        return "";
    }
    
    return line;
}
