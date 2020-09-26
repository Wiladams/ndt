/*
    cl /EHsc /I..\primary bbang.cpp
*/

#include <cstdio>
#include <iostream>
#include <string>

#include "bitbang.h"

using namespace ndt;
using std::cout;

void bit_tests()
{
    cout << "bit_tests" << std::endl;
    cout << "BIT8(0): " << std::to_string(BIT8(0)) << std::endl;
    cout << "BIT8(1): " << std::to_string(BIT8(1)) << std::endl;
    cout << "BIT8(2): " << std::to_string(BIT8(2)) << std::endl;
}

void test_bitmask()
{
    cout << "== test_bitmask ==" << std::endl;
    cout << "BITMASK32(0,1): " << std::to_string(BITMASK32(0,1)) << std::endl;
    cout << "BITMASK32(1,1): " << std::to_string(BITMASK32(1,1)) << std::endl;
}

void test_endian()
{
    cout << "== test_endian ==" << std::endl;
    cout << "isLE(): " << isLE () << std::endl;
    cout << "isBE(): " << isBE () << std::endl;
}

void main()
{
    printf("HELLO\n");
    test_endian();
    //bit_tests();
    test_bitmask();
}