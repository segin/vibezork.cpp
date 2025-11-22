#include "core/globals.h"
#include "core/io.h"
#include "parser/parser.h"
#include "verbs/verbs.h"
#include "world/world.h"
#include <iostream>
#include <map>

// Verb dispatch table
std::map<VerbId, std::function<bool()>> verbHandlers = {
    {V_LOOK, Verbs::vLook},
    {V_INVENTORY, Verbs::vInventory},
    {V_QUIT, Verbs::vQuit},
    {V_TAKE, Verbs::vTake},
    {V_DROP, Verbs::vDrop},
    {V_EXAMINE, Verbs::vExamine},
    {V_READ, Verbs::vRead},
    {V_OPEN, Verbs::vOpen},
    {V_CLOSE, Verbs::vClose},
    {V_WALK, Verbs::vWalk}
};

// Global parser instance to maintain state across turns
static Parser globalParser;

void mainLoop1() {
    auto& g = Globals::instance();
    
    std::cout << "> ";
    std::string input = readLine();
    
    // Handle empty input gracefully (Requirement 72.1)
    if (input.empty()) {
        return;
    }
    
    // Trim whitespace (Requirement 72.5)
    size_t start = input.find_first_not_of(" \t\r\n");
    size_t end = input.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return;  // All whitespace
    }
    input = input.substr(start, end - start + 1);
    
    // Handle very long input gracefully (Requirement 72.2)
    if (input.length() > 1000) {
        printLine("That command is too long.");
        return;
    }
    
    // Parse the command
    ParsedCommand cmd = globalParser.parse(input);
    
    // Handle parse errors (Requirement 73)
    if (cmd.verb == 0) {
        // Error message already printed by parser
        return;
    }
    
    // Set global state for verb handlers
    g.prsa = cmd.verb;
    g.prso = cmd.directObj;
    g.prsi = cmd.indirectObj;
    
    // Handle "all" commands
    if (cmd.isAll) {
        if (cmd.allObjects.empty()) {
            printLine("There's nothing here to " + std::string(cmd.words[0]) + ".");
            return;
        }
        
        // Execute verb for each object
        for (auto* obj : cmd.allObjects) {
            g.prso = obj;
            
            // Display what we're doing
            print(obj->getDesc() + ": ");
            
            // Execute the verb
            auto it = verbHandlers.find(cmd.verb);
            if (it != verbHandlers.end()) {
                it->second();
            } else {
                printLine("That verb is not implemented yet.");
            }
        }
        
        g.moves++;
        return;
    }
    
    // Handle direction commands
    if (cmd.isDirection) {
        Verbs::vWalkDir(cmd.direction);
    } else {
        // Execute verb handler
        auto it = verbHandlers.find(cmd.verb);
        if (it != verbHandlers.end()) {
            it->second();
        } else {
            printLine("That verb is not implemented yet.");
        }
    }
    
    g.moves++;
}

void mainLoop() {
    while (true) {
        // Check for EOF before prompting
        if (std::cin.eof()) {
            printLine("\nGoodbye!");
            break;
        }
        
        mainLoop1();
    }
}

void initializeGame() {
    initializeWorld();
}

void go() {
    auto& g = Globals::instance();
    
    printLine("ZORK I: The Great Underground Empire");
    printLine("Copyright (c) 1981, 1982, 1983 Infocom, Inc. All rights reserved.");
    printLine("ZORK is a registered trademark of Infocom, Inc.");
    crlf();
    
    Verbs::vLook();
    
    mainLoop();
}

int main() {
    initializeGame();
    go();
    return 0;
}
