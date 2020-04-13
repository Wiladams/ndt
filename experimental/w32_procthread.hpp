#pragma once

#include "w32_types.hpp"

#define CREATE_SUSPENDED                  0x00000004

#ifdef __cplusplus
extern "C" {
#endif

typedef DWORD (*PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId);


DWORD SuspendThread(HANDLE hThread);
DWORD ResumeThread(HANDLE hThread);
BOOL SwitchToThread(void);
HANDLE GetCurrentThread(void);
DWORD GetCurrentThreadId(void);
void ExitThread(DWORD dwExitCode);
BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode);
BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);

#ifdef __cplusplus
}
#endif
