#ifndef I8080_REG_H
#define I8080_REG_H

#include <cstdint>

namespace i8080 {

struct Reg 
{
    enum : uint8_t {
        B = 0,
        C,
        D,
        E,
        H,
        L,
        M,
        A
    };
};

struct RegPair
{
    enum : uint8_t {
        BC = 0,
        DE, 
        HL,
        SP
    };
};




} // namespace


#endif