//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef GFX_CANVAS_H
#define GFX_CANVAS_H

#include <Adafruit_dvhstx.h>
#include <canvas.h>

#include <cstdint>

// A GfxCanvas implements the generic Canvas interface using the Adafruit GFX library
// GfxCanvas inherits from novela-cpp's Canvas so that it can implement its virtual functions
// GfxCanvas inherits from Adafruit GFX's GFXcanvas16 in order get access to the private buffer instance variable
class GfxCanvas : public Canvas, public GFXcanvas16
{
  public:
    GfxCanvas();
    ~GfxCanvas();

    bool begin();
    void end();
    void swap(bool copy_framebuffer);

    // Canvas
    uint16_t getHeight() override;
    uint16_t getWidth() override;

    void drawCharacter(int16_t x, int16_t y, char c) override;
    void fill(uint16_t color) override;

    void clear();
    // End Canvas

  private:
    mutable pimoroni::DVHSTX hstx;

    unsigned int x = 0;
    unsigned int y = 0;
};

#endif //GFX_CANVAS_H
