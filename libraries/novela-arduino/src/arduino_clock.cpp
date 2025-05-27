//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "arduino_clock.h"

#include <Arduino.h>

int ArduinoClock::now()
{
  return millis();
}