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
    int64_t fInterval;

    static DWORD __stdcall generateTicks(void* param)
    {
        TickTopic* ticker = (TickTopic*)(param);

        StopWatch sw;
        sw.reset();
        DWORD nextMillis = (DWORD)sw.millis();

        //printf("GENERATING TICKS\n");

        while (true) {
            auto interval = ticker->getInterval();

            while (nextMillis <= (DWORD)sw.millis())
            {
                nextMillis += (DWORD)interval;
                printf("dropped frame: %d\n", nextMillis);
            }

            DWORD duration = nextMillis - (DWORD)sw.millis();
            if (duration < 0) {
                //ticker->dropFrame();
                duration = 0;
                //printf("dropped frame\n");
            }

            WaitOnAddress(&interval, &interval, 8, (DWORD)duration);

            ticker->tick(sw.seconds());
        }

        return 0;
    }

public:
    //TIMERCALLBACK callit, void* callitParam
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
