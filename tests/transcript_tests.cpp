// Transcript-based integration tests
// These tests replay actual game sessions to verify correctness
//
// Test data is based on documented playthroughs and walkthroughs
// Commands are extracted and expected outputs verified

#include "test_framework.h"
#include "transcript_data.h"
#include "../src/core/globals.h"
#include "../src/core/io.h"
#include "../src/parser/parser.h"
#include "../src/verbs/verbs.h"
#include "../src/world/world.h"
#include "../src/systems/timer.h"
#include "../src/systems/npc.h"
#include "../src/systems/lamp.h"
#include "../src/systems/candle.h"
#include "../src/systems/sword.h"
#include <sstream>
#include <iostream>
#include <map>

// Verb dispatch table (same as main.cpp)
static std::map<VerbId, std::function<bool()>> verbHandlers = {
    {V_LOOK, Verbs::vLook},
    {V_INVENTORY, Verbs::vInventory},
    {V_QUIT, Verbs::vQuit},
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
    {V_ODYSSEUS, Verbs::vOdysseus}
};

// Global parser for transcript tests
static Parser transcriptParser;

// Execute a single command and capture output
std::string executeCommand(const std::string& command) {
    auto& g = Globals::instance();
    
    // Capture output
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
    
    // Parse the command
    ParsedCommand cmd = transcriptParser.parse(command);
    
    // Handle parse errors
    if (cmd.verb == 0) {
        std::cout.rdbuf(oldCout);
        return buffer.str();
    }
    
    // Set global state for verb handlers
    g.prsa = cmd.verb;
    g.prso = cmd.directObj;
    g.prsi = cmd.indirectObj;
    
    // Handle "all" commands
    if (cmd.isAll) {
        if (cmd.allObjects.empty()) {
            printLine("There's nothing here to " + std::string(cmd.words[0]) + ".");
        } else {
            for (auto* obj : cmd.allObjects) {
                g.prso = obj;
                print(obj->getDesc() + ": ");
                
                auto it = verbHandlers.find(cmd.verb);
                if (it != verbHandlers.end()) {
                    it->second();
                }
            }
        }
        
        g.moves++;
        std::cout.rdbuf(oldCout);
        return buffer.str();
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
    
    // Process timers
    TimerSystem::tick();
    
    // Restore cout
    std::cout.rdbuf(oldCout);
    
    return buffer.str();
}

// Helper to check if output contains expected text (case-insensitive)
bool outputContains(const std::string& output, const std::string& expected) {
    std::string lowerOutput = output;
    std::string lowerExpected = expected;
    
    // Convert to lowercase
    for (char& c : lowerOutput) c = std::tolower(c);
    for (char& c : lowerExpected) c = std::tolower(c);
    
    return lowerOutput.find(lowerExpected) != std::string::npos;
}

// Helper to run a transcript sequence
void runTranscript(const std::vector<TranscriptStep>& steps, const std::string& testName = "") {
    if (!testName.empty()) {
        std::cout << "Running transcript: " << testName << "\n";
    }
    
    int stepNum = 0;
    for (const auto& step : steps) {
        stepNum++;
        
        // Execute command
        std::string output = executeCommand(step.command);
        
        // Verify expected outputs
        for (const auto& expected : step.expectedOutputs) {
            if (!outputContains(output, expected)) {
                std::cout << "Step " << stepNum << " FAILED\n";
                std::cout << "Command: " << step.command << "\n";
                std::cout << "Expected phrase: " << expected << "\n";
                std::cout << "Actual output:\n" << output << "\n";
                ASSERT_TRUE(false);  // Fail the test
            }
        }
    }
}

// Initialize game state for testing
void initializeTestGame() {
    initializeWorld();
    NPCSystem::initializeThief();
    NPCSystem::initializeTroll();
    NPCSystem::initializeCyclops();
    LampSystem::initialize();
    CandleSystem::initialize();
    SwordSystem::initialize();
}

TEST(TranscriptOpeningSequence) {
    initializeTestGame();
    
    std::cout << "Testing game start and mailbox interaction...\n";
    runTranscript(TranscriptData::OPENING_SEQUENCE, "Opening Sequence");
    
    std::cout << "Opening sequence test passed!\n";
}

TEST(TranscriptInitialExploration) {
    initializeTestGame();
    
    std::cout << "Testing initial exploration...\n";
    runTranscript(TranscriptData::INITIAL_EXPLORATION, "Initial Exploration");
    
    std::cout << "Initial exploration test passed!\n";
}

TEST(TranscriptHouseExterior) {
    initializeTestGame();
    runTranscript(TranscriptData::HOUSE_EXTERIOR, "House Exterior Navigation");
}

TEST(TranscriptEnterHouse) {
    initializeTestGame();
    runTranscript(TranscriptData::ENTER_HOUSE, "Enter House");
}

TEST(TranscriptObjectManipulation) {
    initializeTestGame();
    // Navigate to living room first
    executeCommand("north");
    executeCommand("east");
    executeCommand("open window");
    executeCommand("west");  // Enter kitchen through window
    executeCommand("west");  // Go to living room
    
    runTranscript(TranscriptData::OBJECT_MANIPULATION, "Object Manipulation");
}

TEST(TranscriptTrophyCase) {
    initializeTestGame();
    // Navigate to living room
    executeCommand("north");
    executeCommand("east");
    executeCommand("open window");
    executeCommand("west");  // Enter kitchen
    executeCommand("west");  // Go to living room
    
    runTranscript(TranscriptData::TROPHY_CASE, "Trophy Case");
}

TEST(TranscriptErrorHandling) {
    initializeTestGame();
    runTranscript(TranscriptData::ERROR_HANDLING, "Error Handling");
}

TEST(TranscriptParserFeatures) {
    initializeTestGame();
    // Navigate to a room with objects
    executeCommand("north");
    executeCommand("east");
    executeCommand("open window");
    executeCommand("enter");
    
    runTranscript(TranscriptData::PARSER_FEATURES, "Parser Special Features");
}

// Puzzle solution tests
TEST(TranscriptTrollPuzzle) {
    initializeTestGame();
    
    std::cout << "Testing troll puzzle solution...\n";
    
    // Navigate to underground
    executeCommand("north");
    executeCommand("east");
    executeCommand("open window");
    executeCommand("enter");
    executeCommand("west");
    executeCommand("take sword");
    executeCommand("turn on lamp");
    executeCommand("east");
    executeCommand("open trap door");
    executeCommand("down");
    
    runTranscript(TranscriptData::TROLL_PUZZLE_SOLUTION, "Troll Puzzle");
    
    std::cout << "Troll puzzle test passed!\n";
}

// Full game walkthrough test - comprehensive end-to-end test
TEST(TranscriptFullWalkthrough) {
    initializeTestGame();
    
    std::cout << "\n===========================================\n";
    std::cout << "Running full game walkthrough...\n";
    std::cout << "This test verifies the complete game is winnable.\n";
    std::cout << "===========================================\n\n";
    
    runTranscript(TranscriptData::FULL_WALKTHROUGH, "Full Walkthrough");
    
    std::cout << "\n===========================================\n";
    std::cout << "Full walkthrough completed successfully!\n";
    std::cout << "Game is verified to be winnable.\n";
    std::cout << "===========================================\n";
}

int main() {
    std::cout << "Running Transcript Tests\n";
    std::cout << "========================\n\n";
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "\n========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
