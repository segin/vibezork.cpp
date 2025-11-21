#pragma once
#include "core/types.h"
#include "world/rooms.h"

// Verb IDs (from gsyntax.zil)
constexpr VerbId V_TAKE = 1;
constexpr VerbId V_DROP = 2;
constexpr VerbId V_LOOK = 3;
constexpr VerbId V_EXAMINE = 4;
constexpr VerbId V_OPEN = 5;
constexpr VerbId V_CLOSE = 6;
constexpr VerbId V_INVENTORY = 7;
constexpr VerbId V_WALK = 8;
constexpr VerbId V_QUIT = 9;
constexpr VerbId V_ATTACK = 10;
constexpr VerbId V_READ = 11;
constexpr VerbId V_TURN = 12;
constexpr VerbId V_LAMP_ON = 13;
constexpr VerbId V_LAMP_OFF = 14;
constexpr VerbId V_PUT = 15;

// Verb handlers
namespace Verbs {
    bool vLook();
    bool vInventory();
    bool vQuit();
    bool vTake();
    bool vDrop();
    bool vExamine();
    bool vOpen();
    bool vClose();
    bool vWalk();
    bool vWalkDir(Direction dir);
}
