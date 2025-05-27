//
// Created by Dr. Brandon Wiley on 5/26/25.
//

#include "Connection.h"

#include <cstring>

void Connection::write(std::string s)
{
  std::vector<uint8_t> bs(s.begin(), s.end());
  write(bs);
}

void Connection::write(char *cs)
{
  std::vector<uint8_t> bs(cs, cs + strlen(cs));
  write(bs);
}
