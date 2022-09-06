#pragma once
#pragma comment (lib, "Synchronization.lib")

#include "pubsub.h"

#include "Thread.hpp"
#include "stopwatch.hpp"

typedef void (*TIMERCALLBACK)(void* param, int64_t tickCount);

class TickTopic : public Topic<double>
{
    Thread fThread;
    StopWatch fsw;

    uint64_t fTickCount=0;
    uint64_t fDroppedTicks=0;
    size_t fInterval=1000;

    static DWORD __stdcall generateTicks(void* param)
    {
        TickTopic* ticker = (TickTopic*)(param);

        StopWatch sw;
        sw.reset();
        double nextMillis = sw.millis() + ticker->getInterval();

        //printf("GENERATING TICKS\n");

        while (true) {
            int64_t interval = ticker->getInterval();

            while (nextMillis <= sw.millis())
            {
                nextMillis += interval;
                ticker->incrementDropped();
            }

            DWORD duration = (DWORD)(nextMillis - sw.millis());

            // fastest/cheapest in-process waiting primitive
            if (WaitOnAddress(&interval, &interval, 8, duration) == 0)
            {
                auto err = ::GetLastError();    // 1460 == ERROR_TIMEOUT
            }

            ticker->tick(sw.seconds());
        }

        return 0;
    }

public:

    TickTopic()
        : fThread(TickTopic::generateTicks, this)
    {
    }

    TickTopic(const double freq)
        : fThread(TickTopic::generateTicks, this)
        , fTickCount(0)
        , fInterval((uint64_t)(1000.0 / freq))
    {
    }


    int64_t getInterval() { return fInterval; }
    void setInterval(const uint64_t newInterval) { fInterval = newInterval; }
    uint64_t setFrequency(const size_t freq) { fInterval = ((uint64_t)(1000.0 / freq)); return fInterval; }
    int64_t getTickCount() { return fTickCount; }
    
    uint64_t getDroppedTicks() { return fDroppedTicks; }
    void incrementDropped() { fDroppedTicks++; }

    void start()
    {
        fThread.resume();
    }

    void pause()
    {
        fThread.suspend();
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
