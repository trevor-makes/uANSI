// https://github.com/trevor-makes/uANSI.git
// Copyright (c) 2021 Trevor Makes

#pragma once

#include <Stream.h>

#include <stdint.h>

namespace uANSI {

// Styles supported by `set_style`
enum Style {
  STYLE_DEFAULT   = 0,
  STYLE_BOLD      = 1,
  STYLE_FAINT     = 2,
  STYLE_ITALIC    = 3,
  STYLE_UNDERLINE = 4,
  STYLE_BLINK     = 5,
  STYLE_INVERSE   = 7,
};

// Colors supported by `set_foreground` and `set_background`
enum Color {
  COLOR_BLACK   = 0,
  COLOR_RED     = 1,
  COLOR_GREEN   = 2,
  COLOR_YELLOW  = 3,
  COLOR_BLUE    = 4,
  COLOR_MAGENTA = 5,
  COLOR_CYAN    = 6,
  COLOR_WHITE   = 7,
  COLOR_DEFAULT = 9,
};

// Special key codes returned by `StreamEx::read`
enum Key {
  KEY_UP    = 0x100,
  KEY_DOWN  = 0x101,
  KEY_RIGHT = 0x102,
  KEY_LEFT  = 0x103,
};

class StreamEx : public Stream {
  Stream& stream_;
  int peek_ = -1;
  enum class State : uint8_t {
    RESET,
    ESCAPE, // preceding input was "\e"
    CSI, // preceding input was "\e["
    CR, // preceding input was "\r"
  } state_ = State::RESET;

public:
  StreamEx(Stream& stream): stream_{stream} {}

  // Make type non-copyable
  StreamEx(const StreamEx&) = delete;
  StreamEx& operator=(const StreamEx&) = delete;

  // Virtual methods from Stream
  int peek(void) override;
  int read(void) override;
  int available(void) override { return peek() != -1; }

  // Virtual methods from Print
  int availableForWrite(void) override { return stream_.availableForWrite(); }
  void flush(void) override { stream_.flush(); }
  size_t write(uint8_t c) override { return stream_.write(c); }

  // Expose non-virtual methods from Print, as done by HardwareSerial
  using Print::write;

  // Move the cursor to row, col
  void move_cursor(uint8_t row, uint8_t col) {
    stream_.write("\e[");
    stream_.print(row);
    stream_.write(';');
    stream_.print(col);
    stream_.write('H');
  }

  // Hide the cursor
  void hide_cursor() {
    stream_.write("\e[?25l");
  }

  // Show the cursor
  void show_cursor() {
    stream_.write("\e[?25h");
  }

  // Erase all text and formatting
  void clear_screen() {
    stream_.write("\e[2J");
  }

  // Erase N characters following the cursor
  void erase_characters(uint8_t num) {
    stream_.write("\e[");
    stream_.print(num);
    stream_.write('X');
  }

  // Set the text style
  void set_style(Style style) {
    stream_.write("\e[");
    stream_.print(style);
    stream_.write('m');
  }

  // Set the text color
  void set_foreground(Color color) {
    set_style(Style(30 + color));
  }

  // Set the background color
  void set_background(Color color) {
    set_style(Style(40 + color));
  }
};

} // namespace uANSI
