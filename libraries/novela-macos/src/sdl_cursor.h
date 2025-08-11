//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#ifndef SDL_CURSOR_H
#define SDL_CURSOR_H

#include <cursor.h>
#include <clock.h>
#include <canvas.h>
#include <logger.h>
#include <vterm.h>
#include <LovyanGFX.hpp>

class SDLCursor : public Cursor
{
  public:
    SDLCursor(Clock& clock, Canvas& canvas, Logger& logger, lgfx::LGFX_Device* screen);

    void show() override;
    void hide() override;
    void move() override;

  private:
    Canvas& canvas;
    Logger& logger;
    lgfx::LGFX_Device* gfx_screen;

    // Cursor appearance settings
    uint16_t cursor_color = 0xFFFF;  // White by default
    int char_width = 10;             // Will be set based on font
    int char_height = 20;            // Will be set based on font

    // Store what was under the cursor for proper restoration
    uint16_t* saved_pixels = nullptr;
    bool has_saved_content = false;

    void initializeFontMetrics();
    void saveCursorArea();
    void restoreCursorArea();
    void drawCursorShape();
    int pixelX() const { return col * char_width; }
    int pixelY() const { return row * char_height; }
};

#endif // SDL_CURSOR_H