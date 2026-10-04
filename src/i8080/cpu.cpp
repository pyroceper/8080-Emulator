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

uint8_t CPU::get_reg(uint8_t reg) const
{
    if (reg == Reg::M) {
        return bus_.read(get_rp(RegPair::HL));
    }
    return regs_[reg];
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

void CPU::set_zsp(uint8_t result)
{
    flags_.z = (result == 0); // zero
    flags_.s = (result & 0x80) != 0; // sign
    flags_.p = (std::popcount(result) % 2) == 0; // parity
}

void CPU::set_c(bool carry)
{
    flags_.c = carry;
}

void CPU::exec_inr(uint8_t reg)
{
    uint8_t value = get_reg(reg);
    uint8_t result = value  + 1;

    // aux carry
    flags_.ac = (value & 0x0F) == 0x0F; // 0b1111 -> then carry into high nibble 
    set_zsp(result);

    set_reg(reg, result);
}

void CPU::exec_dcr(uint8_t reg)
{
    uint8_t value = get_reg(reg);
    uint8_t result = value - 1;

    // aux carry
    flags_.ac = (value & 0x0F) != 0x00; // 0b0000 -> then borrow from high nibble, set when no borrow
    set_zsp(result);

    set_reg(reg, result);
}

void CPU::exec_rlc()
{
    uint8_t high_order_bit = (a() >> 7) & 0x01;
    flags_.c = high_order_bit;

    uint8_t result = (a() << 1) | high_order_bit;

    set_reg(Reg::A, result);
}

void CPU::exec_rrc()
{
    uint8_t low_order_bit =  a() & 0x01;
    flags_.c = low_order_bit;

    uint8_t result = (a() >> 1) | (low_order_bit << 7);

    set_reg(Reg::A, result);
}

void CPU::exec_ral()
{
    uint8_t carry_bit = flags_.c;
    uint8_t high_order_bit = (a() >> 7) & 0x01;
    flags_.c = high_order_bit;
    
    uint8_t result = (a() << 1) | carry_bit;

    set_reg(Reg::A, result);
}

void CPU::exec_rar()
{
    uint8_t carry_bit = flags_.c;
    uint8_t low_order_bit =  a() & 0x01;
    flags_.c = low_order_bit;

    uint8_t result = (a() >> 1) | (carry_bit << 7);

    set_reg(Reg::A, result);
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

        // XCHG - exchange DE and HL
        case 0xEB: {
            uint16_t temp = get_rp(RegPair::DE);
            set_rp(RegPair::DE, get_rp(RegPair::HL));
            set_rp(RegPair::HL, temp);
        } break;

        // INX - increment register pair
        case 0x03: set_rp(RegPair::BC, get_rp(RegPair::BC) + 1); break;
        case 0x13: set_rp(RegPair::DE, get_rp(RegPair::DE) + 1); break;
        case 0x23: set_rp(RegPair::HL, get_rp(RegPair::HL) + 1); break;
        case 0x33: set_rp(RegPair::SP, get_rp(RegPair::SP) + 1); break;

        // DCX - decrement register pair
        case 0x0B: set_rp(RegPair::BC, get_rp(RegPair::BC) - 1); break;
        case 0x1B: set_rp(RegPair::DE, get_rp(RegPair::DE) - 1); break;
        case 0x2B: set_rp(RegPair::HL, get_rp(RegPair::HL) - 1); break;
        case 0x3B: set_rp(RegPair::SP, get_rp(RegPair::SP) - 1); break;

        // INR - increment register or memory
        case 0x04: exec_inr(Reg::B); break;
        case 0x14: exec_inr(Reg::D); break;
        case 0x24: exec_inr(Reg::H); break;
        case 0x34: exec_inr(Reg::M); break;
        case 0x0C: exec_inr(Reg::C); break;
        case 0x1C: exec_inr(Reg::E); break;
        case 0x2C: exec_inr(Reg::L); break;
        case 0x3C: exec_inr(Reg::A); break;
        
        // DCR - decrement register or memory
        case 0x05: exec_dcr(Reg::B); break;
        case 0x15: exec_dcr(Reg::D); break;
        case 0x25: exec_dcr(Reg::H); break;
        case 0x35: exec_dcr(Reg::M); break;
        case 0x0D: exec_dcr(Reg::C); break;
        case 0x1D: exec_dcr(Reg::E); break;
        case 0x2D: exec_dcr(Reg::L); break;
        case 0x3D: exec_dcr(Reg::A); break;

        // Rotate Accumulator
        case 0x07: exec_rlc(); break; // RLC
        case 0x0F: exec_rrc(); break; // RRC
        case 0x17: exec_ral(); break; // RAL
        case 0x1F: exec_rar(); break; // RAR

        default: {
            // MOV opcodes
            if (opcode >= 0x40 && opcode <= 0x7F) { // 0x76, HLT already handled
                // 0x40 -> 0b001_000_000
                //   Binary op   DST SRC ; DST = B, SRC = B
                // 0x4A -> 0b001_001_010 ; DST = C, SRC = D
                uint8_t dst = (opcode >> 3) & 0x07; // maintain same register for 6 opcodes, 0x_0 to 0x_6
                uint8_t src = opcode & 0x07; // 0 to 6 registers
                set_reg(dst, get_reg(src));
            } else { 
                fmt::print(stderr, "Unimplemented opcode {:02X} at {:04X}\n", opcode, static_cast<uint16_t>(pc_ - 1));
            }
        }
    }
}


} // namespace