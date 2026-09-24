#include <fmt/format.h>

#include "i8080/disasm.h"

namespace i8080 {

struct OpInfo 
{
    const char* mnemonic;
    uint8_t length; 
};


// opcode list
static const OpInfo kOpTable[256] = {
    /*0x00*/ {"NOP", 1},
    /*0x01*/ {"LXI B,{:04X}H", 3},
    /*0x02*/ {"STAX B", 1},
    /*0x03*/ {"INX B", 1},
    /*0x04*/ {"INR B", 1},
    /*0x05*/ {"DCR B", 1},
    /*0x06*/ {"MVI B,{:02X}H", 2},
    /*0x07*/ {"RLC", 1},
    /*0x08*/ {"NOP?", 1}, // undocumented?
    /*0x09*/ {"DAD B", 1},
    /*0x0A*/ {"LDAX B", 1},
    /*0x0B*/ {"DCX B", 1},
    /*0x0C*/ {"INR C", 1},
    /*0x0D*/ {"DCR C", 1},
    /*0x0E*/ {"MVI C,{:02X}H", 2},
    /*0x0F*/ {"RRC", 1},
    
    /*0x10*/ {"NOP?", 1},
    /*0x11*/ {"LXI D,{:04X}H", 3},
    /*0x12*/ {"STAX D",1},
    /*0x13*/ {"INX D", 1},
    /*0x14*/ {"INR D", 1},
    /*0x15*/ {"DCR D", 1},
    /*0x16*/ {"MVI D,{:02X}H", 2},
    /*0x17*/ {"RAL", 1},
    /*0x18*/ {"NOP?", 1},
    /*0x19*/ {"DAD D", 1},
    /*0x1A*/ {"LDAX D", 1},
    /*0x1B*/ {"DCX D", 1},
    /*0x1C*/ {"INR E", 1},
    /*0x1D*/ {"DCR E", 1},
    /*0x1E*/ {"MVI E,{:02X}H", 2},
    /*0x1F*/ {"RAR", 1},

    /*0x20*/ {"NOP?", 1}, 
    /*0x21*/ {"LXI H,{:04X}H", 3},
    /*0x22*/ {"SHLD {:04X}H", 3},
    /*0x23*/ {"INX H", 1},
    /*0x24*/ {"INR H", 1},
    /*0x25*/ {"DCR H", 1},
    /*0x26*/ {"MVI H,{:02X}H", 2},
    /*0x27*/ {"DAA", 1},
    /*0x28*/ {"NOP?", 1},
    /*0x29*/ {"DAD H", 1},
    /*0x2A*/ {"LHLD {:04X}H", 3},
    /*0x2B*/ {"DCX H", 1},
    /*0x2C*/ {"INR L", 1},
    /*0x2D*/ {"DCR L", 1},
    /*0x2E*/ {"MVI L,{:02X}H", 2},
    /*0x2F*/ {"CMA", 1},

    /*0x30*/ {"NOP?", 1},
    /*0x31*/ {"LXI SP,{:04X}H", 3},
    /*0x32*/ {"STA {:04X}H", 3},
    /*0x33*/ {"INX SP", 1},
    /*0x34*/ {"INR M", 1},
    /*0x35*/ {"DCR M", 1},
    /*0x36*/ {"MVI M,{:02X}H", 2},
    /*0x37*/ {"STC", 1},
    /*0x38*/ {"NOP?", 1},
    /*0x39*/ {"DAD SP", 1},
    /*0x3A*/ {"LDA {:04X}H", 3},
    /*0x3B*/ {"DCX SP", 1},
    /*0x3C*/ {"INR A"},
    /*0x3D*/ {"DCR A"},
    /*0x3E*/ {"MVI A,{:02X}H", 2},
    /*0x3F*/ {"CMC", 1}
};

DecodedInstr disassemble_at(uint16_t address, const std::function<uint8_t(uint16_t)>& mem_read)
{
    uint8_t op = mem_read(address);
    const OpInfo& info = kOpTable[op];

    // temp
    if (info.mnemonic == nullptr) {
        return { "???", 1};
    }

    char buf[32];

    if (info.length == 1) {
        return { info.mnemonic, 1};
        
    } else if (info.length == 2) {
        uint8_t d8 = mem_read(address + 1);
        std::string text = fmt::format(fmt::runtime(info.mnemonic), d8);
        return { text, 2};

    } else { // length == 3
        uint16_t d16 = mem_read(address + 1) | (mem_read(address + 2) << 8);
        std::string text = fmt::format(fmt::runtime(info.mnemonic), d16);
        return { text, 3};
    }
}


} // namespace