// World objects initialization - clean, consolidated object creation
// Replaces the messy world_init.cpp + missing_objects.cpp pattern
#pragma once

// Forward declarations for all initialization functions
void initializeTreasures();
void initializeTools();
void initializeContainers();
void initializeNPCs();
void initializeConsumables();
void initializeReadables();
void initializeVehicles();
void initializeSpecialObjects();
void initializeScenery();
void initializePlayer();

// Master initialization function - call this to create all objects
inline void initializeAllObjects() {
    // Order matters: containers before items that go in them
    initializeContainers();
    initializeTreasures();
    initializeTools();
    initializeConsumables();
    initializeReadables();
    initializeVehicles();
    initializeSpecialObjects();
    initializeScenery();
    initializeNPCs();
    initializePlayer();
}
