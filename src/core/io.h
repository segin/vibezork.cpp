#pragma once
#include <string>
#include <iostream>

// Output functions (ZIL TELL macro equivalents)
inline void print(const std::string& str) {
    std::cout << str;
}

inline void printLine(const std::string& str) {
    std::cout << str << std::endl;
}

inline void crlf() {
    std::cout << std::endl;
}

inline void printDesc(const class ZObject* obj);

// Input functions
std::string readLine();
