//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#ifndef CANVAS_H
#define CANVAS_H

#include <cstdint>

#include "colors.h"

class Canvas
{
  public:
    virtual ~Canvas() = default;

    virtual uint16_t getHeight() = 0;
    virtual uint16_t getWidth() = 0;

    virtual void drawCharacter(int16_t x, int16_t y, char c) = 0;
    virtual void fill(uint16_t color) = 0;
};

#endif //CANVAS_H
