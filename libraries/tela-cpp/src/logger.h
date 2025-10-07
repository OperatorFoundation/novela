//
// Created by Dr. Brandon Wiley on 5/27/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#ifdef DEBUG
#undef DEBUG
#endif

#include <cstdarg>

class Logger
{
  public:
    enum class Level : unsigned char
    {
      DEBUG = 1,
      INFO = 2,
      WARNING = 3,
      ERROR = 4,
      FATAL = 5
    };

    virtual ~Logger() = default;

    void info(const char *);
    void infof(const char *, ...);
    void debug(const char *);
    void debugf(const char *, ...);
    void setLevel(Level level);

  protected:
    Level threshold = Level::INFO;

    void log(Level logLevel, const char *cs);
    virtual void write(const char *cs);
};

#endif //LOGGER_H
