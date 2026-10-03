#include <fmt/format.h>

#include "i8080/bus.h"
#include "i8080/cpu.h"

int main(int argc, char *argv[]) 
{
    fmt::print("[ DEBUG ] Space Invaders emulator\n");

    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // TEMP TEST
    // NOP
    // HLT
    // NOP
    bus.write(0x0000, 0x00);
    bus.write(0x0001, 0x76);
    bus.write(0x0002, 0x00);

    while (!cpu.halted()) {
        cpu.step();
    }

    fmt::print("[ DEBUG ] Execution complete\n");

    return 0;
}