#include <gtest/gtest.h>

#include "i8080/cpu.h"

TEST(CPUTest, InitialStateIsZero)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    EXPECT_EQ(cpu.a(), 0);
    EXPECT_EQ(cpu.b(), 0);
    EXPECT_EQ(cpu.c(), 0);
    EXPECT_EQ(cpu.d(), 0);
    EXPECT_EQ(cpu.e(), 0);
    EXPECT_EQ(cpu.h(), 0);
    EXPECT_EQ(cpu.l(), 0);

    EXPECT_EQ(cpu.pc(), 0);
    EXPECT_EQ(cpu.sp(), 0);

}

TEST(CPUTest, Fetch8Bits)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    bus.write(0x0001, 0x42);

    cpu.fetch8();

    EXPECT_EQ(cpu.pc(), 0x0001);
    EXPECT_EQ(bus.read(cpu.pc()), 0x42);
}

TEST(CPUTest, Fetch16Bits)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    bus.write(0x0000, 0x42);
    bus.write(0x0001, 0x69);

    uint16_t val = cpu.fetch16();

    EXPECT_EQ(cpu.pc(), 0x0002);
    EXPECT_EQ(val, 0x6942);
}

TEST(CPUTest, SetRegPair)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    cpu.set_rp(i8080::RegPair::BC, 0x1234);
    cpu.set_rp(i8080::RegPair::DE, 0x5678);
    cpu.set_rp(i8080::RegPair::HL, 0x1122);
    cpu.set_rp(i8080::RegPair::SP, 0x3344);

    EXPECT_EQ(cpu.b(), 0x12);
    EXPECT_EQ(cpu.c(), 0x34);

    EXPECT_EQ(cpu.d(), 0x56);
    EXPECT_EQ(cpu.e(), 0x78);

    EXPECT_EQ(cpu.h(), 0x11);
    EXPECT_EQ(cpu.l(), 0x22);

    EXPECT_EQ(cpu.sp(), 0x3344);
}

TEST(CPUTest, GetRegPair)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);
    
    cpu.set_rp(i8080::RegPair::BC, 0x1234);
    cpu.set_rp(i8080::RegPair::DE, 0x5678);
    cpu.set_rp(i8080::RegPair::HL, 0x1122);
    cpu.set_rp(i8080::RegPair::SP, 0x3344);

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::BC), 0x1234);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5678);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x1122);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::SP), 0x3344);
}


TEST(CPUTest, LoadStoreOPs)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // STAX B -> memory[BC] = A
    // BC = 0x0100
    // A = 0x42
    // memory[0x0100] = a()
    // memory[0x0100] = 0x42
    uint16_t addr = 0x0100;
    uint8_t value = 0x42;
    cpu.set_rp(i8080::RegPair::BC, addr);
    cpu.set_reg(i8080::Reg::A, value);

    bus.write(0x0000, 0x02); // STAX B opcode

    cpu.step(); // execute
    EXPECT_EQ(bus.read(addr), value); // check if address contains value
    addr = cpu.get_rp(i8080::RegPair::BC);
    EXPECT_EQ(bus.read(addr), value);

    // STAX D -> memory[DE] = A
    // DE = 0x0200
    // A = 0x69
    addr = 0x0200;
    value = 0x69;
    cpu.set_rp(i8080::RegPair::DE, addr);
    cpu.set_reg(i8080::Reg::A, value);

    bus.write(0x0001, 0x12); // STAX D opcode
    

    cpu.step(); // execute
    EXPECT_EQ(bus.read(addr), value); // check if address contains value
    addr = cpu.get_rp(i8080::RegPair::DE);
    EXPECT_EQ(bus.read(addr), value);

    // LDAX B -> A = memory[BC] 
    // BC = 0x0100 ; memory[BC] = 0x42
    // A = memory[BC]
    bus.write(0x0002, 0x0A); // LDAX B opcode
    cpu.step(); // execute
    EXPECT_EQ(cpu.a(), 0x42);

    // LDAX D -> D = memory[DE]
    // DE = 0x0200 ; memory[DE] = 0x69
    // A = memory[DE]
    bus.write(0x0003, 0x1A); // LDAX D opcode
    cpu.step(); // execute
    EXPECT_EQ(cpu.a(), 0x69);

    // SHLD a16
    // SHLD 0500H; memory[0x500] = l(); memory[0x501] = h();
    bus.write(0x0004, 0x22); // SHLD a16 opcode
    bus.write(0x0005, 0x00); // lo of a16
    bus.write(0x0006, 0x05); // hi of a16

    // L = 0x42; H = 0x69; HL = 6942H
    cpu.set_reg(i8080::Reg::L, 0x42);
    cpu.set_reg(i8080::Reg::H, 0x69);

    cpu.step(); // execute
    addr = 0x500;
    EXPECT_EQ(bus.read(addr), 0x42);
    EXPECT_EQ(bus.read(addr + 1), 0x69);
    
    // LHLD a16
    cpu.set_rp(i8080::RegPair::HL, 0x0000); // reset HL 

    bus.write(0x0007, 0x2A); // LHLD a16 opcode
    bus.write(0x0008, 0x00); // lo of a16
    bus.write(0x0009, 0x05); // hi of a16

    cpu.step(); // execute
    EXPECT_EQ(cpu.l(), 0x42);
    EXPECT_EQ(cpu.h(), 0x69);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x6942);

    // STA a16
    // A = 0x12
    // addr = 0x0256
    // memory[addr] = A
    cpu.set_reg(i8080::Reg::A, 0x12);
    
    bus.write(0x000A, 0x32); // STA a16 opcode
    bus.write(0x000B, 0x56); // lo of a16
    bus.write(0x000C, 0x02); // hi of a16

    cpu.step(); // execute
    EXPECT_EQ(bus.read(0x0256), 0x12);
    // LDA a16
    // A = memory[addr]
    cpu.set_reg(i8080::Reg::A, 0x00); // reset A

    bus.write(0x000D, 0x3A); // LDA a16 opcode
    bus.write(0x000E, 0x56); // lo of a16
    bus.write(0x000F, 0x02); // hi of a16

    cpu.step(); // execute
    EXPECT_EQ(cpu.a(), 0x12);
}

TEST(CPUTest, MVIOpcodes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    uint8_t value = 0x42;
    bus.write(0x0000, 0x06); // MVI B,d8 opcode
    bus.write(0x0001, value); // d8
    cpu.step(); // execute
    
    bus.write(0x0002, 0x16); // MVI D,d8 opcode
    bus.write(0x0003, value); // d8
    cpu.step(); // execute

    bus.write(0x0004, 0x26); // MVI H,d8 opcode
    bus.write(0x0005, value); // d8
    cpu.step(); // execute

    value = 0x69;

    bus.write(0x0006, 0x0E); // MVI C,d8 opcode
    bus.write(0x0007, value); // d8
    cpu.step(); // execute
    
    bus.write(0x0008, 0x1E); // MVI E,d8 opcode
    bus.write(0x0009, value); // d8
    cpu.step(); // execute

    bus.write(0x000A, 0x2E); // MVI L,d8 opcode
    bus.write(0x000B, value); // d8
    cpu.step(); // execute

    bus.write(0x000C, 0x3E); // MVI A,d8 opcode
    bus.write(0x000D, value); // d8
    cpu.step(); // execute

    EXPECT_EQ(cpu.c(), value);
    EXPECT_EQ(cpu.e(), value);
    EXPECT_EQ(cpu.l(), value);
    EXPECT_EQ(cpu.a(), value);
    value = 0x42;
    EXPECT_EQ(cpu.b(), value);
    EXPECT_EQ(cpu.d(), value);
    EXPECT_EQ(cpu.h(), value);    

    cpu.set_rp(i8080::RegPair::HL, 0x0256); // HL = 0x0256

    bus.write(0x000E, 0x36); // MVI M,d8 opcode
    bus.write(0x000F, value); // d8
    cpu.step(); // execute

    uint16_t addr = cpu.get_rp(i8080::RegPair::HL);
    EXPECT_EQ(bus.read(addr), value);
}

TEST(CPUTest, MOVOpcodes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);
    bus.write(0x0000, 0x00); // NOP
    cpu.step(); // execute

    uint8_t val = 0x00;
    uint16_t addr = 0x0001;
    for (uint8_t dst = 0; dst < 8; dst++) {
        for (uint8_t src = 0; src < 8; src++) {
            // 0x76 is HLT
            // there's no MOV M, M
            if (dst == i8080::Reg::M && src == i8080::Reg::M) 
                continue;

            // MOV opcode -> 01_DDD_SSS
            const uint8_t opcode = 0x40 | (dst << 3) | src;

            cpu.set_reg(src, val); // set src to specific val
            bus.write(addr, opcode); // MOV dst,src opcode
            addr++;

            cpu.step(); // execute
            EXPECT_EQ(cpu.get_reg(dst), val);
            val++;
        }
    }

    // XCHG - DE and HL
    cpu.set_rp(i8080::RegPair::DE, 0x1234);
    cpu.set_rp(i8080::RegPair::HL, 0x5678);

    bus.write(addr, 0xEB); // XCHG opcode
    cpu.step(); // execute
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5678);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x1234);
}

TEST(CPUTest, INXAndDCXOpcodes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus); 

    bus.write(0x0000, 0x03); // INX B
    cpu.set_rp(i8080::RegPair::BC, 0x1233);
    cpu.step(); // execute

    bus.write(0x0001, 0x13); // INX D
    cpu.set_rp(i8080::RegPair::DE, 0x5677);
    cpu.step(); // execute

    bus.write(0x0002, 0x23); // INX H
    cpu.set_rp(i8080::RegPair::HL, 0x1121);
    cpu.step(); // execute

    bus.write(0x0003, 0x33); // INX SP
    cpu.set_rp(i8080::RegPair::SP, 0x3343);
    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::BC), 0x1234);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5678);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x1122);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::SP), 0x3344);    

    bus.write(0x0004, 0x0B); // DCX B
    cpu.step(); // execute

    bus.write(0x0005, 0x1B); // DCX D
    cpu.step(); // execute

    bus.write(0x0006, 0x2B); // DCX H
    cpu.step(); // execute

    bus.write(0x0007, 0x3B); // DCX SP
    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::BC), 0x1233);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5677);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x1121);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::SP), 0x3343); 
    
    // overflow and underflow tests
    cpu.set_rp(i8080::RegPair::DE, 0xFFFF);
    bus.write(0x0008, 0x13); // INX D
    cpu.step(); // execute

    cpu.set_rp(i8080::RegPair::HL, 0x0000);
    bus.write(0x0009, 0x2B); // DCX H
    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x0000);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0xFFFF);
}

TEST(CPUTest, ZSP)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);    

    cpu.set_zsp(0x80); // 0b1000_0000
    EXPECT_FALSE(cpu.zero());
    EXPECT_TRUE(cpu.sign());
    EXPECT_FALSE(cpu.parity());
    // TODO: add more flag tests
    // for .ac, wrap, sign boundary
}

TEST(CPUTest, INRAndDCROpcodes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus); 
    
    cpu.set_rp(i8080::RegPair::BC, 0x1234);
    cpu.set_rp(i8080::RegPair::DE, 0x5678);
    cpu.set_rp(i8080::RegPair::HL, 0x9ABC);
    cpu.set_reg(i8080::Reg::A, 0x42);

    bus.write(0x0000, 0x04); // INR B
    cpu.step(); // execute

    bus.write(0x0001, 0x14); // INR D
    cpu.step(); // execute

    bus.write(0x0002, 0x24); // INR H
    cpu.step(); // execute

    bus.write(0x0003, 0x0C); // INR C
    cpu.step(); // execute

    bus.write(0x0004, 0x1C); // INR E
    cpu.step(); // execute

    bus.write(0x0005, 0x2C); // INR L
    cpu.step(); // execute

    bus.write(0x0006, 0x3C); // INR A
    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::BC), 0x1335);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5779);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x9BBD);
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x43);
    
    // check flags for 0x43 -> 0b0100_0011
    EXPECT_FALSE(cpu.zero());
    EXPECT_FALSE(cpu.sign());
    EXPECT_FALSE(cpu.parity());

    bus.write(0x0007, 0x05); // DCR B
    cpu.step(); // execute

    bus.write(0x0008, 0x15); // DCR D
    cpu.step(); // execute

    bus.write(0x0009, 0x25); // DCR H
    cpu.step(); // execute

    bus.write(0x000A, 0x0D); // DCR C
    cpu.step(); // execute

    bus.write(0x000B, 0x1D); // DCR E
    cpu.step(); // execute

    bus.write(0x000C, 0x2D); // DCR L
    cpu.step(); // execute

    bus.write(0x000D, 0x3D); // DCR A
    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::BC), 0x1234);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::DE), 0x5678);
    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x9ABC);
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x42);

    uint16_t addr = 0x0100;
    uint8_t value = 0x69;
    cpu.set_rp(i8080::RegPair::HL, addr);
    bus.write(addr, value);

    bus.write(0x000E, 0x34); // INR M; M -> (HL)
    cpu.step(); // execute

    EXPECT_EQ(bus.read(addr), 0x6A);

    bus.write(0x000F, 0x35); // DCR M; M -> (HL)
    cpu.step(); // execute

    EXPECT_EQ(bus.read(addr), 0x69);
}

TEST(CPUTest, RLCOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // RLC - rotate A left
    cpu.set_reg(i8080::Reg::A, 0x80); // 0x80 -> 0b10000000
    cpu.exec_rlc();
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x01);
    EXPECT_TRUE(cpu.carry());
}

TEST(CPUTest, RRCOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // RLC - rotate A right
    cpu.set_reg(i8080::Reg::A, 0x01); 
    cpu.exec_rrc();
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x80); // 0x80 -> 0b10000000
    EXPECT_TRUE(cpu.carry());
}

TEST(CPUTest, RALOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // RAL - rotate A left
    cpu.set_reg(i8080::Reg::A, 0x80); // 0x80 -> 0b10000000
    cpu.set_c(true);
    cpu.exec_ral();
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x01); 
    EXPECT_TRUE(cpu.carry());
}

TEST(CPUTest, RAROpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // RAR - rotate A right
    cpu.set_reg(i8080::Reg::A, 0x01); 
    cpu.set_c(true);
    cpu.exec_rar();
    EXPECT_EQ(cpu.get_reg(i8080::Reg::A), 0x80); // 0x80 -> 0b10000000
    EXPECT_TRUE(cpu.carry());
}

TEST(CPUTest, DAAOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // A = 0x12
    // ac = 1; c = 0; expected A = 0x18; expected c = 0
    cpu.set_reg(i8080::Reg::A, 0x12);
    cpu.set_aux_c(1);
    cpu.set_c(0);

    bus.write(0x0000, 0x27); // DAA

    cpu.step(); // execute

    EXPECT_EQ(cpu.a(), 0x18);
    EXPECT_EQ(cpu.carry(), 0);

    // A = 0xCE
    // ac = 0; c = 0; expected A = 0x34; expected c = 1
    cpu.set_reg(i8080::Reg::A, 0xCE);
    cpu.set_aux_c(0);
    cpu.set_c(0);

    bus.write(0x0001, 0x27); // DAA

    cpu.step(); // execute

    EXPECT_EQ(cpu.a(), 0x34);
    EXPECT_EQ(cpu.carry(), 1);
}

TEST(CPUTest, CMAOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // A = 0x00; expected A = 0xFF
    cpu.set_reg(i8080::Reg::A, 0x00);

    bus.write(0x0000, 0x2F); // CMA

    cpu.step(); // execute

    EXPECT_EQ(cpu.a(), 0xFF);

    // A = 0xFF; expected A = 0x00
    bus.write(0x0001, 0x2F); // CMA

    cpu.step(); // execute

    EXPECT_EQ(cpu.a(), 0x00);

    // A = 0xA5; expected A = 0x5A
    cpu.set_reg(i8080::Reg::A, 0xA5);

    bus.write(0x0002, 0x2F); // CMA

    cpu.step(); // execute

    EXPECT_EQ(cpu.a(), 0x5A);
}

TEST(CPUTest, STCAndCMCOpcodes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);
    
    bus.write(0x0000, 0x37); // STC
    bus.write(0x0001, 0x3F); // CMC
    bus.write(0x0002, 0x3F); // CMC

    cpu.step(); // execute
    EXPECT_TRUE(cpu.carry());

    cpu.step(); // execute
    EXPECT_FALSE(cpu.carry());

    cpu.step(); // execute
    EXPECT_TRUE(cpu.carry());
}

TEST(CPUTest, DADOpcode)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    // BC = 339F; HL = A17B; BC + HL = D51A
    cpu.set_rp(i8080::RegPair::BC, 0x339F);
    cpu.set_rp(i8080::RegPair::HL, 0xA17B);

    bus.write(0x0000, 0x09); // DAD B

    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0xD51A);
    EXPECT_FALSE(cpu.carry());

    // DE = FF00; HL = 1000; DE + HL = 0x10F00
    cpu.set_rp(i8080::RegPair::DE, 0xFF00);
    cpu.set_rp(i8080::RegPair::HL, 0x1000);

    bus.write(0x0001, 0x19); // DAD D

    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x0F00);
    EXPECT_TRUE(cpu.carry());


    // HL = 1234; HL + HL = 2468
    cpu.set_rp(i8080::RegPair::HL, 0x1234);

    bus.write(0x0002, 0x29); // DAD H

    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x2468);


    // SP = 0100; HL = 1000; SP + HL = 1100
    cpu.set_rp(i8080::RegPair::SP, 0x0100);
    cpu.set_rp(i8080::RegPair::HL, 0x1000);

    bus.write(0x0003, 0x39); // DAD SP

    cpu.step(); // execute

    EXPECT_EQ(cpu.get_rp(i8080::RegPair::HL), 0x1100);
}