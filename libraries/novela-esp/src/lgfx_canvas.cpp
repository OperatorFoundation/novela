//
// Created by Dr. Brandon Wiley on 5/13/25.
//

#include "lgfx_canvas.h"

#include <Arduino.h>

#include <algorithm>
#include <cstring>
#include <colors.h>

LGFXCanvas::LGFXCanvas() {}

LGFXCanvas::~LGFXCanvas()
{
  end();
}

bool LGFXCanvas::begin(lgfx::LGFX_Device* newScreen)
{
  screen = newScreen;
  screen->setFont(&fonts::Font2);
  screen->setTextSize(1);
  screen->setTextWrap(false);
  clear();

  char_width = screen->textWidth("M");
  char_height = screen->fontHeight();

  return true;
}

void LGFXCanvas::end()
{
  clear();
}

uint16_t LGFXCanvas::getHeight()
{
  if(screen == nullptr)
  {
    return 0;
  }

  return static_cast<uint16_t>(screen->height() / char_height);
}

uint16_t LGFXCanvas::getWidth()
{
  if(screen == nullptr)
  {
    return 0;
  }

  return static_cast<uint16_t>(screen->width() / char_width);
}


void LGFXCanvas::drawCharacter(int16_t x, int16_t y, char c)
{
  if(screen == nullptr)
  {
    return;
  }

  if(c == ' ')
  {
    screen->drawChar(x * char_width, y * char_height, c, TFT_BLACK, TFT_BLACK, 1);
  }
  else
  {
    screen->drawChar(x * char_width, y * char_height, c, TFT_BLACK, fg_color, 1);
  }
}

void LGFXCanvas::fill(uint16_t color)
{
  if(screen == nullptr)
  {
    return;
  }

  screen->fillScreen(color);
}

void LGFXCanvas::clear()
{
  if(screen == nullptr)
  {
    return;
  }

  screen->fillScreen(TFT_BLACK);
}

void LGFXCanvas::nextTheme()
{
  if(screen == nullptr)
  {
    return;
  }

  theme = (theme + 1) % themes.size();
  fg_color = themes[theme];
}

int LGFXCanvas::move(int sourceStartX, int sourceStartY, int sourceEndX, int sourceEndY, int destStartX, int destStartY, int destEndX, int destEndY)
{
  int src_x = sourceStartX * char_width;
  int src_y = sourceStartY * char_height;
  int dest_x = destStartX * char_width;
  int dest_y = destStartY * char_height;
  int pixel_width = (sourceEndX - sourceStartX) * char_width;
  int pixel_height = (sourceEndY - sourceStartY) * char_height;

  // Allocate buffer for the pixel data
  size_t buffer_size = pixel_width * pixel_height;
  uint16_t* buffer = new uint16_t[buffer_size];

  // Read the source rectangle
  screen->readRect(src_x, src_y, pixel_width, pixel_height, buffer);

  // Write to the destination
  screen->pushImage(dest_x, dest_y, pixel_width, pixel_height, buffer);

  // Clean up
  delete[] buffer;

  // Now we need to clear the area that was exposed by the move
  // This depends on the direction of movement
  int exposedStartX = 0;
  int exposedStartY = 0;
  int exposedEndX = 0;
  int exposedEndY = 0;
  bool need_clear = false;

  if (destStartY < sourceStartY)
  {
    // Moved up - exposed area is at the bottom
    exposedStartY = destEndY;
    exposedEndY = sourceEndY;
    exposedStartX = sourceStartX;
    exposedEndX = sourceEndX;
    need_clear = true;
  }
  else if (destStartY > sourceStartY)
  {
    // Moved down - exposed area is at the top
    exposedStartY = sourceStartY;
    exposedEndY = destStartY;
    exposedStartX = sourceStartX;
    exposedEndX = sourceEndX;
    need_clear = true;
  }
  else if (destStartX < sourceStartX)
  {
    // Moved left - exposed area is on the right
    exposedStartY = sourceStartY;
    exposedEndY = sourceEndY;
    exposedStartX = destEndX;
    exposedEndX = sourceEndX;
    need_clear = true;
  }
  else if (destStartX > sourceStartX)
  {
    // Moved right - exposed area is on the left
    exposedStartY = sourceStartY;
    exposedEndY = sourceEndY;
    exposedStartX = sourceStartX;
    exposedEndX = destStartX;
    need_clear = true;
  }

  // Clear the exposed area
  if (need_clear)
  {
    uint16_t bg_color = TFT_BLACK;

    int clear_x = exposedStartX * char_width;
    int clear_y = exposedStartY * char_height;
    int clear_width = (exposedEndX - exposedStartX) * char_width;
    int clear_height = (exposedEndY - exposedStartY) * char_height;

    screen->fillRect(clear_x, clear_y, clear_width, clear_height, bg_color);
  }

  return 1;
}