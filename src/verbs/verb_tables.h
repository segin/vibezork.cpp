#pragma once
#include "core/gmacros.h"
#include <string>

/**
 * @file verb_tables.h
 * @brief The PICK-ONE message tables defined in gverbs.zil
 *
 * ZIL stores these as LTABLEs whose element 0 is the count of entries already
 * used this cycle; PICK-ONE swaps the chosen entry into that slot so every
 * entry is printed once before any repeats. GMacros::ZilRandomTable reproduces
 * that algorithm, so the tables are mutable singletons, not constants.
 *
 * Source: zil/gverbs.zil
 */
namespace VerbTables {

/// ZIL: <GLOBAL YUKS <LTABLE 0 ...>> (gverbs.zil:2210-2216)
GMacros::ZilRandomTable<std::string> &yuks();

/// ZIL: <GLOBAL HELLOS <LTABLE 0 ...>> (gverbs.zil:2200-2204)
GMacros::ZilRandomTable<std::string> &hellos();

/// ZIL: <GLOBAL HO-HUM <LTABLE 0 ...>> (gverbs.zil:2030-2036)
GMacros::ZilRandomTable<std::string> &hoHum();

/// ZIL: <GLOBAL JUMPLOSS <LTABLE 0 ...>> (gverbs.zil:844-848)
GMacros::ZilRandomTable<std::string> &jumploss();

/// ZIL: <GLOBAL WHEEEEE <LTABLE 0 ...>> (gverbs.zil:1272-1276)
GMacros::ZilRandomTable<std::string> &wheeeee();

/// ZIL: <GLOBAL DUMMY <LTABLE 0 ...>> (gverbs.zil:2213-2217)
GMacros::ZilRandomTable<std::string> &dummy();

/// ZIL: <GLOBAL SWIMYUKS <LTABLE 0 ...>> (gverbs.zil:2194-2196)
GMacros::ZilRandomTable<std::string> &swimyuks();

} // namespace VerbTables
