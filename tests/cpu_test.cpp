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