#pragma once
#include "core/types.h"
#include "world/rooms.h"

// Verb IDs (from gsyntax.zil)
// Game commands
constexpr VerbId V_VERBOSE = 1;
constexpr VerbId V_BRIEF = 2;
constexpr VerbId V_SUPERBRIEF = 3;
constexpr VerbId V_DIAGNOSE = 4;
constexpr VerbId V_INVENTORY = 5;
constexpr VerbId V_QUIT = 6;
constexpr VerbId V_RESTART = 7;
constexpr VerbId V_RESTORE = 8;
constexpr VerbId V_SAVE = 9;
constexpr VerbId V_SCORE = 10;
constexpr VerbId V_VERSION = 11;

// Real verbs - manipulation
constexpr VerbId V_TAKE = 20;
constexpr VerbId V_DROP = 21;
constexpr VerbId V_PUT = 22;
constexpr VerbId V_PUT_ON = 23;
constexpr VerbId V_GIVE = 24;

// Examination
constexpr VerbId V_LOOK = 30;
constexpr VerbId V_EXAMINE = 31;
constexpr VerbId V_READ = 32;
constexpr VerbId V_LOOK_INSIDE = 33;
constexpr VerbId V_SEARCH = 34;

// Container operations
constexpr VerbId V_OPEN = 40;
constexpr VerbId V_CLOSE = 41;
constexpr VerbId V_LOCK = 42;
constexpr VerbId V_UNLOCK = 43;

// Movement
constexpr VerbId V_WALK = 50;
constexpr VerbId V_ENTER = 51;
constexpr VerbId V_EXIT = 52;
constexpr VerbId V_CLIMB_UP = 53;
constexpr VerbId V_CLIMB_DOWN = 54;
constexpr VerbId V_CLIMB_ON = 55;
constexpr VerbId V_BOARD = 56;
constexpr VerbId V_DISEMBARK = 57;

// Combat
constexpr VerbId V_ATTACK = 60;
constexpr VerbId V_KILL = 61;
constexpr VerbId V_THROW = 62;
constexpr VerbId V_SWING = 63;

// Light
constexpr VerbId V_LAMP_ON = 70;
constexpr VerbId V_LAMP_OFF = 71;

// Manipulation
constexpr VerbId V_TURN = 80;
constexpr VerbId V_PUSH = 81;
constexpr VerbId V_PULL = 82;
constexpr VerbId V_MOVE = 83;

// Interaction
constexpr VerbId V_TIE = 90;
constexpr VerbId V_UNTIE = 91;
constexpr VerbId V_LISTEN = 92;
constexpr VerbId V_SMELL = 93;
constexpr VerbId V_TOUCH = 94;

// Consumption
constexpr VerbId V_EAT = 100;
constexpr VerbId V_DRINK = 101;

// Special actions
constexpr VerbId V_INFLATE = 110;
constexpr VerbId V_DEFLATE = 111;
constexpr VerbId V_PRAY = 112;
constexpr VerbId V_EXORCISE = 113;
constexpr VerbId V_WAVE = 114;
constexpr VerbId V_RUB = 115;
constexpr VerbId V_RING = 116;
constexpr VerbId V_BURN = 117;
constexpr VerbId V_DIG = 118;
constexpr VerbId V_FILL = 119;

// Communication
constexpr VerbId V_TALK = 120;
constexpr VerbId V_ASK = 121;
constexpr VerbId V_TELL = 122;
constexpr VerbId V_ODYSSEUS = 123;  // Special verb for cyclops puzzle
constexpr VerbId V_YELL = 124;

// Easter eggs / special words
constexpr VerbId V_HELLO = 130;
constexpr VerbId V_ZORK = 131;
constexpr VerbId V_PLUGH = 132;  // Also XYZZY - Adventure reference
constexpr VerbId V_FROBOZZ = 133;

// Verb handlers
namespace Verbs {
    bool vLook();
    bool vInventory();
    bool vQuit();
    bool vTake();
    bool vDrop();
    bool vPut();
    bool vExamine();
    bool vRead();
    bool vLookInside();
    bool vSearch();
    bool vOpen();
    bool vClose();
    bool vLock();
    bool vUnlock();
    bool vWalk();
    bool vWalkDir(Direction dir);
    
    // Special movement verbs
    bool vEnter();
    bool vExit();
    bool vClimbUp();
    bool vClimbDown();
    bool vBoard();
    bool vDisembark();
    
    // Manipulation verbs
    bool vTurn();
    bool vPush();
    bool vPull();
    bool vMove();
    
    // Interaction verbs
    bool vTie();
    bool vUntie();
    bool vListen();
    bool vSmell();
    bool vTouch();
    bool vYell();
    
    // Consumption verbs
    bool vEat();
    bool vDrink();
    
    // Light source verbs
    bool vLampOn();
    bool vLampOff();
    
    // Special action verbs
    bool vInflate();
    bool vDeflate();
    bool vPray();
    bool vExorcise();
    bool vWave();
    bool vRub();
    bool vRing();
    
    // Combat verbs
    bool vAttack();
    bool vKill();
    bool vThrow();
    bool vSwing();
    
    // Meta-game verbs
    bool vScore();
    bool vDiagnose();
    bool vVerbose();
    bool vBrief();
    bool vSuperbrief();
    
    // Game control verbs
    bool vSave();
    bool vRestore();
    bool vRestart();
    bool vVersion();
    
    // Communication verbs
    bool vTalk();
    bool vAsk();
    bool vTell();
    bool vOdysseus();
    
    // Easter eggs / special words
    bool vHello();
    bool vZork();
    bool vPlugh();  // Also handles XYZZY
    bool vFrobozz();
    
    // Helper for special movement
    bool trySpecialMovement(int verbId, Direction dir);
}
