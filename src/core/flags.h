#pragma once
#include <cstdint>

// Object flags (from GLOBAL-OBJECTS in gglobals.zil)
enum class ObjectFlag : uint32_t {
    RMUNGBIT     = 1U << 0,
    INVISIBLE    = 1U << 1,
    TOUCHBIT     = 1U << 2,
    SURFACEBIT   = 1U << 3,
    TRYTAKEBIT   = 1U << 4,
    OPENBIT      = 1U << 5,
    SEARCHBIT    = 1U << 6,
    TRANSBIT     = 1U << 7,
    ONBIT        = 1U << 8,
    RLANDBIT     = 1U << 9,
    FIGHTBIT     = 1U << 10,
    STAGGERED    = 1U << 11,
    WEARBIT      = 1U << 12,
    NDESCBIT     = 1U << 13,
    TAKEBIT      = 1U << 14,
    DOORBIT      = 1U << 15,
    CONTBIT      = 1U << 16,
    LIGHTBIT     = 1U << 17,
    ACTORBIT     = 1U << 18,
    WEAPONBIT    = 1U << 19,
    TOOLBIT      = 1U << 20,
    BURNBIT      = 1U << 21,
    FLAMEBIT     = 1U << 22,
    VEHBIT       = 1U << 23,
    CLIMBBIT     = 1U << 24,
    DRINKBIT     = 1U << 25,
    FOODBIT      = 1U << 26,
    READBIT      = 1U << 27,
    TURNBIT      = 1U << 28,
    SACREDBIT    = 1U << 29,
    LOCKEDBIT    = 1U << 30,
    DEADBIT      = 1U << 31
};

inline uint32_t operator|(ObjectFlag a, ObjectFlag b) {
    return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
}

inline uint32_t operator|(uint32_t a, ObjectFlag b) {
    return a | static_cast<uint32_t>(b);
}
