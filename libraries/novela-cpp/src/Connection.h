#ifndef RELIABLE_CONNECTION_H_
#define RELIABLE_CONNECTION_H_

#define EXPORT
#include <vector>

using byte = uint8_t;
using bytes = std::vector<byte>;

class EXPORT Connection
{
    public:
        virtual ~Connection() = default;

        [[nodiscard]] virtual char readOne() const = 0;
        [[nodiscard]] virtual bytes read(int size) const = 0;
        virtual void write(bytes bs) const = 0;
};

#endif