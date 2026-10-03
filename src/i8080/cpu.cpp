#include "i8080/cpu.h"
#include <fmt/format.h>

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

    halted_ = false;
}

uint16_t CPU::get_rp(uint8_t rp) const 
{
    switch (rp) {
        case RegPair::BC: return (static_cast<uint16_t>(regs_[Reg::B]) << 8) | regs_[Reg::C];
        case RegPair::DE: return (static_cast<uint16_t>(regs_[Reg::D]) << 8) | regs_[Reg::E];
        case RegPair::HL: return (static_cast<uint16_t>(regs_[Reg::H]) << 8) | regs_[Reg::L];
        case RegPair::SP: return sp_;
    }
    return 0;
}

void CPU::set_rp(uint8_t rp, uint16_t value)
{
    switch (rp) {
        case RegPair::BC: {
            regs_[Reg::B] = static_cast<uint8_t>(value >> 8);
            regs_[Reg::C] = static_cast<uint8_t>(value & 0xFF);
        } break;
        case RegPair::DE: {
            regs_[Reg::D] = static_cast<uint8_t>(value >> 8);
            regs_[Reg::E] = static_cast<uint8_t>(value & 0xFF);
        } break;
        case RegPair::HL: {
            regs_[Reg::H] = static_cast<uint8_t>(value >> 8);
            regs_[Reg::L] = static_cast<uint8_t>(value & 0xFF);
        } break;
        case RegPair::SP: {
            sp_ = value;
        } break;
    }
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

        case 0x76: {
            // HLT
            halted_ = true;
        } break;

        // LXI - load immediate instructions
        case 0x01: set_rp(RegPair::BC, fetch16()); break; // LXI B,d16
        case 0x11: set_rp(RegPair::DE, fetch16()); break; // LXI D,d16
        case 0x21: set_rp(RegPair::HL, fetch16()); break; // LXI H,d16
        case 0x31: set_rp(RegPair::SP, fetch16()); break; // LXI SP,d16

        default: {
            fmt::print(stderr, "Unimplemented opcode {:02X} at {:04X}\n", opcode, static_cast<uint16_t>(pc_ - 1));
        }
    }
}


} // namespace