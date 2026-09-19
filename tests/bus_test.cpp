#include <gtest/gtest.h>

#include "i8080/bus.h"

TEST(BusTest, WriteAndReadByte)
{
    i8080::Bus bus;

    bus.write(0x1234, 0x42);

    EXPECT_EQ(bus.read(0x1234), 0x42);
}

TEST(BusTest, AddressSpaceInitiallyZero)
{
    i8080::Bus bus;

    EXPECT_EQ(bus.read(0x0000), 0);
    EXPECT_EQ(bus.read(0x1234), 0);
    EXPECT_EQ(bus.read(0xFFFF), 0);
}

TEST(BusTest, ReadWriteAddressBoundaries)
{
    i8080::Bus bus;

    bus.write(0x0000, 0x42);
    bus.write(0xFFFF, 0x69);

    EXPECT_EQ(bus.read(0x0000), 0x42);
    EXPECT_EQ(bus.read(0xFFFF), 0x69);
}

