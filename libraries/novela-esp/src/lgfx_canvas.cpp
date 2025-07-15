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