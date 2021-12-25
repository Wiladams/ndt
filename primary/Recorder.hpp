
/*
    Record a Surface

        Then, to generate video

        ffmpeg -framerate 15 -i <name>%06d.bmp <outputname>.mp4
*/

#include "Surface.h"
#include <string>

class Recorder
{
    std::shared_ptr<Surface> fSurface;

    std::string fBasename;
    int fFrameRate;

    bool fIsRecording;
    int fCurrentFrame;
    int fMaxFrames;

    Recorder() = delete;    // Don't want default constructor

public:
    Recorder(std::shared_ptr<Surface> surf, const char* basename = "frame", int fps = 30, int maxFrames=0)
        : fSurface(surf)
        , fBasename(basename)
        , fFrameRate(fps)
        , fIsRecording(false)
        , fCurrentFrame(0)
    {
    }

    bool isRecording() { return fIsRecording; }
    void toggleRecording() {
        if (fIsRecording)
            pause();
        else
            record();
    }

    void saveFrame()
    {
        if (!fIsRecording) 
            return ;

        if (fMaxFrames > 0) {
            if (fCurrentFrame >= fMaxFrames) {
                return ;   // reached maximum frames, maybe stop timer
            }
        }
        
        char frameName[256];
        sprintf_s(frameName, 255, "%s%06d.bmp", fBasename.c_str(), fCurrentFrame);
        BLImageCodec codec;
        codec.findByName("BMP");
        fSurface->getImage().writeToFile(frameName, codec);

        fCurrentFrame = fCurrentFrame + 1;
    }

    bool record()
    {
        // need to cancel existing timer
        if (fIsRecording)
            return false;

        fIsRecording = true;
    }

    void pause()
    {      
        fIsRecording = false;
    }

    void stop()
    {
        //if (fTimer)
        //    fTimer.cancel();

        fCurrentFrame = 0;
        fIsRecording = false;
    }

};

