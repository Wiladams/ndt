#pragma once

#include "VfwCaptureDevice.hpp"

    /// <summary>
    /// The VfwCameraDevice object represents a Vfw video capture device.  As Vfw
    /// also supports audio capture devices, this object is specialized to only
    /// the video capture interfaces.
    /// </summary>
class VfwCameraDevice : public VfwCaptureDevice
{

    // Configuration fields
    BITMAPINFO fBitmapInfo;
    CAPTUREPARMS fCaptureParams;
    unsigned int fCaptureInterval;

public:
    VfwCameraDevice(int index, int width, int height, int fps, int windowStyle, HWND parentWindow)
        : VfwCaptureDevice(index, width, height, fps, windowStyle, parentWindow)
    {
        // bitmapinfoheader MUST be initialized
        fBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    }

    VfwCameraDevice(int width, int height)
        : VfwCameraDevice(0, width, height, 15, 0, nullptr)
    {
    }



    void configureVideoFormat()
    {
        // Get the current format information
        int formatSize = getVideoFormatSize();
        getVideoFormat(fBitmapInfo);

        //PrintCompressionType(fBitmapInfo.bmiHeader.biCompression);

        // Fill in the things we want to change
        fBitmapInfo.bmiHeader.biWidth = getWidth();
        fBitmapInfo.bmiHeader.biHeight = getHeight();
        fBitmapInfo.bmiHeader.biPlanes = 1;
        fBitmapInfo.bmiHeader.biBitCount = 24; // 24 bits per frame - RGB
        fBitmapInfo.bmiHeader.biCompression = BI_RGB;

        // Try to set the video format
        bool setSuccess = setVideoFormat(fBitmapInfo);

        // Get the settings again, and set the width
        // and height based on what was actually set
        getVideoFormat(fBitmapInfo);

        setWidth(fBitmapInfo.bmiHeader.biWidth);
        setHeight(fBitmapInfo.bmiHeader.biHeight);
    }

    /// <summary>
    /// We can configure some settings related to capturing images from
    /// the camera.  Primarily we want to control how many buffers are 
    /// used in the capture process.
    /// 
    /// If we are doing continuous capture, we can configure
    /// the interval between subsequent frames.
    /// </summary>
    void configureCapture()
    {
        //fCaptureParams = new CAPTUREPARMS();
            
        captureGetSetup(fCaptureParams);

        // These are the core parameters that must be 
        // set when we're capturing video

        fCaptureParams.dwRequestMicroSecPerFrame = (unsigned int)fCaptureInterval;   // Start with 15 frames per second
        fCaptureParams.wPercentDropForError = 10;               // Percentage of frames that can drop before error is signaled
        fCaptureParams.fYield = true;                          // If set to true, a thread will be spawned            
        fCaptureParams.wNumVideoRequested = 2;                  // Maximum number of video buffers to allocate
            
        // Related to capturing audio
        fCaptureParams.fCaptureAudio = false;
        fCaptureParams.wNumAudioRequested = 0;
        fCaptureParams.dwAudioBufferSize = 0;

        fCaptureParams.fLimitEnabled = false;
        fCaptureParams.wTimeLimit = 0;

        fCaptureParams.fDisableWriteCache=false;
        fCaptureParams.AVStreamMaster=0;

        // Used when capturing to a file
        fCaptureParams.dwIndexSize = 0;             // Determines the limit of audio/video buffers that can be captured
        fCaptureParams.wChunkGranularity = 0;

        // Related to MCI
        fCaptureParams.fMCIControl = false;
        fCaptureParams.fStepMCIDevice = false;
        fCaptureParams.dwMCIStartTime = 0;
        fCaptureParams.dwMCIStopTime = 0;
        fCaptureParams.fStepCaptureAt2x = false;
        fCaptureParams.wStepCaptureAverageFrames = 5;

            
        // These are related to mouse and keyboard control of capturing
        // Since we want total control of the capture process
        // we don't want these used at all.
        fCaptureParams.vKeyAbort = 0;
        fCaptureParams.fAbortLeftMouse = false;
        fCaptureParams.fAbortRightMouse = false;
        fCaptureParams.fMakeUserHitOKToCapture = false;

        fCaptureParams.fUsingDOSMemory = false;

            
        // Configure the actual capture settings
        captureSetSetup(fCaptureParams);
    }



    void onConnected()
    {
        //printf("VfwCameraDevice.onConnected - BEGIN\n");

        configureVideoFormat();
        configureCapture();
    }



    // unique methods
    bool startStreaming()
    {
        bool success = SendSimpleMessage(WM_CAP_SEQUENCE_NOFILE) == 1;
        return success;
    }

    bool stopStreaming()
    {
        bool success = SendSimpleMessage(WM_CAP_STOP) == 1;
        return success;
    }

        /// <summary>
        /// Grab a single frame from the video device.  Disables Overlay
        /// and preview.
        /// 
        /// Since we want complete control of video grabbing, this is what we
        /// want.  We don't want or need a preview window, and we don't want 
        /// to be doing overlay either.
        /// 
        /// As the frame is grabbed, our frameDelegate function is called with 
        /// the retrieved frame.
        /// </summary>
    bool grabSingleFrame()
    {
        bool success = SendSimpleMessage(WM_CAP_GRAB_FRAME) == 1;
        //bool success = SendSimpleMessage(WM_CAP_GRAB_FRAME_NOSTOP)==1;

        return success;
    }

    bool startPreview()
    {
        bool success = SendMessage(WM_CAP_SET_PREVIEW, 1, 0)==1;

        return success;
    }

    bool stopPreview()
    {
        bool success = SendMessage(WM_CAP_SET_PREVIEW, 0, 0)==1;

        return success;
    }

    bool hidePreview()
    {
        ShowWindow(getWindowHandle(), SW_HIDE);
        return true;
    }

    int setPreviewRate(int value)
    {
        return SendMessage(WM_CAP_SET_PREVIEWRATE, value, 0);
    }

    int setPreviewScale(int value)
    {
        return SendMessage(WM_CAP_SET_SCALE, value, 0);
    }

    // Properties
    BITMAPINFO GetBitmapInfo()
    {
            return fBitmapInfo;
    }
/*
    VfwVideoFrameDelegate FrameDelegate
        {
            get { return fVideoFrameDelegate; }
            set
            {
                fVideoFrameDelegate = value;
                SetCallbackOnFrame(fVideoFrameDelegate);
            }
        }

        VfwVideoFrameDelegate StreamDelegate
        {
            get { return fVideoStreamDelegate; }
            set
            {
                fVideoStreamDelegate = value;
                SetCallbackOnVideoStream(fVideoStreamDelegate);
            }
        }
*/


    bool setCallbackOnFrame(CAPVIDEOCALLBACK fpProc)
    {
        bool success = SendMessage(WM_CAP_SET_CALLBACK_FRAME, 0, (LPARAM)(LPVOID)fpProc) == 1;
        return success;
    }


/*
        // Callback Delegate Functions
        private int StatusCallback(IntPtr hWnd, int nID, string lpsz)
        {
            return 0;   // Indicating we're doing streaming or single frame capture
        }

        private int ControlCallback(IntPtr hWnd, int nState)
        {
            if (nState == Vfw.CONTROLCALLBACK_PREROLL)
                return 1;

            else
                return 1;
        }

        private int ErrorCallback(IntPtr hWnd, int nID, string lpsz)
        {

            return 1;
        }

        private void StreamCallBack(IntPtr hwnd, ref VIDEOHDR hdr)
        {
            Console.WriteLine("VfwCameraDevice.StreamCallBack");
        }

        private void FrameCallBack(IntPtr hwnd, ref VIDEOHDR hdr)
        {
            Console.WriteLine("VfwCameraDevice.Frame Received");
        }
        #endregion
    }
*/
};
