//
// Created by Dr. Brandon Wiley on 5/27/25.
//
#ifndef CONSOLE_LOGGER_H
#define CONSOLE_LOGGER_H

#include <logger.h>

class ConsoleLogger : public Logger
{
  public:
    ConsoleLogger() = default;

  protected:
    void write(const char* cs) override;
};

#endif // CONSOLE_LOGGER_H