#include "binops.hpp"

#include <cstdio>

void printx32(uint32_t x)
{
    char buff[9];
    if (!bops::tohex32(x, buff, 9))
        return;

    buff[8]=0;
    printf("0x%s", buff);
}

void test_tohex32()
{
    printf("==== test_tohex32 ====\n");
    
    uint32_t nums[] = {
        0xe7,
        0xfeedface,
    };

    int numnums = sizeof(nums)/sizeof(uint32_t);

    for (int i=0;i<numnums; i++){
        printf("tonex32(%d): ", nums[i]);
        printx32(nums[i]);
        printf("\n");
    }
}

void test_tobin32()
{
    printf("==== test_tobin32 ====\n");
    char buff[33];

    for (int i=0;i<256;i++) {
        if (!bops::tobin32(i, buff, 33)) {
            printf("tobin32 failed\n");
            continue;
        }

        printf("tobin32(%3d): %s\n", i, buff);
    }
}

void test_arshift16()
{
    printf("arshift(256, 8): %d\n", bops::arshift16(256,8));
    printf("arshift(-256, 8): %d\n", bops::arshift16(-256,8));
}

void test_bswap32()
{
    printf("bswap32(0xaabbccdd): %lx\n", bops::bswap32(0xaabbccdd));
}

void main()
{
    //test_tohex32();
    //test_tobin32();
    //test_arshift16();
    test_bswap32();
}