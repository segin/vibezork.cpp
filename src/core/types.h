#pragma once
#include <cstdint>

// Forward declarations
class ZObject;
class ZRoom;

// Type aliases
using ObjectId = int32_t;
using VerbId = int32_t;
using PropertyId = int32_t;

// Special return values
constexpr bool RTRUE = true;
constexpr bool RFALSE = false;

// ZIL: gmain.zil constants
constexpr int SERIAL = 0;
constexpr int M_FATAL = 2;
constexpr int M_HANDLED = 1;
constexpr int M_NOT_HANDLED = 0;
constexpr int M_OBJECT = 0;
constexpr int M_BEG = 1;
constexpr int M_ENTER = 2;
constexpr int M_LOOK = 3;
constexpr int M_FLASH = 4;
constexpr int M_OBJDESC = 5;
constexpr int M_END = 6;
constexpr int RFATAL = M_FATAL;

// Additional room action codes
constexpr int M_PRAY = 101;
constexpr int M_LISTEN = 102;
constexpr int M_YELL = 103;
