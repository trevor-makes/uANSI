# uANSI

**Deprecated! Merged into [core](https://github.com/trevor-makes/core)**

uANSI is a minimal library for changing the color and formatting of text sent to a compatible terminal emulator (such as xterm or PuTTY) using ANSI control sequences, similar to the Unix curses library. uANSI also supports parsing multi-character input sequences for special keys which do not map to ASCII characters, such as arrow keys.

uANSI is designed for Arduino and supports serial connections that implement the Stream interface, like Serial, SoftwareSerial, and SerialUSB. This package is intended to be used with [PlatformIO](https://platformio.org/), but the source files can be manually copied into a project when using the Arduino IDE.

To add uANSI to an existing PlatformIO project, modify the `platformio.ini` configuration file as follows:

```
lib_deps =
    https://github.com/trevor-makes/uANSI.git
```

uANSI is distributed under the [MIT license](LICENSE.txt)

## Cursor manipulation

The cursor can be repositioned with `move_cursor`:

```
uANSI::move_cursor(Serial, 5, 10); // move the cursor to row 5, column 10
Serial.write("Printed at row 5, column 10");
```

The cursor can be hidden/shown with `hide_cursor` and `show_cursor`:

```
uANSI::hide_cursor(Serial);
...
uANSI::show_cursor(Serial);
```

## Erasing text

The entire screen can be cleared with `clear_screen`:

```
uANSI::clear_screen(Serial);
```

Characters following the cursor can be erased with `erase_characters`:

```
uANSI::erase_characters(Serial, 8); // deletes 8 characters following the cursor
```

## Text style

The text style can be changed with `set_style`. The supported styles are:

- STYLE_DEFAULT
- STYLE_BOLD
- STYLE_FAINT
- STYLE_ITALIC
- STYLE_UNDERLINE
- STYLE_BLINK
- STYLE_INVERSE

Styles are additive, except for `STYLE_DEFAULT` which resets all attributes (including text colors).

```
uANSI::set_style(Serial, uANSI::STYLE_BOLD);
uANSI::set_style(Serial, uANSI::STYLE_INVERSE);
Serial.write("Bold and inverted text");
...
uANSI::set_style(Serial, uANSI::STYLE_DEFAULT);
Serial.write("Text with default style");
```

## Text color

The text color can be changed with `set_foreground` and `set_background`. The supported colors are:

- COLOR_BLACK
- COLOR_RED
- COLOR_GREEN
- COLOR_YELLOW
- COLOR_BLUE
- COLOR_MAGENTA
- COLOR_CYAN
- COLOR_WHITE
- COLOR_DEFAULT

```
uANSI::set_foreground(Serial, uANSI::COLOR_WHITE);
uANSI::set_background(Serial, uANSI::COLOR_BLUE);
Serial.write("White text on blue background");
```

## Input sequences

Multi-character input sequences can be parsed using `read_key(Serial)` instead of `Serial.read()`. Special keys are returned as values greater than 255, so the return value should be assigned type `int` rather than `char`:

```
int key = uANSI::read_key(stream);
switch (key) {
    case -1:
        // no keys pressed
        break;
    case uANSI::KEY_UP:
        // up arrow pressed
        break;
    case 'a':
        // 'a' key pressed
        break;
    ...
}
```

## Contributors

[Trevor Makes](mailto:the.trevor.makes@gmail.com)
