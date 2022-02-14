// https://github.com/trevor-makes/uANSI.git
// Copyright (c) 2021 Trevor Makes

#include "uANSI.hpp"

#include <Arduino.h>

namespace uANSI {

enum class ParseState : uint8_t {
  RESET,
  ESCAPE, // preceding input was "\e"
  CSI, // preceding input was "\e["
  CR, // preceding input was "\r"
};

int read_key(Stream& stream) {
  static ParseState state = ParseState::RESET;
  for (;;) {
    int input = stream.peek();
    if (input == -1) {
      return -1;
    } else if (state == ParseState::ESCAPE) {
      if (input == '[') {
        stream.read();
        state = ParseState::CSI;
      } else {
        state = ParseState::RESET;
        return '\e';
      }
    } else if (state == ParseState::CSI) {
      stream.read();
      state = ParseState::RESET;
      switch (input) {
        case 'A':
          return KEY_UP;
        case 'B':
          return KEY_DOWN;
        case 'C':
          return KEY_RIGHT;
        case 'D':
          return KEY_LEFT;
        default:
          // ignore other sequences
          break;
      }
    } else if (input == '\e') {
      stream.read();
      state = ParseState::ESCAPE;
    } else if (input == '\r') {
      // Transform both CR and CRLF to a single LF
      stream.read();
      state = ParseState::CR;
      return '\n';
    } else if (state == ParseState::CR && input == '\n') {
      // Discard LF following CR
      stream.read();
      state = ParseState::RESET;
    } else {
      stream.read();
      state = ParseState::RESET;
      return input;
    }
  }
}

} // namespace uANSI
