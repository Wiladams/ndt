#pragma once

#include "definitions.h"


#if BUILD_AS_DLL
  #define APP_API		__declspec(dllexport)
#else
//  #define APP_API		__declspec(dllimport)
  #define APP_API
#endif

#define APP_EXPORT		__declspec(dllexport)
//#define APP_EXPORT

#define APP_INLINE  static inline
#define APP_EXTERN  extern