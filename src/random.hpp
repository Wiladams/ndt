#pragma once

/*
    Implementing a Tausworthe Random number generator
https://homes.luddy.indiana.edu/kapadia/project2/node9.html

    This is the same style of generator that 
    LuaJIT employs, and in fact this code was
    heavily borrowed from LuaJIT.
    
    NOTES FROM LUAJIT IMPLEMENTATION
    This implements a Tausworthe PRNG with period 2^223. Based on:
    Tables of maximally-equidistributed combined LFSR generators,
    Pierre L'Ecuyer, 1991, table 3, 1st entry.
    Full-period ME-CF generator with L=64, J=4, k=223, N1=49.


    If you want to play with other intervals and whatnot
    then some reading of the original tables is required
    
    Here is an implementation of the
    same using an object encapsulation.
*/

#include <cstdint>
#include <cmath>

#include "definitions.hpp"

// union used for getting bit patterns
// for doubles
typedef union {
    uint64_t u64;
    double d;
} U64double;

struct TausPRNG {
private:
    uint64_t gen[4];    // state of 4 generators
    int valid;          // State is valid

private:
    #define TW223_GEN(i, k, q, s) \
    z = this->gen[i]; \
    z = (((z<<q)^z) >> (k-s)) ^ ((z&((uint64_t)(int64_t)-1 << (64-k)))<<s); \
    r ^= z; this->gen[i] = z;

    uint64_t step()
    {
        uint64_t z, r = 0;
        TW223_GEN(0, 63, 31, 18)
        TW223_GEN(1, 58, 19, 28)
        TW223_GEN(2, 55, 24, 7)
        TW223_GEN(3, 47, 21, 8)

        return (r & U64x(000fffff,ffffffff)) | U64x(3ff00000,00000000);
    }


public:
    // Constructor with a seed
    TausPRNG(double d)
    {
        uint32_t r = 0x11090601;  // 64-k[i] as four 8 bit constants.
        int i;
        for (i = 0; i < 4; i++) {
            U64double u;
            uint32_t m = 1u << (r&255);
            r >>= 8;
            u.d = d = d * 3.14159265358979323846 + 2.7182818284590452354;
            if (u.u64 < m) // Ensure k[i] MSB of gen[i] are non-zero.
                u.u64 += m;  
            
            gen[i] = u.u64;
        }
        valid = 1;
        for (i = 0; i < 10; i++)
            step();
    }

    // Return a double value between 0..1, inclusive
    double next()
    {
        U64double u;
        double d;
        u.u64 = step();
        d = u.d - 1.0;

        return d;
    }

    // return an integer in the range [1..high]
    int next(int high)
    {
        double d = next();
        d = floor(d*high) + 1.0;
        return (int)d;
    }

    // return a value in the range
    // between low and high, inclusive
    double next(double low, double high)
    {
        double d = next();
        d = (d*(high-low+1.0)) + low;
        return d;
    }
};




