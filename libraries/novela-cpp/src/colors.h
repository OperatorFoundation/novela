

#ifndef COLORS_H
#define COLORS_H

// Copied from Adafruit DVI HSTX -> drivers/dvhstx/dvhstx.hpp
// I don't think there is a good way to import these without adding unwanted depencies on Arduino-specific code.

/*
MIT License

Copyright (c) 2024 Michael Bell and Pimoroni Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
enum Color{ // Note: This was renamed from TextColor to Color to avoid a name conflict in novela-arduino.
  TEXT_BLACK = 0,
  TEXT_RED,
  TEXT_GREEN,
  TEXT_BLUE,
  TEXT_YELLOW,
  TEXT_MAGENTA,
  TEXT_CYAN,
  TEXT_WHITE,

  BG_BLACK = 0,
  BG_RED = TEXT_RED << 3,
  BG_GREEN = TEXT_GREEN << 3,
  BG_BLUE = TEXT_BLUE << 3,
  BG_YELLOW = TEXT_YELLOW << 3,
  BG_MAGENTA = TEXT_MAGENTA << 3,
  BG_CYAN = TEXT_CYAN << 3,
  BG_WHITE = TEXT_WHITE << 3,

  ATTR_NORMAL_INTEN = 0,
  ATTR_LOW_INTEN = 1 << 6,
  ATTR_V_LOW_INTEN = 1 << 7 | ATTR_LOW_INTEN,
  };

#endif //COLORS_H
