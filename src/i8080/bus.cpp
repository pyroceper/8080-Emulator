#include "i8080/bus.h"

namespace i8080 {

uint8_t Bus::read(uint16_t address) const
{
    return address_space_[address];
}

void Bus::write(uint16_t address, uint8_t val)
{
    address_space_[address] = val;
}


} // namespace