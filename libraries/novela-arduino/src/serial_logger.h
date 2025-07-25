//
// Created by Dr. Brandon Wiley on 5/27/25.
//

#ifndef SERIAL_LOGGER_H
#define SERIAL_LOGGER_H

#include <logger.h>

class SerialLogger : public Logger
{
  public:
    SerialLogger() = default;

  protected:
    void write(const char*) override;
};

#endif //SERIAL_LOGGER_H
