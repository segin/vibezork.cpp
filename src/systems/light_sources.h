#pragma once

#include "world/generated/zil_tables.h"
#include <span>

/**
 * @file light_sources.h
 * @brief The light-source interrupts of 1actions.zil.
 *
 * Source: zil/1actions.zil:2216-2325 (LAMP-TABLE, LANTERN, I-LANTERN,
 *         LIGHT-INT), 2406-2413 (CANDLE-TABLE), 3851-3889 (I-SWORD, INFESTED?)
 *
 * LAMP-TABLE and CANDLE-TABLE are pairs of (tick, message).  The interrupt
 * re-queues itself with the tick at the head of the table, calls LIGHT-INT to
 * print that entry's message, then advances the table pointer by one pair
 * (ZIL: <REST .TBL 4>, two words).  A zero tick is the final entry: the object
 * loses ONBIT, gains RMUNGBIT and the "better have more light" line prints.
 *
 * Because GO queues I-LANTERN with 200 and I-CANDLES with 40, the lamp burns
 * for 200 + 100 + 70 + 15 turns and the candles for 40 + 20 + 10 + 5.
 */

class ZObject;

namespace LightSources {

using zork::zil::TimerStep;

/// ZIL: <ROUTINE LIGHT-INT (OBJ TBL TICK)> (1actions.zil:2319-2330)
void lightInt(ZObject *obj, std::span<const TimerStep> tbl, int tick);

/// ZIL: <ROUTINE I-LANTERN ...> (1actions.zil:2303-2308)
bool iLantern();

/// ZIL: <ROUTINE I-CANDLES ...> (1actions.zil:2310-2317)
bool iCandles();

/// ZIL: <ROUTINE I-SWORD ...> (1actions.zil:3851-3878)
bool iSword();

/// ZIL: <ROUTINE INFESTED? (R)> (1actions.zil:3880-3886): does the room hold
/// a visible ACTORBIT object?
bool infested(ZObject *room);

/// ZIL: <GLOBAL LAMP-TABLE ...> as the interrupt currently sees it; the
/// global is a pointer into the table that I-LANTERN advances.
std::span<const TimerStep> lampTable();

/// ZIL: <GLOBAL CANDLE-TABLE ...> as I-CANDLES currently sees it.
std::span<const TimerStep> candleTable();

/// Reset the table pointers to the head (start of a new game).
void reset();

} // namespace LightSources
