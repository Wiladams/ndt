
#include "bitbang.hpp"

#include <cstdio>

using namespace bitbang;

void test_getset()
{
    for (unsigned int i = 0; i<64; i++) {
        //printf("ABIT[%2zd]: 0x%016llx\n", i, BIT(i));
        printf("BIT[%2d]: o%llo\n", i, BIT(i));
    }
}

void main()
{
    test_getset();
}