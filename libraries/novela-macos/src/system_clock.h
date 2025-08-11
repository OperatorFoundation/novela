#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

#include <clock.h>
#include <chrono>

class SystemClock : public Clock
{
  public:
    SystemClock();

    int now() override;

  private:
    std::chrono::steady_clock::time_point start_time;
};

#endif // SYSTEM_CLOCK_H