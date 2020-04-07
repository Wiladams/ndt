#pragma once

// synchapi.h
#include "w32_types.hpp"

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, const char * lpName);

HANDLE OpenEventA(DWORD dwDesiredAccess, BOOL bInheritHandle, const char * lpName);

BOOL ResetEvent(HANDLE hEvent);

BOOL SetEvent(HANDLE hEvent);

DWORD SleepEx(DWORD dwMilliseconds, BOOL bAlertable);

DWORD WaitForSingleObjectEx(
     HANDLE hHandle,
     DWORD dwMilliseconds,
     BOOL bAlertable);

DWORD WaitForMultipleObjectsEx(
    DWORD nCount,
    const HANDLE* lpHandles,
    BOOL bWaitAll,
    DWORD dwMilliseconds,
    BOOL bAlertable);

#ifdef __cplusplus
} // extern "C"
#endif  /* __cplusplus */
