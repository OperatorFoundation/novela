// cursor.cpp
// Refactored implementation using the improved Cursor API
//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "cursor.h"
#include <vterm.h>

Cursor::Cursor(Clock& clock) : clock(clock)
{
}

Cursor::~Cursor() = default;

void Cursor::setScreen(VTermScreen* newScreen)
{
  screen = newScreen;
}

void Cursor::setVisible(bool visible)
{
  if(visible && !set_visible)
  {
    // Cursor is being made visible
    blinking_visible = true; // Start in "on" phase
    blink_start_time = clock.now();
    show();
    set_visible = visible;
  }
  else if(!visible && set_visible)
  {
    // Cursor is being hidden
    blinking_visible = false;
    blink_start_time = 0;
    hide();
    set_visible = visible;
  }
  // else no change needed
}

void Cursor::setBlinking(bool blinking)
{
  if(set_visible)
  {
    if(blinking && !set_blinking)
    {
      // Start blinking from "on" phase
      blinking_visible = true;
      blink_start_time = clock.now();
      show();
    }
    else if(!blinking && set_blinking)
    {
      // Stop blinking - ensure cursor is visible
      blinking_visible = true; // Keep in "on" state
      blink_start_time = 0;
      show();
    }
  }

  set_blinking = blinking;
}

void Cursor::setShape(int newShape)
{
  if(shape != newShape)
  {
    // Hide old cursor shape if it was visible
    if(set_visible)
    {
      hide();
    }

    shape = newShape;

    // Show new cursor shape if it should be visible
    if(set_visible)
    {
      show();
    }
  }
}

void Cursor::setPosition(int newCol, int newRow)
{
  // Only process if position actually changed
  if(col != newCol || row != newRow)
  {
    // Restore what was under the old cursor position
    if(set_visible)
    {
      hide();
    }

    col = newCol;
    row = newRow;

    // Save what's under new position and show cursor
    if(set_visible)
    {
      show();
    }
  }
}

void Cursor::update()
{
  // Handle blinking if enabled and not paused
  if(set_visible && set_blinking)
  {
    if(clock.now() - blink_start_time > blink_duration)
    {
      blink_start_time = clock.now();

      // Toggle blink phase
      blinking_visible = !blinking_visible;
    }
  }
}