#pragma once
#include "rooms.h"
#include "objects.h"

// Initialize all rooms and objects
void initializeWorld();

// Object action handlers
bool kitchenWindowAction();
bool mirrorAction();

// Room action handlers
int behindHouseAction(int rarg);
int eastHouse(int rarg);
int cave2Room(int rarg);
int boomRoom(int rarg);
int batsRoom(int rarg);
int fallsRoom(int rarg);
int rivr4Room(int rarg);
int treeRoom(int rarg);
int forestRoom(int rarg);
int mirrorRoom(int rarg);
int lldRoom(int rarg);
bool isForestRoom();
bool iForestRoom();
bool iXb();
bool iXc();
bool iXbh();

// Helper routines
void fweep(int n);
void flyMe();
