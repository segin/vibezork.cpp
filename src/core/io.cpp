#include "io.h"
#include "object.h"
#include <iostream>
#include <string>

void printDesc(const ZObject* obj) {
    if (obj) {
        std::cout << obj->getDesc();
    }
}

std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}
