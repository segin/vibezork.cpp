#pragma once

/**
 * @file go.h
 * @brief Game start-up (mirrors ZIL GO, 1dungeon.zil:2637-2661)
 */

/// Build the world, verbs, NPC state and register the interrupt routines
/// (the C-RTN atoms) so GO can QUEUE them by name.
void initializeGame();

// ZIL: <ROUTINE GO () ...> (1dungeon.zil:2637-2661)
// Queues the start-up interrupts, patches DEF*-RES, sets HERE/LIT/WINNER,
// prints the banner and first room description, then runs MAIN-LOOP.
void go();
