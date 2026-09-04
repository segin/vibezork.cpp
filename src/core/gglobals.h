#pragma once
#include "core/types.h"
#include <string_view>

class Globals;

/**
 * @file gglobals.h
 * @brief Canonical global routines, variables, and objects ported from GGLOBALS.ZIL
 *
 * Source: zil/gglobals.zil:1-309
 *
 * Implements:
 * - NOT-HERE-OBJECT-F (gglobals.zil:52-74)
 * - NOT-HERE-PRINT (gglobals.zil:76-84)
 * - NULL-F (gglobals.zil:85-87)
 * - STAIRS-F (gglobals.zil:110-113)
 * - SAILOR-FCN (gglobals.zil:122-162)
 * - GROUND-FUNCTION (gglobals.zil:170-183)
 * - GRUE-FUNCTION (gglobals.zil:191-206)
 * - CRETIN-FCN (gglobals.zil:221-265)
 * - PATH-OBJECT (gglobals.zil:282-288)
 * - ZORKMID-FUNCTION (gglobals.zil:296-302)
 * - initGlobalObjects(Globals &g): Creates and registers all 18 objects defined in gglobals.zil
 */

namespace GGlobals {

// Routines from gglobals.zil
bool notHereObjectF();
void notHerePrint(bool prso = true);
bool nullF(void *a1 = nullptr, void *a2 = nullptr);
bool stairsF();
bool sailorFcn();
bool groundFunction();
bool grueFunction();
bool cretinFcn();
bool pathObject();
bool zorkmidFunction();

// Object initialization for all 18 objects in gglobals.zil
void initGlobalObjects(Globals &g);

} // namespace GGlobals
