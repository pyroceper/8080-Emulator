#include "i8080/cpu.h"

namespace i8080 {

CPU::CPU(Bus& bus) : bus_(bus) 
{
    reset();
}

void CPU::reset()
{
    regs_.fill(0);

    sp_ = 0;
    pc_ = 0;

}


} // namespace