#include "ReliableConnection.h"

#include <Arduino.h>

ReliableConnection::ReliableConnection()
{
}

int ReliableConnection::tryReadOne() const
{
  return Serial2.read();
}

char ReliableConnection::readOne() const
{
  // Wait for serial port to be ready
  while(!Serial2 || !Serial2.available())
  {
    delay(0.01);
  }

  int b = -1;
  while(b == -1)
  {
    b = Serial2.read();
  }

  return (byte)b;
}

bytes ReliableConnection::read(int size) const
{
  bytes r = bytes();

  while(r.size() < size)
  {
    byte b = readOne();
    r.push_back(b);
  }

  return r;
}

void ReliableConnection::write(bytes bs) const
{
  for (byte b : bs)
  {
    Serial2.write(b); // Send each byte
  }
}

