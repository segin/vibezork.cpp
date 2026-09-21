#pragma once

/**
 * @file go.h
 * @brief Game start-up (mirrors ZIL GO, 1dungeon.zil:2637-2661)
 */

/// Build the world, verbs, NPC state and register the interrupt routines
/// (the C-RTN atoms) so GO can QUEUE them by name.
void initializeGame();

/// Everything GO does before it describes the room and enters MAIN-LOOP:
/// queue the start-up interrupts, patch DEF*-RES, and set HERE, IT, LIT,
/// WINNER and PLAYER.  Split out so a test can reach the started state
/// without running the game.  Source: zil/1dungeon.zil:2638-2659
void goSetup();

// ZIL: <ROUTINE GO () ...> (1dungeon.zil:2637-2661)
// Queues the start-up interrupts, patches DEF*-RES, sets HERE/LIT/WINNER,
// prints the banner and first room description, then runs MAIN-LOOP.
void go();
