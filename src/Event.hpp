#include "w32_event.hpp"
#include "w32_funcs.hpp"
#include "w32_const.hpp"

#include <cstdio>

class Event {
    HANDLE fHandle;    // Handle of event
    bool fOwnsHandle;
    int fLastError;

    Event() = delete;
    Event(const Event &) = delete;

public:
    Event(bool initiallySet, bool resetManually, const char *lpName = nullptr) 
        : fLastError(0)
    {
        LPSECURITY_ATTRIBUTES lpEventAttributes = nullptr;  // default security attributes
        BOOL bManualReset = resetManually ? 1 : 0;
        BOOL bInitialState = initiallySet ? 1 : 0;

        fHandle = ::CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);

        if (nullptr == fHandle)
        {
            fLastError = GetLastError();
        }
    }

    ~Event()
    {
        CloseHandle(fHandle);
    }

    bool isValid() const {return nullptr != fHandle;}
    int getLastError() const
    {
        return fLastError;
    }

    bool set()
    {
        BOOL bResult = ::SetEvent(fHandle);
        if (0 == bResult) {
            fLastError = GetLastError();
            printf("Event.set: %d\n", fLastError);
            return false;
        }

        return true;
    }

    bool reset()
    {
        BOOL bResult = ::ResetEvent(fHandle);
        if (0 == bResult) {
            fLastError = GetLastError();
            return false;
        }

        return true;
    }

    bool wait(DWORD dwMilliseconds = INFINITE)
    {
        BOOL bAlertable = 0;
        DWORD res = ::WaitForSingleObjectEx(fHandle,dwMilliseconds,bAlertable);
    
        if (WAIT_FAILED == res) {
            fLastError = GetLastError();
            return false;
        }

        fLastError = res;
        return true; 
    }
};