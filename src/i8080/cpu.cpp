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

uint8_t CPU::fetch8()
{
    return bus_.read(pc_++);
}

uint16_t CPU::fetch16()
{
    uint8_t lo = fetch8();
    uint8_t hi = fetch8();

    return (static_cast<uint16_t>(hi) << 8) | lo;
}

void CPU::step()
{
    const uint8_t opcode = fetch8();

    switch (opcode) {
        case 0x00: // NOP
            break;
    }
}


} // namespace