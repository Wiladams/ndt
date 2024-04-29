#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1


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

#include <windows.h>
#include <atomic>

#include <cstdint>		// uint8_t, etc
#include <cstddef>		// nullptr_t, ptrdiff_t, size_t


#ifdef WHISPER_SHARED
#    ifdef _WIN32
#        ifdef APP_BUILD
#            define APP_API __declspec(dllexport)
#        else
#            define APP_API __declspec(dllimport)
#        endif
#    else
#        define APP_API __attribute__ ((visibility ("default")))
#    endif
#else
#    define APP_API
#endif


#if defined(_MSC_VER)
#define NDT_IS_MSVC
#endif

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE static inline
#endif

namespace ndt 
{
    // Determine at runtime if the CPU is little-endian (intel standard)
    static INLINE bool isLE() noexcept {int i = 1; return (int)*((unsigned char*)&i) == 1;}
    static INLINE bool isBE() noexcept { return !isLE(); }
}

