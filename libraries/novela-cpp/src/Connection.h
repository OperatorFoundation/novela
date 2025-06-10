#ifndef RELIABLE_CONNECTION_H_
#define RELIABLE_CONNECTION_H_

#define EXPORT
#include <vector>
#include <string>
#include <cstdint>

using byte = uint8_t;
using bytes = std::vector<byte>;

class EXPORT Connection
{
    public:
        virtual ~Connection() = default;

        void write(std::string s);
        void write(bytes bs);

        [[nodiscard]] virtual int tryReadOne() const = 0;
        [[nodiscard]] virtual char readOne() const = 0;
        [[nodiscard]] virtual bytes read(int size) const = 0;
        virtual void write(std::vector<char> bs) const = 0;
};

#endif