//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#include "gfx_canvas.h"

#include <algorithm>
#include <cstring>
#include <colors.h>

// Initialize the internal GFXcanvas16 to 19 columns, 30 rows, and do not allocate a buffer.
// The buffer will be shared with HSTX instead.
GfxCanvas::GfxCanvas() : GFXcanvas16(91, 30, false) {}

GfxCanvas::~GfxCanvas()
{
  end();
}

bool GfxCanvas::begin()
{
  // Initialize HSTX, which will do all the HDMI drawing work
  bool result = hstx.init(91, 30, pimoroni::DVHSTX::MODE_TEXT_RGB111, false, DVHSTX_PINOUT_DEFAULT);
  if(!result)
  {
    return false;
  }

  // Get the HSTX buffer and store it in our GFXcanvas16 private buffer instance variable
  buffer = hstx.get_back_buffer<uint16_t>();

  return true;
}

void GfxCanvas::end()
{
  hstx.reset();
}

uint16_t GfxCanvas::getHeight()
{
  return _height;
}

uint16_t GfxCanvas::getWidth()
{
  return _width;
}


void GfxCanvas::drawCharacter(int16_t x, int16_t y, char c)
{
  GFXcanvas16::drawPixel(x, y, (TextColor::TEXT_WHITE << 8) | static_cast<int16_t>(c));
}

void GfxCanvas::fill(uint16_t color)
{
  GFXcanvas16::fillScreen(color);
}

void GfxCanvas::swap(bool copy_framebuffer)
{
  return;

  // hstx.flip_blocking();
  //
  // if(copy_framebuffer)
  // {
  //   memcpy(hstx.get_front_buffer<uint8_t>(), hstx.get_back_buffer<uint8_t>(), sizeof(uint16_t) * _width * _height);
  // }
  //
  // buffer = hstx.get_back_buffer<uint16_t>();
}

void GfxCanvas::clear()
{
  std::fill(getBuffer(), getBuffer() + getWidth() * getHeight(), 0);
}