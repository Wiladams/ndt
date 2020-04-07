#include "w32.hpp"

/*
    This is rudimentary thread support.  You can easily
    create a thread, and start it.

    DWORD ARoutine(void *param)
    {
        // my thread routine
    }

    Thread t1(routine)

    Threads are a fairly low level primitive.  More advanced
    constructs can be composed from threads.
*/

class Thread
{
    PTHREAD_START_ROUTINE fRoutine;     // The routine we're going to run
    HANDLE fHandle;
    DWORD fThreadId;
    int fLastError;

public:
    Thread(PTHREAD_START_ROUTINE routine, void * lpParameter)
        : fRoutine(routine),
        fThreadId(0),
        fLastError(0)
    {
        LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;
        SIZE_T dwStackSize = 0;
        DWORD dwCreationFlags = CREATE_SUSPENDED;
        DWORD lpThreadId;

        fHandle = ::CreateThread(lpThreadAttributes,dwStackSize, routine,lpParameter,dwCreationFlags,&lpThreadId);
        if (fHandle == nullptr) {
            // return without indicating we're valid
            return ;
        }

        fThreadId = lpThreadId;
    }

    bool isValid() const {return fHandle != nullptr;}
    int getLastError() const {return fLastError;}
    DWORD getThreadId() const {return fThreadId;}

    int resume()
    {
        if (!isValid()) {
            return 0;
        }

        return ::ResumeThread(fHandle);
    }

    // force a thread to terminate
    // if you're inside the thread, you should 
    // call ExitThread()
    bool terminate(unsigned long exitCode=0)
    {
        bool success = ::TerminateThread(fHandle, exitCode) != 0;

        return success;
    }
};