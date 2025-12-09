#include "core/globals.h"
#include "core/io.h"
#include "parser/parser.h"
#include "verbs/verbs.h"
#include "world/world.h"
#include "systems/npc.h"
#include "systems/timer.h"
#include "systems/lamp.h"
#include "systems/candle.h"
#include "systems/sword.h"
#include "systems/score.h"
#include <iostream>
#include <map>
#include <cstdlib>
#include <sstream>

// Verb dispatch table
std::map<VerbId, std::function<bool()>> verbHandlers = {
    {V_LOOK, Verbs::vLook},
    {V_INVENTORY, Verbs::vInventory},
    {V_QUIT, Verbs::vQuit},
    {V_RAISE, Verbs::vRaise},
    {V_MAKE, Verbs::vMake},
    {V_WIND, Verbs::vWind},
    {V_TAKE, Verbs::vTake},
    {V_DROP, Verbs::vDrop},
    {V_EXAMINE, Verbs::vExamine},
    {V_READ, Verbs::vRead},
    {V_OPEN, Verbs::vOpen},
    {V_CLOSE, Verbs::vClose},
    {V_WALK, Verbs::vWalk},
    {V_PUT, Verbs::vPut},
    {V_LOCK, Verbs::vLock},
    {V_UNLOCK, Verbs::vUnlock},
    {V_LOOK_INSIDE, Verbs::vLookInside},
    {V_SEARCH, Verbs::vSearch},
    {V_ENTER, Verbs::vEnter},
    {V_EXIT, Verbs::vExit},
    {V_CLIMB_UP, Verbs::vClimbUp},
    {V_CLIMB_DOWN, Verbs::vClimbDown},
    {V_BOARD, Verbs::vBoard},
    {V_DISEMBARK, Verbs::vDisembark},
    {V_TURN, Verbs::vTurn},
    {V_PUSH, Verbs::vPush},
    {V_PULL, Verbs::vPull},
    {V_MOVE, Verbs::vMove},
    {V_TIE, Verbs::vTie},
    {V_UNTIE, Verbs::vUntie},
    {V_LISTEN, Verbs::vListen},
    {V_SMELL, Verbs::vSmell},
    {V_TOUCH, Verbs::vTouch},
    {V_YELL, Verbs::vYell},
    {V_EAT, Verbs::vEat},
    {V_DRINK, Verbs::vDrink},
    {V_LAMP_ON, Verbs::vLampOn},
    {V_LAMP_OFF, Verbs::vLampOff},
    {V_INFLATE, Verbs::vInflate},
    {V_DEFLATE, Verbs::vDeflate},
    {V_PRAY, Verbs::vPray},
    {V_EXORCISE, Verbs::vExorcise},
    {V_WAVE, Verbs::vWave},
    {V_RUB, Verbs::vRub},
    {V_RING, Verbs::vRing},
    {V_ATTACK, Verbs::vAttack},
    {V_KILL, Verbs::vKill},
    {V_THROW, Verbs::vThrow},
    {V_SWING, Verbs::vSwing},
    {V_SCORE, Verbs::vScore},
    {V_DIAGNOSE, Verbs::vDiagnose},
    {V_VERBOSE, Verbs::vVerbose},
    {V_BRIEF, Verbs::vBrief},
    {V_SUPERBRIEF, Verbs::vSuperbrief},
    {V_SAVE, Verbs::vSave},
    {V_RESTORE, Verbs::vRestore},
    {V_RESTART, Verbs::vRestart},
    {V_VERSION, Verbs::vVersion},
    {V_TALK, Verbs::vTalk},
    {V_ASK, Verbs::vAsk},
    {V_TELL, Verbs::vTell},
    {V_ODYSSEUS, Verbs::vOdysseus},
    // Easter eggs / special words
    {V_HELLO, Verbs::vHello},
    {V_ZORK, Verbs::vZork},
    {V_PLUGH, Verbs::vPlugh},
    {V_FROBOZZ, Verbs::vFrobozz},
    // Additional common verbs
    {V_WAIT, Verbs::vWait},
    {V_SWIM, Verbs::vSwim},
    {V_BACK, Verbs::vBack},
    {V_JUMP, Verbs::vJump},
    {V_CURSE, Verbs::vCurse}
};

// Use global parser from parser_instance.cpp
// Parser& getGlobalParser() is declared in parser.h

void mainLoop1() {
    auto& g = Globals::instance();
    auto& score = ScoreSystem::instance();
    
    // Display status bar using ANSI escape codes
    // Save cursor, move to top, print in reverse video, restore
    std::string locationName = g.here ? g.here->getDesc() : "???";
    
    // Get terminal width (default 80 if unknown)
    int termWidth = 80;
    const char* cols = std::getenv("COLUMNS");
    if (cols) {
        termWidth = std::atoi(cols);
        if (termWidth < 40) termWidth = 80;
    }
    
    // Build status line content
    std::ostringstream status;
    status << " " << locationName;
    
    // Right-align score/moves
    std::ostringstream rightPart;
    rightPart << "Score: " << score.getScore() << "  Moves: " << score.getMoves() << " ";
    
    // Calculate padding
    int leftLen = status.str().length();
    int rightLen = rightPart.str().length();
    int padding = termWidth - leftLen - rightLen;
    if (padding < 1) padding = 1;
    
    // Print status bar with ANSI reverse video
    std::cout << "\033[7m";  // Reverse video on
    std::cout << status.str();
    for (int i = 0; i < padding; ++i) std::cout << ' ';
    std::cout << rightPart.str();
    std::cout << "\033[0m" << std::endl;  // Reset attributes
    
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
    ParsedCommand cmd = getGlobalParser().parse(input);
    
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
            
            // Execute the verb (C++17 if with initializer)
            if (auto it = verbHandlers.find(cmd.verb); it != verbHandlers.end()) {
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
        // Execute verb handler (C++17 if with initializer)
        if (auto it = verbHandlers.find(cmd.verb); it != verbHandlers.end()) {
            it->second();
        } else {
            printLine("That verb is not implemented yet.");
        }
    }
    
    g.moves++;
    
    // Process all timers (includes thief, troll, cyclops, lamp, etc.)
    TimerSystem::tick();
    
    // Process NPC actions that aren't timer-based
    NPCSystem::processTrollTurn();
    NPCSystem::processCyclopsTurn();
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
    NPCSystem::initializeThief();
    NPCSystem::initializeTroll();
    NPCSystem::initializeCyclops();
    LampSystem::initialize();  // Initialize lamp timer (Requirement 47)
    CandleSystem::initialize();  // Initialize candle timer (Requirement 48)
    SwordSystem::initialize();  // Initialize sword glow timer (Requirement 49)
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
