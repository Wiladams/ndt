#pragma once

#include "definitions.h"

//#include <math.h>

//
// Some of the routines here are inspired by the bit twiddling hacks
// http://graphics.stanford.edu/~seander/bithacks.html
//

namespace binops {

// Return various forms of pow(2,bitnum)
// There are different ones, which allow the user to specify how
// many bits they want
static INLINE uint8_t BIT8(size_t bitnum) noexcept {return (uint8_t)1 << bitnum; }
static INLINE uint16_t BIT16(size_t bitnum) noexcept {return (uint16_t)1 << bitnum; }
static INLINE uint32_t BIT32(size_t bitnum) noexcept {return (uint32_t)1 << bitnum; }
static INLINE uint64_t BIT64(size_t bitnum) noexcept {return (uint64_t)1 << bitnum; }

// One general purpose which will default to BIT64
//static inline uint64_t BIT(unsigned int bitnum) {return BIT64(bitnum);}

// return true if the specified bit is set in the value
static INLINE bool isset(const uint64_t value, const size_t bitnum) noexcept {return (value & BIT64(bitnum)) > 0; }

// set a specific bit within a value
static INLINE uint64_t setbit(const uint64_t value, const size_t bitnum) noexcept {return (value | BIT64(bitnum));}

// BITMASK64
// A bitmask is an integer where all the bits from the 
// specified low value to the specified high value
// are set to 1.
// The trick used in BITMASK64 is to set a single bit
// above the required range, then subtracting 1 from that
// value.  By subtracting 1, we get all 1 bits below the
// single bit value.
// This set of 1 bits are then shifted upward by the number
// of the low bit, and we have our mask.
// Discussion: 
//   https://stackoverflow.com/questions/28035794/masking-bits-within-a-range-given-in-parameter-in-c
//
//  uint64_t mask = (uint64_t)1 << (high-low);
//  mask <<= 1;
//  mask--;         // turn on all the bits below
//  mask <<= low;   // shift up to proper position
//  return mask;

static INLINE uint64_t BITMASK64(const size_t low, const size_t high) noexcept {return ((((uint64_t)1ULL << (high-low)) << 1) - 1) << low;}

static INLINE uint8_t BITMASK8(const size_t low, const size_t high) noexcept {return (uint8_t)BITMASK64(low, high);}
static INLINE uint16_t BITMASK16(const size_t low, const size_t high) noexcept {return (uint16_t)BITMASK64(low,high);}
static INLINE uint32_t BITMASK32(const size_t low, const size_t high) noexcept {return (uint32_t)BITMASK64(low, high);}

//#define BITMASK BITMASK64


// BITSVALUE
// Retrieve a value from a lowbit highbit pair
static INLINE  uint64_t BITSVALUE(uint64_t src, size_t lowbit, size_t highbit) noexcept
{
    return ((src & BITMASK64(lowbit, highbit)) >> lowbit);
}

// Given a bit number, calculate which byte
// it would be in, and which bit within that
// byte.
static INLINE void getbitbyteoffset(size_t bitnumber, size_t &byteoffset, size_t &bitoffset) noexcept
{
    byteoffset = (int)(bitnumber / 8);
    bitoffset = bitnumber % 8;
}


static INLINE uint64_t bitsValueFromBytes(const uint8_t *bytes, const size_t startbit, const size_t bitcount, bool bigendian = false) noexcept
{
    // Sanity check
    if (nullptr == bytes)
        return 0;

    uint64_t value = 0;

    if (bigendian) {
        for (int i=(int)bitcount; i>= 0; i--) {
            size_t byteoffset=0;
            size_t bitoffset=0;
            getbitbyteoffset(startbit+i, byteoffset, bitoffset);
            bool bitval = isset(bytes[byteoffset], bitoffset);

		    if (bitval) {
			    value = setbit(value, i);
            }
        }
    } else {
        for (size_t i=0; i<bitcount; i++) {
            size_t byteoffset=0;
            size_t bitoffset=0;
            getbitbyteoffset(startbit+i, byteoffset, bitoffset);
            bool bitval = isset(bytes[byteoffset], bitoffset);

            if (bitval) {
                value = setbit(value, i);
            }
        }
    }

    return value;
}


// swap 2 bytes (16-bit) around
static INLINE uint16_t swapUInt16(const uint16_t num) noexcept
{
    return (((num & 0x00ff) << 8) | ((num & 0xff00) >> 8));
}

// swap 4 bytes (32-bit) around
static INLINE uint32_t swapUInt32(const uint32_t num) noexcept
{
    uint32_t x = (num & 0x0000FFFF) << 16 | (num & 0xFFFF0000) >> 16;
    x = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;

    return x;
}

// swap 8 bytes (64-bit) around
static INLINE uint64_t swapUInt64(const uint64_t num) noexcept
{
    return  (num >> 56) |
          ((num<<40) & 0x00FF000000000000) |
          ((num<<24) & 0x0000FF0000000000) |
          ((num<<8) & 0x000000FF00000000) |
          ((num>>8) & 0x00000000FF000000) |
          ((num>>24) & 0x0000000000FF0000) |
          ((num>>40) & 0x000000000000FF00) |
          (num << 56);
}

static INLINE int GetAlignedByteCount(const int width, const int bitsperpixel, const int alignment) noexcept
{
    return (((width * (bitsperpixel / 8)) + (alignment - 1)) & ~(alignment - 1));
}

// Convert a fixed point number into a floating point number
// the fixed number can be up to 64-bits in size
// the 'scale' says where the decimal point is, starting from 
// the least significant bit
// so; 0x13 (0b0001.0011) ,4  == 1.1875
static INLINE double fixedToFloat(const uint64_t vint, const int scale) noexcept
{
    double whole = (double)binops::BITSVALUE(vint, scale, 63);
    double frac = (double)binops::BITSVALUE(vint, 0, ((size_t)scale - 1));

    return (whole + (frac / ((uint64_t)1 << scale)));
}

}; // namespace binops