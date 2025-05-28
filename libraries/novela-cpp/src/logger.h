//
// Created by Dr. Brandon Wiley on 5/27/25.
//

#ifndef LOGGER_H
#define LOGGER_H

class Logger
{
  public:
    virtual ~Logger() = default;

    virtual void debug(const char *);
};

#endif //LOGGER_H
