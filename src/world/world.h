#pragma once
#include "rooms.h"
#include "objects.h"

// Initialize all rooms and objects
void initializeWorld();

// Initialize missing objects (called after initializeWorld)
void initializeMissingObjects();

// Object action handlers (declared here for use in missing_objects.cpp)
bool kitchenWindowAction();
