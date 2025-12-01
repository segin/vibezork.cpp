#pragma once
#include <string>
#include <string_view>
#include <iostream>

// Word wrapping configuration
constexpr int WRAP_WIDTH = 80;

// Output functions (ZIL TELL macro equivalents)
void print(std::string_view str);
void printLine(std::string_view str);

inline void crlf() {
    std::cout << std::endl;
}

void printDesc(const class ZObject* obj);

// Input functions
std::string readLine();
