//
// Created by Dr. Brandon Wiley on 5/26/25.
//

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

    Cursor(Clock& clock);

    void setScreen(VTermScreen *screen);
    void setPosition(int col, int row);
    void setVisible(bool visible);
    void setBlinking(bool blinking);
    void setShape(int shape);
    void update();

  protected:
    Clock& clock;
    VTermScreen *screen = nullptr;

    int col = 0;
    int row = 0;
    int shape = block;
    int blink_start_time = 0;
    bool set_visible = false; // This is whether the cursor is set to visible in the terminal, NOT whether it is currently being drawn when blinking.
    bool set_blinking = false; // This is whether the terminal wants the cursor to be blinking or not.
    bool blinking_visible = false; // This is whether the cursor is currently being drawn on screen in the cursor blink cycle, NOT whether it is visible according to terminal settings.

    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void move() = 0;
};

#endif //CURSOR_H
