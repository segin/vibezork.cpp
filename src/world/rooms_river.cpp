// River and dam area rooms
// Values sourced directly from ZIL 1dungeon.zil

#include "room_builder.h"

void initializeRiverRooms() {
  using namespace RoomIds;
  using enum Direction;
  using enum ObjectFlag;

  // RESERVOIR_SOUTH - South shore of reservoir
  createRoom({
      .id = RESERVOIR_SOUTH,
      .name = "Reservoir South",
      .longDesc = "You are in a long room on the south shore of a large lake, "
                  "far too deep "
                  "and wide for crossing. There is a path along the stream to "
                  "the east or "
                  "west, and a steep pathway climbing southwest along the edge "
                  "of a chasm.",
      .exits = {{SE, DEEP_CANYON},
                {SW, CHASM_ROOM},
                {EAST, DAM_ROOM},
                {WEST, STREAM_VIEW}},
      .flags = {RLANDBIT} // Dark
  });

  // RESERVOIR - On the lake (water room)
  createRoom({
      .id = RESERVOIR,
      .name = "Reservoir",
      .longDesc = "You are on the lake. The water is cold and the current is "
                  "strong. It is "
                  "difficult to stay afloat.",
      .exits = {{NORTH, RESERVOIR_NORTH},
                {SOUTH, RESERVOIR_SOUTH},
                {UP, IN_STREAM},
                {WEST, IN_STREAM}},
      .flags = {NONLANDBIT} // Water room - requires boat
  });

  // RESERVOIR_NORTH - North shore of reservoir
  createRoom({
      .id = RESERVOIR_NORTH,
      .name = "Reservoir North",
      .longDesc = "You are in a long room on the north shore of a large lake, "
                  "far too deep "
                  "and wide for crossing.",
      .exits = {{NORTH, ATLANTIS_ROOM}},
      .flags = {RLANDBIT} // Dark
  });

  // STREAM_VIEW - Path beside stream
  createRoom({
      .id = STREAM_VIEW,
      .name = "Stream View",
      .longDesc =
          "You are standing on a path beside a gently flowing stream. The path "
          "follows the stream, which flows from west to east.",
      .exits = {{EAST, RESERVOIR_SOUTH}},
      .flags = {RLANDBIT} // Dark
  });

  // IN_STREAM - On the stream (water room)
  createRoom({
      .id = IN_STREAM,
      .name = "Stream",
      .longDesc = "You are on the gently flowing stream. The upstream route is "
                  "too narrow to "
                  "navigate, and the downstream route is invisible due to "
                  "twisting walls. "
                  "There is a narrow beach to land on.",
      .exits = {{DOWN, RESERVOIR}, {EAST, RESERVOIR}},
      .flags = {NONLANDBIT} // Water room - requires boat
  });

  // ATLANTIS_ROOM - Ancient underwater room
  createRoom({
      .id = ATLANTIS_ROOM,
      .name = "Atlantis Room",
      .longDesc = "This is an ancient room, long under water. There is an exit "
                  "to the south "
                  "and a staircase leading up.",
      .exits = {{UP, SMALL_CAVE}, {SOUTH, RESERVOIR_NORTH}},
      .flags = {RLANDBIT} // Dark
  });

  // DAM_ROOM - Top of the dam
  createRoom({
      .id = DAM_ROOM,
      .name = "Dam",
      .longDesc = "You are standing on the top of the Flood Control Dam #3, "
                  "which was quite "
                  "a tourist attraction in times far distant. There are paths "
                  "to the north, "
                  "south, and west, and a scramble down.",
      .exits = {{SOUTH, DEEP_CANYON},
                {DOWN, DAM_BASE},
                {EAST, DAM_BASE},
                {NORTH, DAM_LOBBY},
                {WEST, RESERVOIR_SOUTH}},
      .flags = {RLANDBIT, ONBIT} // Dark, lit
  });

  // DAM_LOBBY - Dam waiting room
  createRoom({
      .id = DAM_LOBBY,
      .name = "Dam Lobby",
      .longDesc = "This room appears to have been the waiting room for groups "
                  "touring the dam. "
                  "There are open doorways here to the north and east marked "
                  "\"Private\", and "
                  "there is a path leading south over the top of the dam.",
      .exits = {{SOUTH, DAM_ROOM},
                {NORTH, MAINTENANCE_ROOM},
                {EAST, MAINTENANCE_ROOM}},
      .flags = {RLANDBIT, ONBIT} // Dark, lit
  });

  // MAINTENANCE_ROOM - Dam control room
  createRoom({
      .id = MAINTENANCE_ROOM,
      .name = "Maintenance Room",
      .longDesc = "This is what appears to have been the maintenance room for "
                  "Flood Control "
                  "Dam #3. Apparently, this room has been ransacked recently, "
                  "for most of the "
                  "valuable equipment is gone. On the wall in front of you is "
                  "a group of "
                  "buttons colored blue, yellow, brown, and red. There are "
                  "doorways to the "
                  "west and south.",
      .exits = {{SOUTH, DAM_LOBBY}, {WEST, DAM_LOBBY}},
      .flags = {RLANDBIT} // Dark
  });

  // DAM_BASE - Base of the dam
  createRoom({
      .id = DAM_BASE,
      .name = "Dam Base",
      .longDesc =
          "You are at the base of Flood Control Dam #3, which looms above you "
          "and to "
          "the north. The river Frigid is flowing by here. Along the river are "
          "the "
          "White Cliffs which seem to form giant walls stretching from north "
          "to south "
          "along the shores of the river as it winds its way downstream.",
      .exits = {{NORTH, DAM_ROOM}, {UP, DAM_ROOM}},
      .flags = {RLANDBIT, ONBIT, SACREDBIT} // Dark, lit, safe
  });
}
