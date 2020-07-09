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
#include <stdint.h>


#if defined(_WIN32) || defined(_WIN64)
#define NDT_IS_WINDOWS
#endif

#if defined(_MSC_VER)
#define NDT_IS_MSVC
#endif
