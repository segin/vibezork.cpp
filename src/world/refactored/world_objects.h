// World objects initialization - clean, consolidated object creation
// Replaces the messy world_init.cpp + missing_objects.cpp pattern
#pragma once

// Forward declarations for object initialization functions
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

// Forward declarations for room initialization functions
void initializeExteriorRooms();
void initializeHouseRooms();
void initializeUndergroundRooms();
void initializeMazeRooms();
void initializeSpecialRooms();
void initializeRiverRooms();
void initializeMineRooms();

// Master initialization function - call this to create all objects and rooms
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

// Master room initialization function
inline void initializeAllRooms() {
    initializeExteriorRooms();
    initializeHouseRooms();
    initializeUndergroundRooms();
    initializeMazeRooms();
    initializeSpecialRooms();
    initializeRiverRooms();
    initializeMineRooms();
}
