//
// Created by Dr. Brandon Wiley on 5/29/25.
//

#ifndef SOFT_CURSOR_H
#define SOFT_CURSOR_H

#include <cursor.h>
#include <clock.h>
#include <canvas.h>
#include <logger.h>
#include <vterm.h>

class SoftCursor : public Cursor
{
  public:
    SoftCursor(Clock& clock, Canvas& canvas, Logger& logger);

    void show() override;
    void hide() override;
    void move() override;

  private:
    Canvas& canvas;
    Logger& logger;
};

#endif //SOFT_CURSOR_H
