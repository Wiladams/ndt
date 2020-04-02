
#include "bitbang.hpp"

#include <cstdio>

void test_getset()
{
    for (unsigned __int64 i = 0; i<64; i++) {
        //printf("ABIT[%2zd]: 0x%016llx\n", i, ABIT(i));
        printf("ABIT[%2zd]: o%llo\n", i, ABIT(i));
    }
}

void main()
{
    test_getset();
}