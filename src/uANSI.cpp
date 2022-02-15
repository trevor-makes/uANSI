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
  // Remember state across calls with a static variable
  // TODO As-is this won't work if multiple streams are used! Making Stream a
  // template parameter could partially fix this, but Serial1, Serial2, etc as
  // on Arduino Mega are all the same non-template HardwareSerial type.
  // Adding newtype wrappers around each could be a bandaid for that.
  static ParseState state = ParseState::RESET;

  for (;;) {
    // Peek input and return without blocking when none available
    // NOTE need to call stream.read() if we decide to consume the input
    int input = stream.peek();
    if (input == -1) {
      return -1;
    }

    // State machine for handling sequences of control characters
    switch (state) {
    case ParseState::ESCAPE:
      if (input == '[') {
        // Eat CSI; loop back for next character without emitting
        stream.read();
        state = ParseState::CSI;
        continue;
      } else {
        // Otherwise, spit the escape back out as-is
        state = ParseState::RESET;
        return '\e';
      }
    case ParseState::CSI:
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
        // Discard other input sequences... for now
        // TODO handle other sequences and modifier keys; some are more than one char!
        continue;
      }
    case ParseState::CR:
      if (input == '\n') {
        // Discard LF following CR
        stream.read();
      }
      state = ParseState::RESET;
      continue;
    case ParseState::RESET:
      stream.read();
      switch (input) {
      case '\e':
        // Eat escape; loop back for next character without emitting yet
        state = ParseState::ESCAPE;
        continue;
      case '\r':
        // Transform both CR and CRLF to a single LF
        state = ParseState::CR;
        return '\n';
      default:
        // Emit other characters as-is
        return input;
      }
    }
  }
}

} // namespace uANSI
