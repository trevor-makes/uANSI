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

// Special key codes returned by `read_key`
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
};

// Move the cursor to row, col
template <typename T>
void move_cursor(T& stream, uint8_t row, uint8_t col) {
  stream.write("\e[");
  stream.print(row);
  stream.write(';');
  stream.print(col);
  stream.write('H');
}

// Hide the cursor
template <typename T>
void hide_cursor(T& stream) {
  stream.write("\e[?25l");
}

// Show the cursor
template <typename T>
void show_cursor(T& stream) {
  stream.write("\e[?25h");
}

// Erase all text and formatting
template <typename T>
void clear_screen(T& stream) {
  stream.write("\e[2J");
}

// Erase N characters following the cursor
template <typename T>
void erase_characters(T& stream, uint8_t num) {
  stream.write("\e[");
  stream.print(num);
  stream.write('X');
}

// Set the text style
template <typename T>
void set_style(T& stream, Style style) {
  stream.write("\e[");
  stream.print(style);
  stream.write('m');
}

// Set the text color
template <typename T>
void set_foreground(T& stream, Color color) {
  set_style(stream, 30 + color);
}

// Set the background color
template <typename T>
void set_background(T& stream, Color color) {
  set_style(stream, 40 + color);
}

} // namespace uANSI
