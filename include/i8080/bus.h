#ifndef I8080_BUS_H
#define I8080_BUS_H

#include <array>
#include <cstdint>

namespace i8080 {

class Bus 
{
    public:
        uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t val);

    private:
        std::array<uint8_t, 0x10000> address_space_{}; // 65536
};

} // namespace

#endif