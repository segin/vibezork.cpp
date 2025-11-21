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
    {V_OPEN, Verbs::vOpen},
    {V_CLOSE, Verbs::vClose},
    {V_WALK, Verbs::vWalk}
};

void mainLoop1() {
    auto& g = Globals::instance();
    Parser parser;
    
    std::cout << "> ";
    std::string input = readLine();
    
    if (input.empty()) {
        return;
    }
    
    ParsedCommand cmd = parser.parse(input);
    
    if (cmd.verb == 0) {
        printLine("I don't understand that.");
        return;
    }
    
    g.prsa = cmd.verb;
    g.prso = cmd.directObj;
    g.prsi = cmd.indirectObj;
    
    // Handle direction commands
    if (cmd.isDirection) {
        Verbs::vWalkDir(cmd.direction);
    } else {
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
