#ifndef I8080_DISASM_H
#define I8080_DISASM_H

#include <cstdint>
#include <string>
#include <functional>

namespace i8080 {


struct DecodedInstr
{
    std::string text;
    uint8_t length; // instrcution length
};    

DecodedInstr disassemble_at(uint16_t address, const std::function<uint8_t(uint16_t)>& mem_read);


} // namespace

#endif