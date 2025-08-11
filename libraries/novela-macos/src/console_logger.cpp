//
// Created by Dr. Brandon Wiley on 5/27/25.
//

#include "console_logger.h"
#include <iostream>

void ConsoleLogger::write(const char* cs)
{
  std::cout << cs << std::endl;
  std::cout.flush();  // Ensure immediate output
}