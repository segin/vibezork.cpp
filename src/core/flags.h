#pragma once
#include <cstdint>

// Object flags (from GLOBAL-OBJECTS in gglobals.zil)
enum class ObjectFlag : uint32_t {
    RMUNGBIT     = 1 << 0,
    INVISIBLE    = 1 << 1,
    TOUCHBIT     = 1 << 2,
    SURFACEBIT   = 1 << 3,
    TRYTAKEBIT   = 1 << 4,
    OPENBIT      = 1 << 5,
    SEARCHBIT    = 1 << 6,
    TRANSBIT     = 1 << 7,
    ONBIT        = 1 << 8,
    RLANDBIT     = 1 << 9,
    FIGHTBIT     = 1 << 10,
    STAGGERED    = 1 << 11,
    WEARBIT      = 1 << 12,
    NDESCBIT     = 1 << 13,
    TAKEBIT      = 1 << 14,
    DOORBIT      = 1 << 15,
    CONTBIT      = 1 << 16,
    LIGHTBIT     = 1 << 17,
    ACTORBIT     = 1 << 18,
    WEAPONBIT    = 1 << 19,
    TOOLBIT      = 1 << 20,
    BURNBIT      = 1 << 21,
    FLAMEBIT     = 1 << 22,
    VEHBIT       = 1 << 23,
    CLIMBBIT     = 1 << 24,
    DRINKBIT     = 1 << 25,
    FOODBIT      = 1 << 26,
    READBIT      = 1 << 27,
    TURNBIT      = 1 << 28,
    SACREDBIT    = 1 << 29
};

inline uint32_t operator|(ObjectFlag a, ObjectFlag b) {
    return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

inline uint32_t operator|(uint32_t a, ObjectFlag b) {
    return a | static_cast<uint32_t>(b);
}
