// https://github.com/trevor-makes/uANSI.git
// Copyright (c) 2021 Trevor Makes

#include "uANSI.hpp"

#include <Arduino.h>

namespace uANSI {

int read_key(Stream& stream) {
  static uint8_t state = 0;
  for (;;) {
    int input = stream.peek();
    if (input == -1) {
      return -1;
    } else if (state == 1) { // if previous key was '\e'
      if (input == '[') {
        stream.read();
        state = 2;
      } else {
        state = 0;
        return '\e';
      }
    } else if (state == 2) { // if previous keys were "\e["
      stream.read();
      state = 0;
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
      state = 1;
    } else {
      stream.read();
      return input;
    }
  }
}

} // namespace uANSI
