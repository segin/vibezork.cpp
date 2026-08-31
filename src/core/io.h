#pragma once
#include <string>
#include <string_view>
#include <iostream>
#include <type_traits>

// Forward declaration
class ZObject;

// Word wrapping configuration
constexpr int WRAP_WIDTH = 80;

// Output functions (ZIL TELL macro equivalents)
void print(std::string_view str);
void printLine(std::string_view str);
void printDesc(const ZObject* obj);

inline void crlf() {
    std::cout << std::endl;
}

namespace detail {
    template <typename T>
    inline void tellSingle(const T& item) {
        using Decayed = std::decay_t<T>;
        if constexpr (std::is_same_v<Decayed, ZObject*> || std::is_same_v<Decayed, const ZObject*>) {
            printDesc(item);
        } else if constexpr (std::is_convertible_v<T, std::string_view>) {
            print(std::string_view(item));
        } else if constexpr (std::is_arithmetic_v<Decayed>) {
            print(std::to_string(item));
        } else {
            std::cout << item;
        }
    }
} // namespace detail

/**
 * @brief Modern C++23 fold expression implementation of ZIL TELL macro
 */
template <typename... Args>
inline void tell(Args&&... args) {
    (detail::tellSingle(std::forward<Args>(args)), ...);
    crlf();
}

// Input functions
std::string readLine();
