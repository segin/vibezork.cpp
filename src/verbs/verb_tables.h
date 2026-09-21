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

} // namespace VerbTables
