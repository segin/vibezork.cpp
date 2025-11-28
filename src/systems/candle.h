#pragma once

// Candle System - handles candle burning and warnings
// Based on I-CANDLES interrupt from GCLOCK.ZIL
// 
// The candles burn down over time when lit.
// The candles start with 50 units of wax (stored in P_STRENGTH property).
// They burn 1 unit per turn when lit.
// Warnings are given when candles are nearly gone.
// When wax reaches 0, the candles are removed from the game.

namespace CandleSystem {

// Initialize the candle timer
// This registers the I-CANDLES timer with the timer system
void initialize();

// Candle timer callback
// Called each turn by the timer system
// Decrements wax, warns when low, removes when burned out
void candleTimerCallback();

// Enable the candle timer (when candles are lit)
void enableCandleTimer();

// Disable the candle timer (when candles are extinguished or burned out)
void disableCandleTimer();

// Check if candles have wax remaining
bool hasCandleWax();

// Get current wax level
int getCandleWax();

} // namespace CandleSystem
