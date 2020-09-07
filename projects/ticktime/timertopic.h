#pragma once
#pragma comment (lib, "Synchronization.lib")

#include "pubsub.h"

#include "Thread.hpp"
#include "stopwatch.hpp"

typedef void (*TIMERCALLBACK)(void* param, int64_t tickCount);

class TimerTopic : public Topic<double>
{
    StopWatch fsw;
    int64_t fTickCount;
    Thread fThread;
    int64_t fInterval;

    static DWORD __stdcall generateTicks(void* param)
    {
        TimerTopic* ticker = (TimerTopic*)(param);
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

            ticker->tick(sw.seconds());
        }

        return 0;
    }

public:
    //TIMERCALLBACK callit, void* callitParam
    TimerTopic(const double freq)
        :fInterval((uint64_t)(1000.0/freq))
        , fThread(TimerTopic::generateTicks, this)
        , fTickCount(0)
    {
    }


    int64_t getInterval() { return fInterval; }

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

    void tick(const double secs)
    {
        fTickCount++;
        notify(secs);
    }
};
