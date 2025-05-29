//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "bell.h"

void Bell::ring()
{
  show();

  start_time = clock.now();
}

void Bell::update()
{
  if(clock.now() - start_time > duration)
  {
    hide();
    start_time = 0;
  }
}

void Bell::show()
{
  canvas.drawCharacter(0, 0, '*');
}

void Bell::hide()
{
  canvas.drawCharacter(0, 0, ' ');
}
