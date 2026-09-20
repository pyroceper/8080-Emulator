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
    /*0x01*/ {"LXI B,{:04X}H", 3}
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