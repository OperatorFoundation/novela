//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef GFX_CANVAS_H
#define GFX_CANVAS_H

#include <canvas.h>

#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>

namespace TerminalColors
{
  // Classic CRT monitors
  const uint16_t VT100_GREEN = 0x07E0;        // Pure green
  const uint16_t VT220_AMBER = 0xFE00;        // Amber
  const uint16_t IBM_5151 = 0x37E8;           // IBM monochrome
  const uint16_t APPLE_IIE = 0x37E0;          // Apple //e

  // Modern terminal greens
  const uint16_t MATRIX = 0x05E0;             // Matrix dark green
  const uint16_t TERMINAL_APP = 0x9FE5;       // macOS Terminal
  const uint16_t PHOSPHOR_P1 = 0x4FE5;        // Classic phosphor
  const uint16_t PHOSPHOR_P3 = 0xFDC0;        // Amber phosphor
}

// An LGFXSDLCanvas implements the generic tela Canvas interface using the LovyanGFX library
// LGFXSDLCanvas inherits from tela-cpp's Canvas so that it can implement its virtual functions
class LGFXSDLCanvas : public Canvas
{
  public:
    LGFXSDLCanvas();
    ~LGFXSDLCanvas();

    bool begin(LGFX *newScreen);
    void end();
    void swap(bool copy_framebuffer);

    // Canvas
    uint16_t getHeight() override;
    uint16_t getWidth() override;

    void drawCharacter(int16_t x, int16_t y, char c) override;
    void fill(uint16_t color) override;

    int move(int sourceStartX, int sourceStartY, int sourceEndX, int sourceEndY, int destStartX, int destStartY, int destEndX, int destEndY) override;

    void clear();
    // End Canvas

    int getCurrentThemeColor() const;
    void nextTheme();

  private:
    LGFX *screen = nullptr;

    // In pixels - these are just default values, depends on font
    unsigned int char_height = 20;
    unsigned int char_width = 10;

    int fg_color = TerminalColors::VT100_GREEN;

    std::vector<int> themes = {
      TerminalColors::VT100_GREEN,
      TerminalColors::VT220_AMBER,
      TerminalColors::IBM_5151,
      TerminalColors::APPLE_IIE,
      TerminalColors::MATRIX,
      TerminalColors::TERMINAL_APP,
      TerminalColors::PHOSPHOR_P1,
      TerminalColors::PHOSPHOR_P3
    };

    int theme = 0;

    unsigned int x = 0;
    unsigned int y = 0;
};

#endif //GFX_CANVAS_H
