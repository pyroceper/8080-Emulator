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

TEST(CPUTest, Fetch8Bytes)
{
    i8080::Bus bus;
    i8080::CPU cpu(bus);

    bus.write(0x0001, 0x42);

    cpu.fetch8();

    EXPECT_EQ(cpu.pc(), 0x0001);
    EXPECT_EQ(bus.read(cpu.pc()), 0x42);
}

TEST(CPUTest, Fetch16Bytes)
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