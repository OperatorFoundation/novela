// cursor_refactored.h
// Improved Cursor API with better support for local echo and VT100 features

#ifndef CURSOR_H
#define CURSOR_H

#include <canvas.h>
#include <clock.h>
#include <vterm.h>

class Cursor
{
  public:
    static constexpr int block = 1;
    static constexpr int underline = 2;
    static constexpr int bar = 3;
    static constexpr int underline_steady = 4;
    static constexpr int bar_steady = 5;
    static constexpr int bar_blinking = 6;

    int blink_duration = 500;

    explicit Cursor(Clock& clock);
    virtual ~Cursor();

    void setScreen(VTermScreen *screen);
    void setPosition(int col, int row);

    // Terminal-requested visibility and blinking
    void setVisible(bool visible);      // Terminal wants cursor visible/hidden
    void setBlinking(bool blinking);    // Terminal wants cursor blinking/steady
    void setShape(int shape);

    void update();

  protected:
    Clock& clock;
    VTermScreen *screen = nullptr;

    // Position and appearance
    int col = 0;
    int row = 0;
    int shape = block;

    // Terminal-requested state
    bool set_visible = false;           // Terminal wants cursor visible
    bool set_blinking = false;          // Terminal wants cursor blinking

    // Blink cycle state
    bool blinking_visible = false;      // Currently in "on" phase of blink
    int blink_start_time = 0;

    // Platform-specific implementations
    virtual void show() = 0;            // Draw cursor on screen
    virtual void hide() = 0;            // Remove cursor from screen
};

#endif //CURSOR_H