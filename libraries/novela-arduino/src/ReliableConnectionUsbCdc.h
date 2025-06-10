//
// Created by Dr. Brandon Wiley on 6/5/25.
//

#ifndef RELIABLECONNECTIONUSBCDC_H
#define RELIABLECONNECTIONUSBCDC_H

#include <Connection.h>

class ReliableConnectionUsbCdc : public Connection
{
  public:
    ReliableConnectionUsbCdc();
    ~ReliableConnectionUsbCdc() {}

    int tryReadOne() const;
    char readOne() const;
    std::vector<char> read() const;
    bytes read(int size) const;
    void write(std::vector<char> bs) const;
};

#endif //RELIABLECONNECTIONUSBCDC_H
