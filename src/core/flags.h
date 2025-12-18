#pragma once
#include <cstdint>

// Object flags (from GLOBAL-OBJECTS in gglobals.zil)
// Expanded to uint64_t to accommodate all ZIL flags
enum class ObjectFlag : uint64_t {
  // Core object flags (bits 0-15)
  RMUNGBIT = 1ULL << 0,   // Room has been munged/destroyed
  INVISIBLE = 1ULL << 1,  // Object is invisible
  TOUCHBIT = 1ULL << 2,   // Object has been touched
  SURFACEBIT = 1ULL << 3, // Object is a surface
  TRYTAKEBIT = 1ULL << 4, // Taking triggers special action
  OPENBIT = 1ULL << 5,    // Container/door is open
  SEARCHBIT = 1ULL << 6,  // Object can be searched
  TRANSBIT = 1ULL << 7,   // Container is transparent
  ONBIT = 1ULL << 8,      // Device is on
  RLANDBIT = 1ULL << 9,   // Room is on land
  FIGHTBIT = 1ULL << 10,  // Combat-related flag
  STAGGERED = 1ULL << 11, // NPC is staggered in combat
  WEARBIT = 1ULL << 12,   // Object can be worn
  NDESCBIT = 1ULL << 13,  // Suppress normal description
  TAKEBIT = 1ULL << 14,   // Object can be taken
  DOORBIT = 1ULL << 15,   // Object is a door

  // Extended flags (bits 16-31)
  CONTBIT = 1ULL << 16,   // Object is a container
  LIGHTBIT = 1ULL << 17,  // Object provides light
  ACTORBIT = 1ULL << 18,  // Object is an NPC/actor
  WEAPONBIT = 1ULL << 19, // Object is a weapon
  TOOLBIT = 1ULL << 20,   // Object is a tool
  BURNBIT = 1ULL << 21,   // Object can be burned
  FLAMEBIT = 1ULL << 22,  // Object is on fire
  VEHBIT = 1ULL << 23,    // Object is a vehicle
  CLIMBBIT = 1ULL << 24,  // Object can be climbed
  DRINKBIT = 1ULL << 25,  // Object can be drunk
  FOODBIT = 1ULL << 26,   // Object can be eaten
  READBIT = 1ULL << 27,   // Object has readable text
  TURNBIT = 1ULL << 28,   // Object can be turned
  SACREDBIT = 1ULL << 29, // Sacred location (no combat)
  LOCKEDBIT = 1ULL << 30, // Container/door is locked
  DEADBIT = 1ULL << 31,   // NPC is dead

  // Additional ZIL flags (bits 32-47)
  MAZEBIT = 1ULL << 32,    // Room is in a maze
  NONLANDBIT = 1ULL << 33, // Room is not on land (water)
  GWIMBIT = 1ULL << 34,    // "Get What I Mean" parser hint
  INHIBIT = 1ULL << 35,    // Inhibit certain behaviors
  MULTIBIT = 1ULL << 36,   // Parser: multiple objects allowed
  SLOCBIT = 1ULL << 37     // Special location bit
};

inline uint64_t operator|(ObjectFlag a, ObjectFlag b) {
  return static_cast<uint64_t>(a) | static_cast<uint64_t>(b);
}

inline uint64_t operator|(uint64_t a, ObjectFlag b) {
  return a | static_cast<uint64_t>(b);
}
