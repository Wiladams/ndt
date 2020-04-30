
#include "bitbang.hpp"

#include <cstdio>

using namespace bitbang;

void test_BITMASK()
{
    printf("==== test_BITMASK ====\n");
    printf("BITMASK64(0,2): 0x%llx\n", BITMASK64(0,2));
    printf("BITMASK32(0,31): 0x%lx\n", BITMASK32(0,31));
    printf("BITMASK32(0,15): 0x%08lx\n", BITMASK32(0,15));
    printf("BITMASK32(16,31): 0x%lx\n", BITMASK32(16,31));
    printf("BITMASK16(0,15): 0x%04x\n", BITMASK16(0,15));
    printf("BITMASK16(8,15): 0x%04x\n", BITMASK16(8,15));
    printf("BITMASK16(0,7): 0x%04x\n", BITMASK16(0,7));
}

void test_BIT64()
{
    for (unsigned int i = 0; i<64; i++) {
        //printf("ABIT[%2zd]: 0x%016llx\n", i, BIT(i));
        printf("BIT64[%2d]: o%llo\n", i, BIT64(i));
    }
}

void test_bitshift()
{
    printf("1<<31 : 0x%lx\n", ((uint32_t)1) << 31);
    printf("(1<<31) << 1 : 0x%lx\n", (((uint32_t)1) << 31) << 1);
    printf("((1<<31) << 1) -1 : 0x%lx\n", ((((uint32_t)1) << 31) << 1)-1);

}

void test_bitbytes()
{
    uint8_t bytes[] = {0xE5,0x9d,0x03,0x04};

    printf("0,3[%x]: %Ix\n", bytes[0], bitsValueFromBytes(bytes, 0, 3));
    printf("3,13[%x]: %Ix\n", bytes[1], bitsValueFromBytes(bytes, 3, 13));
    printf("16,8[%x]: %Ix\n", bytes[2], bitsValueFromBytes(bytes, 16, 8));
    printf("24,8[%x]: %Ix\n", bytes[3], bitsValueFromBytes(bytes, 24, 8));
}

void main()
{
    //test_bitshift();
    //test_BIT64();
    //test_BITMASK();
    test_bitbytes();
}