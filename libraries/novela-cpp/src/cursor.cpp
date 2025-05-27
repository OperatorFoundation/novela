//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "cursor.h"

#include <vterm.h>

void Cursor::setVisible(bool visible)
{
  // Cursor is already on the screen
  if(set_visible)
  {
    // Hide cursor, stop blink cycle
    if(!visible)
    {
      set_visible = visible;
      blinking_visible = false;
      blink_start_time = 0;
      hide();
    }
    // Else no change, keep blinking
  }
  else // The cursor is not on the screen right now, but it might be blinking
  {
    // Reveal cursor
    if(visible)
    {
      set_visible = visible;
      blinking_visible = true;
      blink_start_time = clock.now();
      show();
    }
    else // Hiding the cursor, which is not currently visible, so at least stop blinking
    {
      set_visible = visible;
      blinking_visible = false;
      blink_start_time = 0;
      // No need to hide it on screen, it's already hidden due to blinking.
    }
  }
}

void Cursor::setBlinking(bool blinking)
{
  if(set_visible)
  {
    // Start blinking
    if(blinking && !set_blinking)
    {
      blinking_visible = true;
      blink_start_time = clock.now();
      show();
    }
  }

  set_blinking = blinking;
}

void Cursor::setShape(int newShape)
{
  if(set_visible)
  {
    if(set_blinking)
    {
      if(blinking_visible)
      {
        hide();
      }
    }
    else
    {
      hide();
    }
  }

  shape = newShape;

  if(set_visible)
  {
    if(set_blinking)
    {
      if(blinking_visible)
      {
        show();
      }
    }
    else
    {
      show();
    }
  }
}

void Cursor::setPosition(int newCol, int newRow)
{
  if(set_visible)
  {
    if(set_blinking)
    {
      if(blinking_visible)
      {
        hide();
      }
    }
    else
    {
      hide();
    }
  }

  col = newCol;
  row = newRow;

  if(set_visible)
  {
    if(set_blinking)
    {
      if(blinking_visible)
      {
        show();
      }
    }
    else
    {
      show();
    }
  }
}

void Cursor::update()
{
  if(set_visible && set_blinking)
  {
    if(clock.now() - blink_start_time > blink_duration)
    {
      blink_start_time = clock.now();

      if(blinking_visible) // Currently visible
      {
        // Hide
        hide();
        blinking_visible = false;
      }
      else // Currently hidden
      {
        // Show
        show();
        blinking_visible = true;
      }
    }
  }
}

void Cursor::show()
{
  canvas.drawPixel(canvas.getX(), canvas.getY(), '#'); // FIXME - how do other terminals handle the cursor?
}

void Cursor::hide()
{
  VTermPos pos;
  pos.row = canvas.getY() - 1;
  pos.col = canvas.getX() - 1;

  VTermScreenCell cell;

  if(vterm_screen_get_cell(screen, pos, &cell))
  {
    if(cell.chars[0] == 0)
    {
      canvas.drawPixel(canvas.getX(), canvas.getY(), ' ');
    }
    else
    {
      canvas.drawPixel(canvas.getX(), canvas.getY(), cell.chars[0]); // FIXME - how do other terminals handle the cursor?
    }
  }
}

