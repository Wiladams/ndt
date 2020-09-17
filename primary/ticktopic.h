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

    int64_t fTickCount;
    uint64_t fDroppedTicks;
    int64_t fInterval;

    static DWORD __stdcall generateTicks(void* param)
    {
        TickTopic* ticker = (TickTopic*)(param);

        StopWatch sw;
        sw.reset();
        auto nextMillis = sw.millis() + ticker->getInterval();

        //printf("GENERATING TICKS\n");

        while (true) {
            auto interval = ticker->getInterval();

            while (nextMillis <= sw.millis())
            {
                nextMillis += interval;
                ticker->incrementDropped();
            }

            DWORD duration = (DWORD)(nextMillis - sw.millis());

            // fastest/cheapest in-process waiting primitive
            WaitOnAddress(&interval, &interval, 8, duration);

            ticker->tick(sw.seconds());
        }

        return 0;
    }

public:

    TickTopic(const double freq)
        :fInterval((uint64_t)(1000.0/freq))
        , fThread(TickTopic::generateTicks, this)
        , fTickCount(0)
    {
    }


    int64_t getInterval() { return fInterval; }
    void setInterval(const uint64_t newInterval) { fInterval = newInterval; }
    uint64_t setFrequency(const uint64_t freq) { fInterval = ((uint64_t)(1000.0 / freq)); return fInterval; }
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
