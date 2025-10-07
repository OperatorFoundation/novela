//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#ifndef BELL_H
#define BELL_H

#include <canvas.h>
#include <clock.h>

class Bell
{
  public:
    Bell(Canvas& canvas, Clock& clock) : canvas(canvas), clock(clock) {}

    int duration = 1000;

    void ring();
    void update();

  private:
    Canvas& canvas;
    Clock& clock;

    int start_time = 0;

    void show();
    void hide();
};

#endif //BELL_H
