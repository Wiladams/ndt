
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

void main()
{
    //test_bitshift();
    //test_BIT64();
    test_BITMASK();
}