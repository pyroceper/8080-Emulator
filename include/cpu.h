#ifndef I8080_CPU_H
#define I8080_CPU_H

#include <cstdint>

namespace i8080 {


class CPU 
{
    public:
        CPU();

        void reset();


        uint8_t a() const { return a_; }
        uint8_t b() const { return b_; }
        uint8_t c() const { return c_; }
        uint8_t d() const { return d_; }
        uint8_t e() const { return e_; }
        uint8_t h() const { return h_; }
        uint8_t l() const { return l_; }
        
        uint16_t sp() const { return sp_; }
        uint16_t pc() const { return pc_; }
    
    private:
        // 8bit registers
        uint8_t a_;
        uint8_t b_;
        uint8_t c_;
        uint8_t d_;
        uint8_t e_;
        uint8_t h_;
        uint8_t l_;

        // 16bit registers
        uint16_t sp_;
        uint16_t pc_;
};



}


#endif