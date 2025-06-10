//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#ifndef HSTX_CURSOR_H
#define HSTX_CURSOR_H

#include <Arduino.h>
#include <drivers/dvhstx/dvhstx.hpp>

#include <cursor.h>
#include <clock.h>

class HstxCursor : public Cursor
{
  public:
    HstxCursor(Clock& clock, pimoroni::DVHSTX& hstx);

    void show() override;
    void hide() override;
    void move() override;

  private:
   pimoroni::DVHSTX& hstx;
};

#endif //HSTX_CURSOR_H
