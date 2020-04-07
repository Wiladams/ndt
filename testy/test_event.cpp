#include "Event.hpp"
#include "Thread.hpp"
#include <cstdio>

Event mainEvent(false, false);

DWORD task1(void *param)
{
    Event t(false, false);

    t.wait(2000);
    printf("task1, after wait\n");
    mainEvent.set();

    printf("task1, after set, exiting\n");

    return 0;
}


DWORD task2(void *param)
{
    printf("task2, waiting on mainEvent\n");

    mainEvent.wait();

    printf("task2, after waiting main event\n");

    return 0;
}

void test_threadwait()
{
    Thread t1(task1);
    Thread t2(task2);

    t2.resume();
    t1.resume();
}

void test_waitreset()
{
    Event e(false, false);

    printf("e.isValid(): %d\n", e.isValid(), e.getLastError());

    // wait forever
    e.wait(3000);
    printf("after e.wait(3000): %d\n", e.getLastError());
    
    e.reset();
    e.wait();
}


void main()
{
    //test_waitreset();
    test_threadwait();
    
    Event waiter(false, false);
    waiter.wait();
}