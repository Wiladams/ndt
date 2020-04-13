#include "TimeTicker.hpp"
#include "Event.hpp"

StopWatch sw;

void ticker(void *param, int64_t tickCount)
{
    printf("tick: %lld %3.2f\n", tickCount, sw.seconds());
}

void main()
{
    TimeTicker t(250, ticker, nullptr);
    t.start();
    sw.reset();

    // wait around forever
    Event e;
    e.wait();
}