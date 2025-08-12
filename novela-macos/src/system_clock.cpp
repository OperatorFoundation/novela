#include "system_clock.h"

SystemClock::SystemClock()
{
  // Record the start time when the clock is created
  start_time = std::chrono::steady_clock::now();
}

int SystemClock::now()
{
  auto current_time = std::chrono::steady_clock::now();
  auto elapsed = current_time - start_time;

  // Return milliseconds since construction (matching Arduino millis() behavior)
  return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
}