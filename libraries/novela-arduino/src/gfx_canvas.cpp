//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#include "gfx_canvas.h"

#include <cstring>

GfxCanvas::GfxCanvas(DVItext1& gfx) : gfx(gfx)
{
  gfx.begin();
  gfx.fillScreen(0);
  gfx.setCursor(0, 0);
}

uint16_t GfxCanvas::getHeight()
{
  return 240;
}

uint16_t GfxCanvas::getWidth()
{
  return 640;
}

uint16_t GfxCanvas::getX()
{
  return x;
}

uint16_t GfxCanvas::getY()
{
  return y;
}

void GfxCanvas::setX(uint16_t newX)
{
  x = newX;
}

void GfxCanvas::setY(uint16_t newY)
{
  y = newY;
}

void GfxCanvas::incrementX()
{
  x++;
}

void GfxCanvas::incrementY()
{
  y++;
}

void GfxCanvas::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  gfx.drawPixel(x, y, color);
}

void GfxCanvas::fillScreen(uint16_t color)
{
  gfx.fillScreen(color);
}

void GfxCanvas::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  gfx.drawFastVLine(x, y, h, color);
}

void GfxCanvas::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  gfx.drawFastHLine(x, y, w, color);
}

[[nodiscard]] bool GfxCanvas::getPixel(int16_t x, int16_t y) const
{
  return gfx.getPixel(x, y);
}

void GfxCanvas::verticalScroll(int16_t y)
{
  std::memmove(gfx.getBuffer(), gfx.getBuffer() + getWidth(), getWidth() * (getHeight() - 1) * 2);
}
