//
// Created by Dr. Brandon Wiley on 6/5/25.
//

#include "logger.h"

#include <cstdio>
#include <cstdarg>

void Logger::info(const char *cs)
{
  log(Level::INFO, cs);
}

void Logger::infof(const char *format, ...)
{
  char buffer[256];  // Adjust size as needed

  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  log(Level::INFO, buffer);
}

void Logger::debug(const char *cs)
{
  log(Level::DEBUG, cs);
}

void Logger::debugf(const char *format, ...)
{
  char buffer[256];  // Adjust size as needed

  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  log(Level::DEBUG, buffer);
}

void Logger::setLevel(Level newLevel)
{
  threshold = newLevel;
}

void Logger::log(Level logLevel, const char *cs)
{
  if(logLevel >= threshold)
  {
    write(cs);
  }
}

void Logger::write(const char *cs)
{
}