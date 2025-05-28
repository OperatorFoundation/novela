//
// Created by Dr. Brandon Wiley on 5/27/25.
//

#include "serial_logger.h"

#include <Arduino.h>

void SerialLogger::debug(const char *cs)
{
  Serial.println(cs);
}