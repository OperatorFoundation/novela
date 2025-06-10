//
// Created by Dr. Brandon Wiley on 6/5/25.
//

#include "ReliableConnectionUsbCdc.h"

#include <Arduino.h>

ReliableConnectionUsbCdc::ReliableConnectionUsbCdc()
{
}

int ReliableConnectionUsbCdc::tryReadOne() const
{
  return Serial.read();
}

char ReliableConnectionUsbCdc::readOne() const
{
  // Wait for serial port to be ready
  while(!Serial || !Serial.available())
  {
    delay(0.01);
  }

  int b = -1;
  while(b == -1)
  {
    b = Serial.read();
  }

  return (byte)b;
}

std::vector<char> ReliableConnectionUsbCdc::read() const
{
  int available = Serial.available();
  if (available == 0)
  {
    return std::vector<char>();
  }

  std::vector<char> bs = std::vector<char>(available);

  int bytesRead = Serial.readBytes(bs.data(), available);
  bs.resize(bytesRead);

  return bs;
}

bytes ReliableConnectionUsbCdc::read(int size) const
{
  bytes r = bytes();

  while(r.size() < size)
  {
    byte b = readOne();
    r.push_back(b);
  }

  return r;
}

void ReliableConnectionUsbCdc::write(std::vector<char> bs) const
{
  Serial.write(bs.data(), bs.size()); // Send each byte
}

