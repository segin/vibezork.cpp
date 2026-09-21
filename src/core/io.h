#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

// Forward declaration
class ZObject;

/**
 * @file io.h
 * @brief Output layer modelled on the Z-machine screen model and ZIL TELL
 *
 * Strings ported from ZIL are stored as single lines: a newline inside a
 * ZIL string literal is a line continuation (a space), and "|" is a hard
 * newline (CRLF). Nothing is pre-wrapped; the output layer wraps words at
 * the screen width like the interpreter does, and never inserts characters
 * of its own between consecutive prints. TELL does not append a newline;
 * CRLF is explicit (tell(..., CR) or crlf()).
 */

// Default Z-machine screen width used for wrapping; 0 disables wrapping
constexpr int WRAP_WIDTH = 80;

/// Screen width for word wrapping (0 = no wrap)
void setScreenWidth(int width);
int getScreenWidth();

/// Current output column (for tests and the wrapper)
int getOutputColumn();

// Output functions (ZIL TELL macro equivalents)
void print(std::string_view str);   // ZIL: PRINTI / PRINT
void printLine(std::string_view str); // ZIL: <TELL "..." CR>
void printDesc(const ZObject* obj);  // ZIL: PRINTD / TELL D
void crlf();                         // ZIL: CRLF

/// ZIL: the CR / CRLF indicator inside TELL
struct CrTag {};
inline constexpr CrTag CR{};

namespace detail {
    template <typename T>
    inline void tellSingle(const T& item) {
        using Decayed = std::decay_t<T>;
        if constexpr (std::is_same_v<Decayed, CrTag>) {
            crlf();
        } else if constexpr (std::is_same_v<Decayed, ZObject*> || std::is_same_v<Decayed, const ZObject*>) {
            printDesc(item);
        } else if constexpr (std::is_convertible_v<T, std::string_view>) {
            print(std::string_view(item));
        } else if constexpr (std::is_arithmetic_v<Decayed>) {
            print(std::to_string(item)); // ZIL: PRINTN
        } else {
            std::cout << item;
        }
    }
} // namespace detail

/**
 * @brief ZIL TELL macro (gmacros.zil:9-50): prints each item in turn.
 * No newline is added; pass CR where the ZIL has CR.
 */
template <typename... Args>
inline void tell(Args&&... args) {
    (detail::tellSingle(std::forward<Args>(args)), ...);
}

// Input functions
std::string readLine();
