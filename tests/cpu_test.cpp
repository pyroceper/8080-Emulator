#include <gtest/gtest.h>

#include "i8080/cpu.h"

TEST(CPUTest, InitialStateIsZero)
{
    i8080::CPU cpu;

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