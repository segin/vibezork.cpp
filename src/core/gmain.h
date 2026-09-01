#pragma once
#include "types.h"
#include "object.h"
#include <functional>
#include <map>
#include <string_view>

/**
 * @brief Generic MAIN routines (mirrors ZIL gmain.zil)
 *
 * Sourced directly from Infocom's ZIL gmain.zil:
 * - Constants: SERIAL, M-FATAL, M-HANDLED, M-NOT-HANDLED, M-OBJECT, M-BEG, M-END, M-ENTER, M-LOOK, M-FLASH, M-OBJDESC
 * - Globals: P-WON, P-MULT, P-NOT-HERE, PLAYER, L-PRSA, L-PRSO, L-PRSI, DEBUG
 * - Routines: PERFORM, D-APPLY, DD-APPLY, MAIN-LOOP, MAIN-LOOP-1
 *
 * Source: zil/gmain.zil:1-314
 */

// Verb and Preaction handlers registry
void registerVerbHandler(VerbId verb, std::function<bool()> handler);
void registerPreaction(VerbId verb, std::function<bool()> handler);
bool hasVerbHandler(VerbId verb);
bool hasPreaction(VerbId verb);
std::function<bool()> getVerbHandler(VerbId verb);
std::function<bool()> getPreaction(VerbId verb);
void initializeAllVerbHandlers();

// ZIL: <DEFINE D-APPLY (STR FCN "OPTIONAL" FOO "AUX" RES) ...> (gmain.zil:290-308)
int dApply(std::string_view str, const std::function<int()>& fcn);

// ZIL: <ROUTINE DD-APPLY (STR OBJ FCN "OPTIONAL" (FOO <>)) ...> (gmain.zil:309-312)
int ddApply(std::string_view str, ZObject* obj, const std::function<int()>& fcn);

// ZIL: <ROUTINE PERFORM (A "OPTIONAL" (O <>) (I <>) "AUX" V OA OO OI) ...> (gmain.zil:182-288)
int perform(VerbId a, ZObject* o = nullptr, ZObject* i = nullptr);

// Check if a verb is a meta-verb that doesn't consume a game clock turn
// ZIL: <VERB? TELL BRIEF SUPER-BRIEF VERBOSE SAVE VERSION QUIT RESTART SCORE SCRIPT UNSCRIPT RESTORE> (gmain.zil:170-171)
bool isMetaVerb(VerbId verb);

// ZIL: <ROUTINE MAIN-LOOP () ...> (gmain.zil:34-36)
void mainLoop();

// ZIL: <ROUTINE MAIN-LOOP-1 () ...> (gmain.zil:38-173)
void mainLoop1();
