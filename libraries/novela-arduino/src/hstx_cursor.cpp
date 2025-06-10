//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#include "hstx_cursor.h"

HstxCursor::HstxCursor(Clock& clock, pimoroni::DVHSTX& hstx) : Cursor(clock), hstx(hstx)
{
  hide();
}

void HstxCursor::show()
{
  hstx.set_cursor(col, row);
}

void HstxCursor::hide()
{
  hstx.cursor_off();
}

void HstxCursor::move()
{
  hstx.set_cursor(col, row);
}
