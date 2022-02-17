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
      // Handle some common CSI input sequences
      switch (input) {
      case 'A':
        stream_.read();
        state_ = State::RESET;
        return KEY_UP;
      case 'B':
        stream_.read();
        state_ = State::RESET;
        return KEY_DOWN;
      case 'C':
        stream_.read();
        state_ = State::RESET;
        return KEY_RIGHT;
      case 'D':
        stream_.read();
        state_ = State::RESET;
        return KEY_LEFT;
      case 'F':
        stream_.read();
        state_ = State::RESET;
        return KEY_END;
      case 'H':
        stream_.read();
        state_ = State::RESET;
        return KEY_HOME;
      default:
        // If we didn't recognize the sequence, emit the CSI as-is
        state_ = State::EMIT_CSI;
        return '\e';
      }
    case State::EMIT_CSI:
      // Continue emitting the unhandled CSI
      state_ = State::RESET;
      return '[';
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

void StreamEx::cursor_to(uint8_t row, uint8_t col) {
  stream_.write("\e[");
  stream_.print(row);
  stream_.write(';');
  stream_.print(col);
  stream_.write('H');
}

void StreamEx::cursor_up(uint8_t spaces) {
  stream_.write("\e[");
  if (spaces != 1) {
    stream_.print(spaces);
  }
  stream_.write('A');
}

void StreamEx::cursor_down(uint8_t spaces) {
  stream_.write("\e[");
  if (spaces != 1) {
    stream_.print(spaces);
  }
  stream_.write('B');
}

void StreamEx::cursor_right(uint8_t spaces) {
  stream_.write("\e[");
  if (spaces != 1) {
    stream_.print(spaces);
  }
  stream_.write('C');
}

void StreamEx::cursor_left(uint8_t spaces) {
  stream_.write("\e[");
  if (spaces != 1) {
    stream_.print(spaces);
  }
  stream_.write('D');
}

void StreamEx::hide_cursor() {
  stream_.write("\e[?25l");
}

void StreamEx::show_cursor() {
  stream_.write("\e[?25h");
}

void StreamEx::clear_screen() {
  stream_.write("\e[2J");
}

void StreamEx::insert_char(uint8_t count) {
  stream_.write("\e[");
  if (count != 1) {
    stream_.print(count);
  }
  stream_.write('@');
}

void StreamEx::delete_char(uint8_t count) {
  stream_.write("\e[");
  if (count != 1) {
    stream_.print(count);
  }
  stream_.write('P');
}

void StreamEx::erase_char(uint8_t count) {
  stream_.write("\e[");
  if (count != 1) {
    stream_.print(count);
  }
  stream_.write('X');
}

void StreamEx::set_style(Style style) {
  stream_.write("\e[");
  stream_.print(style);
  stream_.write('m');
}

} // namespace uANSI
