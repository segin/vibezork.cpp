#include "io.h"
#include "object.h"
#include <iostream>
#include <string>

void printDesc(const ZObject* obj) {
    if (obj) {
        std::cout << obj->getDesc();
    }
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
