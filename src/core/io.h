#pragma once
#include <string>
#include <iostream>

// Word wrapping configuration
constexpr int WRAP_WIDTH = 80;

// Output functions (ZIL TELL macro equivalents)
void print(const std::string& str);
void printLine(const std::string& str);

inline void crlf() {
    std::cout << std::endl;
}

inline void printDesc(const class ZObject* obj);

// Input functions
std::string readLine();
