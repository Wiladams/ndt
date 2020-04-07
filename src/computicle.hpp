/*
    References
    https://stackoverflow.com/questions/11706985/win32-thread-safe-queue-implementation-using-native-windows-api

*/

#include "w32.hpp"
#include "thread.hpp"
#include "Event.hpp"

// Internal to computicle messages
// User code should not use these
#define CM_NULL             ((WM_USER)+1)
#define CM_SYNC             ((WM_USER)+2)
#define CM_SYNCANDEXIT      ((WM_USER)+3)

// User messages can begin here and beyond
#define CM_USER             ((WM_USER)+1024)


class Computicle
{
    // Status constants
    enum Status {
        PENDING = 0,
        RUNNING = 1,
        FINISHED = 255
    };


    // Synchronization events
    Event fReadyEvent;
    Event fFinishedEvent;
    Event fSyncEvent;

    // Computicle state
    Thread fThread;
    int fLastError;
    int fStatus;

    // This encapsulated work routine is what gets
    // executed within a thread context.  
    // The thread has a message queue, which is the 
    // mechanism for communicating with the computicle
    // The WorkRoutine runs the loop which pulls messages
    // from the thread and executes them.
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
            // Is this severe enough to return?
            //printf("signalReady(), ERROR: %d\n", self->fReadyEvent.getLastError());
        }

        self->setStatus(Computicle::RUNNING);

        int retValue = 0;
        while (true)
        {
            int retValue = ::GetMessageA(&Msg, nullptr, 0, 0);
            
            if (retValue == 0) {
                // received a WM_QUIT
                // so do whatever is necessary for quitting
                self->onQuit();
                break;
            } else if (retValue == -1) {
                self->fLastError = ::GetLastError();
                break;
            } 

            // If we're here, it's normal run
            // of the mill messages
            if (Msg.message == CM_SYNC) {
                self->signalSync();
            } else if (Msg.message == CM_SYNCANDEXIT) {
                // signal sync
                // then exit out of the queue processing
                self->signalSync();
                break;
            } else if (Msg.message == CM_NULL) {
                // Do nothing in particular
            } else {
                // In all other cases, pass it along
                self->onMessage(Msg);
            }
        }

        self->setStatus(Computicle::FINISHED);
        self->signalFinished();


        return 0;
    }

    bool signalReady() {return fReadyEvent.set();}
    bool signalFinished() {return fFinishedEvent.set();}
    bool signalSync() {return fSyncEvent.set();}

    void setStatus(int status)
    {
        fStatus = status;
    }

public:
    Computicle(bool autoRun = true)
        : fThread(Computicle::WorkRoutine, this)
        , fLastError(0)
        //, fReadyEvent(false, false)
        //, fFinishedEvent(false, false)
        //, fSyncEvent(false, false)
    {
        fStatus = Computicle::PENDING;

        // start running immediately
        // This could be deferred
        if (autoRun) {
            run();
        }
    }

    virtual ~Computicle()
    {
        // anything to do for cleanup?
    }

    int getStatus() const {return fStatus;}
    int getLastError() const {return fLastError;}

    bool run()
    {
        // resume thread
        fThread.resume();
        
        // wait on event to continue
        // we wait on the ready event to be set
        // to ensure the caller that we're ready
        // to process messages.
        bool success = fReadyEvent.wait();
        if (!success) {
            //printf("fReadyEvent.wait() ERROR: %d\n", fReadyEvent.getLastError());
            fLastError = fReadyEvent.getLastError();
            return false;
        }

        return true;
    }

    // Send a message that might have some parameters
    // Ideally this would be private to the computicle
    // and you would only be able to implement methods
    // that then utilize this function, but since the 
    // only way to talk to a computicle is through messages
    // this fundamental feature must be exposed
    bool send(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        //printf("send: %d\n", msg);

        BOOL bResult = ::PostThreadMessageA(fThread.getThreadId(), msg, wParam, lParam);
        if (!bResult) {
            fLastError = GetLastError();
            return false;
        }

        return true;
    }
    
    // Send a simple message
    bool send(UINT msg)
    {
        return send(msg, 0, 0);
    }

    // Cause the caller to pause until
    // the CM_SYNC message is processed
    // through the message queue
    // This is good for synchronization across 
    // computicles.  You can send a command, then 
    // immediately issue a sync to ensure the previous
    // command has executed before continuing.
    bool sync()
    {
        fSyncEvent.reset();
        send(CM_SYNC);
        return fSyncEvent.wait();
    }

    // Send the CM_SYNC message
    // wait for it to be processed through the queue
    // cause the computicle to quit processing
    // After calling this, a caller can then join() the
    // computicle to ensure it has exited processing
    bool syncAndExit()
    {
        fSyncEvent.reset();
        send(CM_SYNCANDEXIT);
        return fSyncEvent.wait();
    }


    // The caller will force the computicle
    // to quit almost immediately
    bool forceQuit()
    {
        return send(WM_QUIT, 0,0);
    }

    // The caller will block until the computicle
    // is finished.
    bool join(DWORD millis=INFINITE)
    {
        return fFinishedEvent.wait(millis);
    }

    // Reactions
    // return true if the message was handled
    virtual bool onMessage(const MSG &msg)
    {
        printf("onMessage(%d)\n", msg.message);

        return true;
    }

    virtual bool onError()
    {
        // tempted to quit, but that could
        // end up being a circular thing
        // quit();
        return true;
    }

    // Called as the last thing that happens
    // after computicle has stopped processing 
    // messages
    virtual void onQuit() 
    {
        printf("Computicle.onQuit()\n");
    }
};
