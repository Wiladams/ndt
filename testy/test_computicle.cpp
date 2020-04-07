#include "w32.hpp"
#include "thread.hpp"
#include "Event.hpp"
#include <cstdio>

class Computicle
{
    Event fReadyEvent;
    Event fFinishedEvent;
    Thread fThread;
    int fLastError;

    static DWORD WorkRoutine(void *param)
    {
        Computicle *self = (Computicle *)param;

        // The message object that will be stuffed
        // with message information
        MSG Msg;

        // Do an initial PeekMessage to ensure the event
        // queue is setup
        int res = ::PeekMessageA(&Msg, NULL, WM_USER, WM_USER, PM_REMOVE);



        // Set event indicating ready to be used
        if (!self->signalReady())
        {
            printf("signalReady(), ERROR: %d\n", self->fReadyEvent.getLastError());
        }

        // loop over pulling items off the message
        // queue for the thread
        // public whenever we find something
        // quit if we see a quit message
        // BUGBUG, a better way would be to WaitMultiple...

        int retValue = 0;
        while (true)
        {
            int retValue = ::GetMessageA(&Msg, nullptr, 0, 0);
            
            printf("RECEIVED: %d\n", Msg.message);

            if (retValue == 0) {
                // received a WM_QUIT
                // so do whatever is necessary for quitting
                self->onQuit();
                break;
            }

            if (retValue == -1) {
                break;
            }

            if (Msg.message == 100) {
                self->quit();
            }

        }

        return 0;
    }

    bool signalReady()
    {
        if (!fReadyEvent.set()) {
            return false;
        }

        return true;
    }

    bool signalFinished()
    {
        return fFinishedEvent.set();
    }

public:
    Computicle()
        : fThread(Computicle::WorkRoutine, this)
        , fLastError(0)
        , fReadyEvent(false, false)
        , fFinishedEvent(false, false)
    {
        // resume thread
        fThread.resume();

        // wait on event to continue
        bool success = fReadyEvent.wait();
        if (!success) {
            printf("fReadyEvent.wait() ERROR: %d\n", fReadyEvent.getLastError());
        }

        //printf("done waiting\n");
    }

    virtual ~Computicle()
    {
    }

    bool send(UINT msg, WPARAM wParam=0, LPARAM lParam=0)
    {
        //printf("send: %d\n", msg);

        BOOL bResult = ::PostThreadMessageA(fThread.getThreadId(), msg, wParam, lParam);
        if (!bResult) {
            fLastError = GetLastError();
            return false;
        }

        return true;
    }

    virtual void onQuit() 
    {
        printf("Computicle.onQuit()\n");
    }

    bool quit()
    {
        return send(WM_QUIT, 0,0);
    }

    bool join(DWORD millis=INFINITE)
    {
        return fFinishedEvent.wait(millis);
    }
};


void main()
{
    Computicle c1;

    for (int i = 1; i<= 100; i++)
    {
        //printf("sending: %d\n", i);
        c1.send(i);
    }

    //c1.quit();
    c1.join();
}