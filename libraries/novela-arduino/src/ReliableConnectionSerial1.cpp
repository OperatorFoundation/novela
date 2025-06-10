#include "ReliableConnectionSerial1.h"

#include <Arduino.h>

ReliableConnectionSerial1::ReliableConnectionSerial1()
{
}

int ReliableConnectionSerial1::tryReadOne() const
{
  return Serial1.read();
}

char ReliableConnectionSerial1::readOne() const
{
  // Wait for serial port to be ready
  while(!Serial1 || !Serial1.available())
  {
    delay(0.01);
  }

  int b = -1;
  while(b == -1)
  {
    b = Serial1.read();
  }

  return (byte)b;
}

std::vector<char> ReliableConnectionSerial1::read() const
{
  int available = Serial1.available();
  if (available == 0)
  {
    return std::vector<char>();
  }

  int totalBytesRead = 0;
  int lastBytesRead = 0;
  int maxRetries = 10;
  std::vector<char> results = std::vector<char>();
  while((available && (totalBytesRead < maxBufferSize)) || (lastBytesRead == maxReadSize))
  {
    if(!available && (lastBytesRead == maxReadSize))
    {
      for(int retries = 0; retries < maxRetries; retries++)
      {
        delay((retries + 1) * 10);

        available = Serial1.available();
        if(!available)
        {
          if(Serial.read() != -1)
          {
            Serial.println('!');
          }
        }

        Serial.println('@');

        if(available)
        {
          break;
        }
      }
    }

    std::vector<char> bs = std::vector<char>(available);
    int bytesRead = Serial1.readBytes(bs.data(), available);
    lastBytesRead = bytesRead;

    if(bytesRead)
    {
      results.insert(results.end(), bs.begin(), bs.begin() + bytesRead);
      totalBytesRead += bytesRead;
    }

    available = Serial1.available();
  }

  Serial.println(available);
  Serial.println(totalBytesRead);
  Serial.println(maxBufferSize);
  Serial.println(".");

  return results;
}

bytes ReliableConnectionSerial1::read(int size) const
{
  bytes r = bytes();

  while(r.size() < size)
  {
    byte b = readOne();
    r.push_back(b);
  }

  return r;
}

void ReliableConnectionSerial1::write(std::vector<char> bs) const
{
  Serial1.write(bs.data(), bs.size()); // Send each byte
}

