#ifndef _RELIABLE_CONNECTION_H_
#define _RELIABLE_CONNECTION_H_

#include <Arduino.h>

#include <Connection.h>

class ReliableConnection : public Connection
{
  public:
    ReliableConnection(int tx, int rx);
    ~ReliableConnection() {}

    int tryReadOne() const;
    char readOne() const;
    bytes read(int size) const;
    void write(bytes bs) const;
};

#endif