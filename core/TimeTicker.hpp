#pragma once
//#pragma comment (lib, "Synchronization.lib")

#include "Thread.hpp"
#include "stopwatch.hpp"
#include <cstdio>

typedef void (*TIMERCALLBACK)(void * param, int64_t tickCount);

class TimeTicker
{
    StopWatch fsw;
    int64_t fTickCount;
    Thread fThread;
    int64_t fInterval;
    TIMERCALLBACK fCallbackFunc;
    void * fCallbackParam;

    static DWORD __stdcall generateTicks(void *param)
    {
        TimeTicker * ticker = (TimeTicker *)(param);
        int64_t interval = ticker->getInterval();

        StopWatch sw;
        sw.reset();
        DWORD nextMillis = (DWORD)sw.millis();

//printf("GENERATING TICKS\n");

        while (true) {
            nextMillis += (DWORD)interval;
            DWORD duration = nextMillis - (DWORD)sw.millis();
            if (duration < 0) {
                duration = 0;
            }

            WaitOnAddress(&interval, &interval, 8, (DWORD)duration);

            ticker->tick();
        }

        return 0;
    }

public:
    TimeTicker(int64_t interval, TIMERCALLBACK callit, void * callitParam)
        : fInterval(interval)
        , fThread(TimeTicker::generateTicks, this)
        , fTickCount(0)
        , fCallbackFunc(callit)
        , fCallbackParam(callitParam)
    {
    }

    int64_t getInterval() {return fInterval;}

    void start()
    {
        fThread.resume();
    }

    void pause()
    {
        //fThread.suspend();
    }

    // If we stop
    // reset our current time to now
    void stop()
    {
        fThread.terminate();
    }

    void tick()
    {
        fTickCount++;
        if (fCallbackFunc != nullptr){
            fCallbackFunc(fCallbackParam, fTickCount);
        }
    }
};