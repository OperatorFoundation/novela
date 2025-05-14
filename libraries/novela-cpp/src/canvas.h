//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef CANVAS_H
#define CANVAS_H

#include <cstdint>

class Canvas
{
  public:
    virtual ~Canvas() = default;

    virtual uint16_t getHeight();
    virtual uint16_t getWidth();
    virtual uint16_t getX();
    virtual uint16_t getY();
    virtual void setX(uint16_t newX);
    virtual void setY(uint16_t newY);
    virtual void incrementX();
    virtual void incrementY();

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void fillScreen(uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    [[nodiscard]] virtual bool getPixel(int16_t x, int16_t y) const;

    virtual void verticalScroll(int16_t y);
};

#endif //CANVAS_H
