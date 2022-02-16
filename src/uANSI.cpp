// https://github.com/trevor-makes/uANSI.git
// Copyright (c) 2021 Trevor Makes

#include "uANSI.hpp"

#include <Arduino.h>

namespace uANSI {

int StreamEx::peek() {
  // Fill peek buffer if empty
  if (peek_ == -1) {
    peek_ = read();
  }
  return peek_;
}

int StreamEx::read() {
  // If we peeked, consume it
  if (peek_ != -1) {
    int c = peek_;
    peek_ = -1;
    return c;
  }

  for (;;) {
    // Peek input and return without blocking when none available
    // NOTE need to call stream.read() if we decide to consume the input
    int input = stream_.peek();
    if (input == -1) {
      return -1;
    }

    // State machine for handling sequences of control characters
    switch (state_) {
    case State::ESCAPE:
      if (input == '[') {
        // Eat CSI; loop back for next character without emitting
        stream_.read();
        state_ = State::CSI;
        continue;
      } else {
        // Otherwise, spit the escape back out as-is
        state_ = State::RESET;
        return '\e';
      }
    case State::CSI:
      stream_.read();
      state_ = State::RESET;
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
    case State::CR:
      if (input == '\n') {
        // Discard LF following CR
        stream_.read();
      }
      state_ = State::RESET;
      continue;
    case State::RESET:
      stream_.read();
      switch (input) {
      case '\e':
        // Eat escape; loop back for next character without emitting yet
        state_ = State::ESCAPE;
        continue;
      case '\r':
        // Transform both CR and CRLF to a single LF
        state_ = State::CR;
        return '\n';
      default:
        // Emit other characters as-is
        return input;
      }
    }
  }
}

} // namespace uANSI
