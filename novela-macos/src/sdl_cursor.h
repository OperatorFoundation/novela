// sdl_cursor.h
// SDL Cursor header that matches the existing implementation
//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#ifndef SDL_CURSOR_H
#define SDL_CURSOR_H

#include "cursor.h"
#include <canvas.h>
#include <logger.h>
#include <LGFX_AUTODETECT.hpp>

class SDLCursor : public Cursor
{
  public:
    SDLCursor(Clock& clock, Canvas& canvas, Logger& logger, LGFX* screen);
    virtual ~SDLCursor() {
      if (saved_pixels) {
        delete[] saved_pixels;
      }
    }

  protected:
    // Required virtual methods from Cursor base class
    void show() override;
    void hide() override;
    void move() override;

    // New methods required by refactored Cursor API
    void doSaveUnder() override;
    void doRestoreUnder() override;

  private:
    Canvas& canvas;
    Logger& logger;
    LGFX* gfx_screen;

    // Font metrics
    int char_width = 8;
    int char_height = 8;

    // Cursor appearance
    uint16_t cursor_color = 0xFFFF;  // Default white

    // Storage for saved pixels under cursor
    uint16_t* saved_pixels = nullptr;
    bool has_saved_content = false;

    // Helper methods
    void initializeFontMetrics();
    void saveCursorArea();
    void restoreCursorArea();
    void drawCursorShape();

    // Convert character position to pixel position
    int pixelX() const { return col * char_width; }
    int pixelY() const { return row * char_height; }
};

#endif // SDL_CURSOR_H