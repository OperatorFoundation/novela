//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef GFX_CANVAS_H
#define GFX_CANVAS_H

#include <PicoDVI.h>
#include <canvas.h>

#include <cstdint>

class GfxCanvas : public Canvas
{
  public:
    GfxCanvas();

    uint16_t getHeight();
    uint16_t getWidth();
    uint16_t getX();
    uint16_t getY();
    void setX(uint16_t newX);
    void setY(uint16_t newY);
    void incrementX();
    void incrementY();

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    [[nodiscard]] virtual bool getPixel(int16_t x, int16_t y) const;

    void verticalScroll(int16_t y);

  private:
    DVItext1 gfx;
    unsigned int x = 0;
    unsigned int y = 0;
};

#endif //GFX_CANVAS_H
