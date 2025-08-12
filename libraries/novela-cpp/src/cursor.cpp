// cursor.cpp
// Refactored implementation using the improved Cursor API
//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "cursor.h"
#include <vterm.h>

Cursor::Cursor(Clock& clock) : clock(clock) {}

void Cursor::setScreen(VTermScreen* newScreen)
{
    screen = newScreen;
}

void Cursor::setVisible(bool visible)
{
    // Store what visibility was before
    bool was_shown = shouldBeVisible();

    // Update terminal visibility setting
    set_visible = visible;

    if (visible && !was_shown)
    {
        // Cursor is being made visible
        blinking_visible = true;  // Start in "on" phase
        blink_start_time = clock.now();
        updateVisibility();
    }
    else if (!visible && was_shown)
    {
        // Cursor is being hidden
        blinking_visible = false;
        blink_start_time = 0;
        updateVisibility();
    }
    // else no change needed
}

void Cursor::setBlinking(bool blinking)
{
    bool was_shown = shouldBeVisible();
    bool was_blinking = set_blinking;

    set_blinking = blinking;

    if (set_visible)
    {
        if (blinking && !was_blinking)
        {
            // Start blinking from "on" phase
            blinking_visible = true;
            blink_start_time = clock.now();
            updateVisibility();
        }
        else if (!blinking && was_blinking)
        {
            // Stop blinking - ensure cursor is visible
            blinking_visible = true;  // Keep in "on" state
            blink_start_time = 0;
            updateVisibility();
        }
    }
}

void Cursor::setShape(int newShape)
{
    if (shape != newShape)
    {
        bool was_shown = shouldBeVisible();

        // Hide old cursor shape if it was visible
        if (was_shown && has_saved_under)
        {
            hide();
            restoreUnder();
        }

        shape = newShape;

        // Show new cursor shape if it should be visible
        if (was_shown)
        {
            saveUnder();
            show();
        }
    }
}

void Cursor::setPosition(int newCol, int newRow)
{
    // Only process if position actually changed
    if (col != newCol || row != newRow)
    {
        bool was_shown = shouldBeVisible();

        // Restore what was under the old cursor position
        if (was_shown && has_saved_under)
        {
            hide();
            restoreUnder();
        }

        col = newCol;
        row = newRow;

        // Save what's under new position and show cursor
        if (was_shown)
        {
            saveUnder();
            show();
        }
    }
}

void Cursor::pauseBlinking()
{
    if (!blink_paused)
    {
        blink_paused = true;

        // If blinking and currently hidden, make visible
        if (set_blinking && !blinking_visible)
        {
            blinking_visible = true;
            updateVisibility();
        }
    }
}

void Cursor::resumeBlinking()
{
    if (blink_paused)
    {
        blink_paused = false;

        // Reset blink cycle to start from "on" phase
        if (set_blinking)
        {
            blink_start_time = clock.now();
            blinking_visible = true;
        }

        updateVisibility();
    }
}

void Cursor::forceVisible()
{
    force_visible_flag = true;
    force_hidden_flag = false;
    updateVisibility();
}

void Cursor::forceHidden()
{
    force_hidden_flag = true;
    force_visible_flag = false;
    updateVisibility();
}

void Cursor::resetBlinkPhase()
{
    blink_start_time = clock.now();
    blinking_visible = true;

    if (set_visible && set_blinking && !blink_paused)
    {
        updateVisibility();
    }
}

void Cursor::saveUnder()
{
    if (!has_saved_under)
    {
        doSaveUnder();
        has_saved_under = true;
    }
}

void Cursor::restoreUnder()
{
    if (has_saved_under)
    {
        doRestoreUnder();
        has_saved_under = false;
    }
}

void Cursor::update()
{
    // Handle blinking if enabled and not paused
    if (set_visible && set_blinking && !blink_paused)
    {
        if (clock.now() - blink_start_time > blink_duration)
        {
            blink_start_time = clock.now();

            // Toggle blink phase
            blinking_visible = !blinking_visible;

            updateVisibility();
        }
    }
}

bool Cursor::shouldBeVisible() const
{
    // Force flags override everything
    if (force_hidden_flag) return false;
    if (force_visible_flag) return true;

    // Terminal doesn't want cursor visible
    if (!set_visible) return false;

    // Handle blinking logic
    if (set_blinking && !blink_paused)
    {
        return blinking_visible;
    }

    // Non-blinking cursor or paused blinking - always visible
    return true;
}

void Cursor::updateVisibility()
{
    bool should_show = shouldBeVisible();
    bool currently_shown = has_saved_under;

    if (should_show && !currently_shown)
    {
        // Need to show cursor
        saveUnder();
        show();
    }
    else if (!should_show && currently_shown)
    {
        // Need to hide cursor
        hide();
        restoreUnder();
    }
    // else no change needed
}