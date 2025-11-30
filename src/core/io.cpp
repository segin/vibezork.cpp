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
    // Process string character by character, preserving explicit newlines
    // while still doing word wrapping for long lines
    std::string word;
    
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        
        if (c == '\n') {
            // Output any pending word first
            if (!word.empty()) {
                if (currentColumn > 0 && currentColumn + word.length() + 1 > WRAP_WIDTH) {
                    std::cout << '\n';
                    currentColumn = 0;
                }
                if (currentColumn > 0) {
                    std::cout << ' ';
                    currentColumn++;
                }
                std::cout << word;
                currentColumn += word.length();
                word.clear();
            }
            // Output the newline
            std::cout << '\n';
            currentColumn = 0;
        } else if (c == ' ' || c == '\t') {
            // End of word - output it
            if (!word.empty()) {
                if (currentColumn > 0 && currentColumn + word.length() + 1 > WRAP_WIDTH) {
                    std::cout << '\n';
                    currentColumn = 0;
                }
                if (currentColumn > 0) {
                    std::cout << ' ';
                    currentColumn++;
                }
                std::cout << word;
                currentColumn += word.length();
                word.clear();
            }
        } else {
            // Add character to current word
            word += c;
        }
    }
    
    // Output any remaining word
    if (!word.empty()) {
        if (currentColumn > 0 && currentColumn + word.length() + 1 > WRAP_WIDTH) {
            std::cout << '\n';
            currentColumn = 0;
        }
        if (currentColumn > 0) {
            std::cout << ' ';
            currentColumn++;
        }
        std::cout << word;
        currentColumn += word.length();
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
