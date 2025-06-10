#ifndef _RELIABLE_CONNECTION_SERIAL1_H_
#define _RELIABLE_CONNECTION_SERIAL1_H_

#include <Connection.h>

class ReliableConnectionSerial1 : public Connection
{
  public:
    ReliableConnectionSerial1();
    ~ReliableConnectionSerial1() {}

    int tryReadOne() const;
    char readOne() const;
    std::vector<char> read() const;
    bytes read(int size) const;
    void write(std::vector<char> bs) const;

	private:
		int maxBufferSize = 2048;
		int maxReadSize = 31;
};

#endif