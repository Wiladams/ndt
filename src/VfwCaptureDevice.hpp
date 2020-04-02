#pragma once

/*
    We won't actually use a capture window to 
    display anything.  We will use it just for the
    ability to do message passing.

    The capture Window has the ability to connect
    to a specific driver, and will translate all 
    the messages.
*/

#include <cstdio>
#include <string.h>

#include "w32.hpp"
#include "w32_video.hpp"

class VfwDeviceDriver
{
public:
    unsigned int fDriverIndex;
    char fName[100];
    char fVersion[100];
    bool fIsValid;

    VfwDeviceDriver()
        : fIsValid(false),
        fDriverIndex(0)
    {
        memset(fName, 0, sizeof(fName));
        memset(fVersion, 0, sizeof(fVersion));
    }

    VfwDeviceDriver(const VfwDeviceDriver &rhs)
        :fDriverIndex(rhs.fDriverIndex),
        fIsValid(rhs.fIsValid)
    {
        strncpy_s(fName, sizeof(fName),rhs.fName, sizeof(rhs.fName) );
        strncpy_s(fVersion, sizeof(fVersion),rhs.fVersion, sizeof(rhs.fVersion) );
    }

    VfwDeviceDriver(const unsigned int driverIndex)
        : fIsValid(false)
    {
        fDriverIndex = 0;
        memset(fName, 0, sizeof(fName));
        memset(fVersion, 0, sizeof(fVersion));

        fIsValid = capGetDriverDescriptionA (driverIndex, fName, sizeof(fName), fVersion, sizeof(fVersion)) == 1;
        fDriverIndex = driverIndex;
    }

    bool isValid() const {return fIsValid;}
    unsigned int getIndex() const {return fDriverIndex;}
    const char * getName() const {return fName;}
    const char * getVersion() const {return fVersion;}
    
    char * toString(int n, char *buff)
    {
        int nWritten = snprintf (buff, n, "%s %s", fName, fVersion);
        return buff;
    }
};


class VfwCaptureDevice
{
    HWND fWindowHandle;
    int fDeviceId;
    bool fConnected;
    VfwDeviceDriver fDeviceDriver;
    CAPDRIVERCAPS fDriverCapabilities;
    CAPSTATUS fCaptureStatus;
    int fWidth;
    int fHeight;

public:
    VfwCaptureDevice(unsigned int index, int width, int height, int fps, int windowStyle, HWND parentWindow)
    {
        fWidth = width;
        fHeight = height;

        // The id here should be something other than 0
        // needs to be zero for some reason
        int deviceId = 0;   
        fConnected = false;

        fWindowHandle = capCreateCaptureWindowA("VfwCaptureDevice", windowStyle, 0,0,320,240, nullptr, deviceId);
        printf("VfwCaptureDevice, handle: %p\n", fWindowHandle);

        // Set userdata to be this so that on 
        // callbacks, we can get a handle on this object instance
        SendMessage(WM_CAP_SET_USER_DATA, (WPARAM) 0, (LPARAM)this); 

    }

    VfwCaptureDevice()
        :VfwCaptureDevice(0, 320, 240, 15, WS_VISIBLE, nullptr)
    {
    }

    bool isValid() {return fWindowHandle != nullptr;}
    bool isConnected() {return fConnected;}

    void setWidth(const int width) {fWidth = width;}
    int getWidth() {return fWidth;}

    void setHeight(const int height) {fHeight = height;}
    int getHeight() {return fHeight;}

    HWND getWindowHandle() {return fWindowHandle;}

    
    bool connectToDriver(const VfwDeviceDriver &driver)
    {
        if (!driver.isValid()) {
            printf("connectToDriver, driver not valid\n");
            return false;
        }

        fDeviceDriver = driver;

        fConnected = (1== SendMessage(WM_CAP_DRIVER_CONNECT, fDeviceDriver.getIndex(), 0));

        if (!fConnected) {
            printf("DRIVER_CONNECT failed\n");

            return false;
        }

        // If we did get connected, then we want to grab the
        // driver's capabilities.
        int wSize = sizeof(fDriverCapabilities);
        bool success = (1 == SendMessage(WM_CAP_DRIVER_GET_CAPS, wSize, (LPARAM)&fDriverCapabilities));

        if (!success) {
            return false;
        }

        onConnected();


        return true;
    }

    // When we're finally connected, allow the sub-class to know
    virtual void onConnected() {}

    bool disconnect()
    {
      return (1 ==  SendSimpleMessage(WM_CAP_DRIVER_DISCONNECT));
    }

    bool captureGetSetup(CAPTUREPARMS &s)
    {
        size_t wSize = sizeof(s);
        return 1 == SendMessage(WM_CAP_GET_SEQUENCE_SETUP, wSize, (LPARAM)&s);
    }

    bool captureSetSetup(CAPTUREPARMS &s)
    {
        size_t wSize = sizeof(s);
        return 1 == SendMessage(WM_CAP_SET_SEQUENCE_SETUP, wSize, (LPARAM)&s);
    }

    /// <summary>
    /// If the driver supports a dialog box to select the video Source, this
    /// method will display it on the screen.
    /// </summary>
    bool selectVideoSource()
    {
        if (!isConnected()) {
            return false;
        }

        bool success = (1==SendSimpleMessage(WM_CAP_DLG_VIDEOSOURCE)); 
        
        return success;
    }

    bool selectVideoFormat()
    {
        if (!isConnected()) {
            return false;
        }

        bool success = (1 == SendSimpleMessage(WM_CAP_DLG_VIDEOFORMAT));

        return success;
    }

    bool selectVideoDisplay()
    {
        if (!isConnected()) {
            return false;
        }

        bool success = (1 == SendSimpleMessage( WM_CAP_DLG_VIDEODISPLAY));
    }

    bool selectVideoCompression()
    {
        if (!isConnected()) {
            return false;
        }

        bool success = (1 == SendSimpleMessage( WM_CAP_DLG_VIDEOCOMPRESSION));
    }

    int getVideoFormatSize()
    {
        int retValue = SendSimpleMessage(WM_CAP_GET_VIDEOFORMAT);
        return retValue;
    }

    CAPDRIVERCAPS getDriverCapabilities()
    {
        return fDriverCapabilities;
    }

    int getVideoFormat(BITMAPINFO &s)
    {
        size_t wSize = sizeof(s);
        int  bytesCopied = SendMessage(WM_CAP_GET_VIDEOFORMAT, wSize, (LPARAM)&s);

        return bytesCopied;
    }
    
    bool setVideoFormat(BITMAPINFO &s)
    {
        int wSize = sizeof(s);
        bool success = (1 == SendMessage(WM_CAP_SET_VIDEOFORMAT, wSize, (LPARAM)&s));
        return success;
    }
    


    //#region Frame Grab
        /// <summary>
        /// Grab a single frame from the device.  Overlay and preview are stopped if they
        /// are currently active.
        /// </summary>
        /// <returns></returns>
        bool grabFrame()
        {
            return 1 == SendSimpleMessage(WM_CAP_GRAB_FRAME);
        }

        /// <summary>
        /// Grab a single from from the device.  Overlay and Preview are not stopped.
        /// </summary>
        /// <returns></returns>
        bool grabFrameNoStop()
        {
            return 1 == SendSimpleMessage(WM_CAP_GRAB_FRAME_NOSTOP);
        }
    //#endregion

/*
    // Only needed for a preview window
    // Maybe a preview window should be a separate class
            protected bool capPreview(bool f)
        {
            int BOOL = f == true ? 1 : 0;
            return 1 ==  SendMessage(Vfw.WM_CAP_SET_PREVIEW, (uint)BOOL, 0);
        }

        protected bool capPreviewRate(int wMS)
        {
            return 1 == SendMessage(Vfw.WM_CAP_SET_PREVIEWRATE, (uint)(wMS), 0);
        }

        protected bool capPreviewScale(bool f)
        {
            int BOOL = f == true ? 1 : 0;
            return 1 == SendMessage(Vfw.WM_CAP_SET_SCALE, (uint)(BOOL), 0);
        }
*/
    
    
    
    // Send a message that doesn't have any parameters
    int SendSimpleMessage(int msg)
    {
        int retValue = SendMessageA(fWindowHandle, msg, (WPARAM)0, (LPARAM)0);
        return retValue;
    }

    int SendMessage(int msg, WPARAM wParam, LPARAM lParam)
    {
        int retValue = SendMessageA(fWindowHandle, msg, (WPARAM)wParam, lParam);
        return retValue;
    }
};
