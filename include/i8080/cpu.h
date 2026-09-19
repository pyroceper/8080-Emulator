#ifndef I8080_CPU_H
#define I8080_CPU_H

#include <array>
#include "reg.h"
#include "bus.h"

namespace i8080 {


class CPU 
{
    public:
        explicit CPU(Bus& bus);

        void reset();

        uint8_t a() const { return regs_[Reg::A]; }
        uint8_t b() const { return regs_[Reg::B]; }
        uint8_t c() const { return regs_[Reg::C]; }
        uint8_t d() const { return regs_[Reg::D]; }
        uint8_t e() const { return regs_[Reg::E]; }
        uint8_t h() const { return regs_[Reg::H]; }
        uint8_t l() const { return regs_[Reg::L]; }
        
        uint16_t sp() const { return sp_; }
        uint16_t pc() const { return pc_; }
    
    private:
        // 8bit registers
        std::array<uint8_t, 8> regs_{};

        // 16bit registers
        uint16_t sp_;
        uint16_t pc_;

        // bus
        Bus& bus_;

};



}


#endif