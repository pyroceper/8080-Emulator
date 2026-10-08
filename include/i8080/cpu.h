#ifndef I8080_CPU_H
#define I8080_CPU_H

#include <array>
#include <bit> // std::popcount
#include "reg.h"
#include "flags.h"
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

        void set_reg(uint8_t reg, uint8_t value);
        uint8_t get_reg(uint8_t reg) const;

        bool halted() const { return halted_; }

        uint16_t get_rp(uint8_t rp) const;
        void set_rp(uint8_t rp, uint16_t value);

        bool zero() const { return flags_.z; }
        bool sign() const { return flags_.s; }
        bool parity() const { return flags_.p; }
        bool carry() const { return flags_.c; }
        
        void set_zsp(uint8_t result);
        void set_c(bool carry);

        void exec_shld();
        void exec_lhld();

        void exec_sta();
        void exec_lda();

        void exec_xchg();

        void exec_inr(uint8_t reg);
        void exec_dcr(uint8_t reg);

        void exec_rlc();
        void exec_rrc();
        void exec_ral();
        void exec_rar();

        uint8_t fetch8();
        uint16_t fetch16();

        void step();
    
    private:
        // 8bit registers
        std::array<uint8_t, 8> regs_{};

        // 16bit registers
        uint16_t sp_;
        uint16_t pc_;

        // Flags
        Flags flags_;

        // bus
        Bus& bus_;

        bool halted_; // halt cpu

};



}


#endif