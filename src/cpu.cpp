#include "cpu.h"

namespace i8080 {

CPU::CPU() 
{
    reset();
}

void CPU::reset()
{
    a_ = 0;
    b_ = 0;
    c_ = 0;
    d_ = 0;
    e_ = 0;
    h_ = 0;
    l_ = 0;

    sp_ = 0;
    pc_ = 0;
}


} // namespace