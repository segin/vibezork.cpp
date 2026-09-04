#pragma once
#include "rooms.h"
#include "objects.h"

// Initialize all rooms and objects
void initializeWorld();

// Object action handlers
bool kitchenWindowAction();
bool mirrorAction();

// Room action handlers
void behindHouseAction(int rarg);
void eastHouse(int rarg);
void cave2Room(int rarg);
void boomRoom(int rarg);
void batsRoom(int rarg);
void fallsRoom(int rarg);
void rivr4Room(int rarg);
void treeRoom(int rarg);
void forestRoom(int rarg);
void mirrorRoom(int rarg);
bool isForestRoom();
bool iForestRoom();

// Helper routines
void fweep(int n);
void flyMe();
