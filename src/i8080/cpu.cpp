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

void CPU::set_reg(uint8_t reg, uint8_t value)
{
    if (reg == Reg::M) {
        bus_.write(get_rp(RegPair::HL), value);
    } else {
        regs_[reg] = value;
    }
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

        // STAX - store accumulator, memory[BC] = A; memory[DE] = A;
        case 0x02: bus_.write(get_rp(RegPair::BC), a()); break; // STAX B
        case 0x12: bus_.write(get_rp(RegPair::DE), a()); break; // STAX D
        // LDAX - load A from memory address in register pair BC or DE
        case 0x0A: set_reg(Reg::A, bus_.read(get_rp(RegPair::BC))); break; // LDAX B
        case 0x1A: set_reg(Reg::A, bus_.read(get_rp(RegPair::DE))); break; // LDAX D

        // SHLD - store H and L direct
        case 0x22: { // SHLD a16
            const uint16_t addr = fetch16();
            bus_.write(addr, l());
            bus_.write(addr + 1, h());
        } break;
        // LHLD - load H and L from address
        case 0x2A: { // LHLD a16
            const uint16_t addr = fetch16();

            set_reg(Reg::L, bus_.read(addr));
            set_reg(Reg::H, bus_.read(addr + 1));
        } break;

        // STA - store A in memory
        case 0x32: { // STA a16
            const uint16_t addr = fetch16();
            bus_.write(addr, a());
        } break;
        // LDA - load A from memory
        case 0x3A: { // LDA a16
            const uint16_t addr = fetch16();
            set_reg(Reg::A, bus_.read(addr));
        } break;

        // MVI - move immediate into register or memory
        case 0x06: set_reg(Reg::B, fetch8()); break; // MVI B,d8
        case 0x16: set_reg(Reg::D, fetch8()); break; // MVI C,d8
        case 0x26: set_reg(Reg::H, fetch8()); break; // MVI H,d8
        case 0x36: set_reg(Reg::M, fetch8()); break; // MVI M,d8 ; M -> (HL)
        case 0x0E: set_reg(Reg::C, fetch8()); break; // MVI C,d8
        case 0x1E: set_reg(Reg::E, fetch8()); break; // MVI E,d8
        case 0x2E: set_reg(Reg::L, fetch8()); break; // MVI L,d8
        case 0x3E: set_reg(Reg::A, fetch8()); break; // MVI A,d8

        default: {
            fmt::print(stderr, "Unimplemented opcode {:02X} at {:04X}\n", opcode, static_cast<uint16_t>(pc_ - 1));
        }
    }
}


} // namespace