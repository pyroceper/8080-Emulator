#include <fmt/format.h>
#include <fstream>
#include <vector>

#include "i8080/disasm.h"

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        fmt::print(stderr, "Usage: {} <rom.bin> [start address in hex]\n", argv[0]);
        return 1;
    }

    std::ifstream rom_file(argv[1], std::ios::binary);
    if (!rom_file) {
        fmt::print(stderr, "Error: Could not open {}\n", argv[1]);
        return 1;
    }

    // read rom
    std::vector<uint8_t> mem( (std::istreambuf_iterator<char>(rom_file)) , std::istreambuf_iterator<char>());

    if (mem.empty()) {
        fmt::print(stderr, "Error: ROM file is empty!\n");
        return 1;
    }

    uint16_t start_address = 0x0000; // default start address
    if (argc >= 3) {
        // string to unsigned long int
        // https://cplusplus.com/reference/cstdlib/strtoul/
        start_address = static_cast<uint16_t>(std::strtoul(argv[2], nullptr, 16));
    }

    // takes the address, turns it into an index and returns the hex value
    auto mem_read = [&](uint16_t addr) -> uint8_t {
        uint32_t offset = static_cast<uint32_t>(addr) - start_address; // address to array index
        return (offset < mem.size()) ? mem[offset] : 0x00;
    };

    uint32_t pc = start_address;
    uint32_t end = start_address + static_cast<uint32_t>(mem.size());

    while (pc < end) {
        i8080::DecodedInstr instr = i8080::disassemble_at(static_cast<uint16_t>(pc), mem_read);

        fmt::print("{:04X}  ", pc);
        for (int i = 0; i < 3; i++) {
            if (i < instr.length) {
                fmt::print("{:02X} ", mem_read(static_cast<uint16_t>(pc + i)));
            }
            else {
                fmt::print("   "); // padding for alignment
            }
        }
        fmt::print(" {}\n", instr.text);

        pc += instr.length;
    }

    return 0;
}