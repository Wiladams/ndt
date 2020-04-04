#pragma once

// Herein lie definitions of some things that are low level enough 
// to be universal
// Don't put Windows specifics in here. Put that stuff in the w32_***
// files.
//
// Put things like universal typedefs and the like in here
//
// If this file hasn't already been included, you should include
// it as the first item to be included.
//


#include <intrin.h>
#include <cstdint>


#define U64x(hi, lo)	(((uint64_t)0x##hi << 32) + (uint64_t)0x##lo)
