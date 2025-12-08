// Maze rooms - confusing network of twisty passages
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"

// Forward declaration for maze room action
void mazeRoomAction(int rarg);

void initializeMazeRooms() {
    using namespace RoomIds;
    using enum Direction;
    using enum ObjectFlag;
    
    // Helper lambda to create maze rooms (they're all similar)
    auto createMazeRoom = [](ObjectId id) {
        createRoom({
            .id = id,
            .name = "Maze",
            .longDesc = "This is part of a maze of twisty little passages, all alike.",
            .exits = {},  // Exits set up separately in maze navigation
            .flags = {RLANDBIT}  // Dark
        });
    };
    
    // Create all 15 maze rooms
    createMazeRoom(MAZE_1);
    createMazeRoom(MAZE_2);
    createMazeRoom(MAZE_3);
    createMazeRoom(MAZE_4);
    createMazeRoom(MAZE_5);
    createMazeRoom(MAZE_6);
    createMazeRoom(MAZE_7);
    createMazeRoom(MAZE_8);
    createMazeRoom(MAZE_9);
    createMazeRoom(MAZE_10);
    createMazeRoom(MAZE_11);
    createMazeRoom(MAZE_12);
    createMazeRoom(MAZE_13);
    createMazeRoom(MAZE_14);
    createMazeRoom(MAZE_15);

    
    // GRATING_ROOM - Exit from maze
    createRoom({
        .id = GRATING_ROOM,
        .name = "Grating Room",
        .longDesc = "You are in a small room near the maze. There are twisty passages in the "
                    "immediate vicinity.",
        .exits = {
            {SOUTH, MAZE_14},
            {WEST, MAZE_15},
            {UP, GRATING_CLEARING}  // Through grating when open
        },
        .flags = {RLANDBIT}  // Dark
    });
    
    // Dead end rooms
    createRoom({
        .id = DEAD_END_1,
        .name = "Dead End",
        .longDesc = "You have come to a dead end in the maze.",
        .exits = {},  // Exits set up in maze navigation
        .flags = {RLANDBIT}  // Dark
    });
    
    createRoom({
        .id = DEAD_END_2,
        .name = "Dead End",
        .longDesc = "You have come to a dead end in the maze.",
        .exits = {},  // Exits set up in maze navigation
        .flags = {RLANDBIT}  // Dark
    });
    
    createRoom({
        .id = DEAD_END_3,
        .name = "Dead End",
        .longDesc = "You have come to a dead end in the maze.",
        .exits = {},  // Exits set up in maze navigation
        .flags = {RLANDBIT}  // Dark
    });
    
    createRoom({
        .id = DEAD_END_4,
        .name = "Dead End",
        .longDesc = "You have come to a dead end in the maze.",
        .exits = {},  // Exits set up in maze navigation
        .flags = {RLANDBIT}  // Dark
    });
    
} // end initializeMazeRooms
