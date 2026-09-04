#pragma once
#include "rooms.h"
#include "objects.h"

// Initialize all rooms and objects
void initializeWorld();

// Object action handlers
bool kitchenWindowAction();

// Room action handlers
void behindHouseAction(int rarg);
void eastHouse(int rarg);
