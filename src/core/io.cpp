/**
 * @file io.cpp
 * @brief Output layer: Z-machine style word wrapping, no auto spacing/CRLF
 */

#include "io.h"
#include "object.h"
#include <iostream>
#include <string>

namespace {

int screenWidth = WRAP_WIDTH;
int currentColumn = 0;
std::string pendingWord;   // characters not yet committed to the line
bool pendingSpace = false; // a space seen after the last word

void emit(std::string_view s) {
  std::cout << s;
  currentColumn += static_cast<int>(s.size());
}

void newline() {
  std::cout << '\n';
  currentColumn = 0;
}

// Commit the buffered word to the line, wrapping first if it would not fit
// (a wrap drops the space that preceded the word, as the interpreter does).
void flushWord() {
  if (pendingWord.empty()) {
    return;
  }
  int needed = static_cast<int>(pendingWord.size()) + (pendingSpace ? 1 : 0);
  if (screenWidth > 0 && currentColumn > 0 && currentColumn + needed > screenWidth) {
    newline();
    pendingSpace = false;
  }
  if (pendingSpace) {
    emit(" ");
    pendingSpace = false;
  }
  emit(pendingWord);
  pendingWord.clear();
}

} // namespace

void setScreenWidth(int width) { screenWidth = width < 0 ? 0 : width; }

int getScreenWidth() { return screenWidth; }

int getOutputColumn() {
  flushWord();
  return currentColumn;
}

void printDesc(const ZObject *obj) {
  if (obj) {
    print(obj->getDesc());
  }
}

void print(std::string_view str) {
  for (char c : str) {
    if (c == '\n') {
      flushWord();
      pendingSpace = false;
      newline();
    } else if (c == ' ') {
      flushWord();
      if (pendingSpace) {
        // consecutive spaces are kept verbatim
        emit(" ");
      }
      pendingSpace = true;
    } else {
      pendingWord += c;
    }
  }
  flushWord();
  std::cout.flush();
}

void printLine(std::string_view str) {
  print(str);
  crlf();
}

void crlf() {
  flushWord();
  pendingSpace = false;
  newline();
  std::cout.flush();
}

std::string readLine() {
  std::string line;

  // Handle EOF gracefully (Requirement 72.4)
  if (!std::getline(std::cin, line)) {
    if (std::cin.eof()) {
      // EOF encountered (Ctrl+D on Unix, Ctrl+Z on Windows)
      // Return empty string to signal end
      return "";
    }
    // Other input error
    std::cin.clear();
    return "";
  }

  // The player's newline echoed by the terminal ends the prompt line
  currentColumn = 0;
  pendingSpace = false;
  return line;
}
