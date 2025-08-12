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

    Cursor(Clock& clock);

    void setScreen(VTermScreen *screen);
    void setPosition(int col, int row);

    // Terminal-requested visibility and blinking
    void setVisible(bool visible);      // Terminal wants cursor visible/hidden
    void setBlinking(bool blinking);    // Terminal wants cursor blinking/steady
    void setShape(int shape);

    // Local control for typing/echo support
    void pauseBlinking();                // Temporarily stop blinking (for typing)
    void resumeBlinking();               // Resume blinking after pause
    void forceVisible();                 // Force cursor visible NOW (for local echo)
    void forceHidden();                  // Force cursor hidden NOW
    void resetBlinkPhase();              // Reset blink timer to start of "on" phase

    // Save/restore for proper rendering
    void saveUnder();                    // Save what's under the cursor
    void restoreUnder();                 // Restore what was under the cursor

    // Query current state
    bool isVisible() const { return set_visible && (!set_blinking || blinking_visible); }
    bool isBlinking() const { return set_blinking && !blink_paused; }
    bool isInBlinkOnPhase() const { return blinking_visible; }

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

    // Local control state
    bool blink_paused = false;          // Blinking temporarily paused
    bool force_visible_flag = false;    // Override to force visible
    bool force_hidden_flag = false;     // Override to force hidden

    // Saved content under cursor (platform-specific storage)
    bool has_saved_under = false;

    // Platform-specific implementations
    virtual void show() = 0;            // Draw cursor on screen
    virtual void hide() = 0;            // Remove cursor from screen
    virtual void move() = 0;            // Move cursor (may need to restore old position)

    // New platform-specific methods
    virtual void doSaveUnder() = 0;     // Platform-specific save implementation
    virtual void doRestoreUnder() = 0;  // Platform-specific restore implementation

  private:
    // Determine if cursor should be drawn right now
    bool shouldBeVisible() const;

    // Update cursor visibility based on all factors
    void updateVisibility();
};

#endif //CURSOR_H