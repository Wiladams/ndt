#pragma once

#include <stdint.h>
#include <math.h>

// Return a value representing the single bit being set
#define  BIT(bitnum) (1<<(bitnum))

// return true if the specified bit is set in the value
inline bool isset(const uint64_t value, const size_t bit) {return (value & BIT(bit)) > 0; }

// set a specific bit within a value
inline uint64_t setbit(uint64_t value, size_t bit) {return (value | BIT(bit));}

// This will work for any number of bits from 32 to 63
// create a bit mask based on the low bit to high bit inclusive
// This is useful when you're trying to get a certain value
// out of an integer of some sort
inline uint64_t BITMASK(size_t lowbit, size_t highbit)
{
    uint64_t mask = 0ULL;

    for (int i=lowbit; i <= highbit; i++) {
        mask = mask | BIT(i);
    }

    return mask;
}

// retrieve an integer value from a bitmask on a value
inline  uint64_t BITSVALUE(uint64_t src, size_t lowbit, size_t highbit)
{
    return ((src & BITMASK(lowbit, highbit)) >> lowbit);
}

// Determine at runtime if the CPU is little-endian (intel standard)
inline bool isLE () {
    int i=1;
    return (int)*((unsigned char *)&i)==1;
}

inline bool isBE() {return !isLE();}


// swap 2 bytes (16-bit) around
inline uint16_t swapUInt16(const uint16_t num)
{
    return (((num & 0x00ff) << 8) | ((num & 0xff00) >> 8));
}

// swap 4 bytes (32-bit) around
inline uint32_t swapUInt32(const uint32_t num)
{
    uint32_t x = (num & 0x0000FFFF) << 16 | (num & 0xFFFF0000) >> 16;
    x = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;

    return x;
}

// swap 8 bytes (64-bit) around
inline uint64_t swapUInt64(const uint64_t num)
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