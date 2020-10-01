#pragma once

/*
    Take a snapshot of the video display using GDI.

    Construct an instance of the object by determining
    the location and size of the capture.  It is a fixed
    size and location, so can not be moved.  If you want
    to capture a different location, create another capture
    object.

    ScreenSnapshot ss(x,y, width, height);

    getting an actual snapshot, call next()

    ss.next()

    And finally to get a pixelbuffer that was captured

    ss.getCurrent()
    ss.getImage()

    References:
    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
    https://www.codeproject.com/Articles/1017223/CaptureManager-SDK-Capturing-Recording-and-Streami
    https://www.codeproject.com/Tips/1116253/Desktop-Screen-Capture-on-Windows-via-Windows-Desk
*/

#include "canvas.h"

#include <memory>
#include <d3d11.h>
#include <dxgi1_2.h>

class DXCapture : public GCanvas
{
    int fOriginX;
    int fOriginY;
    int fWidth;
    int fHeight;

    // All the stuff needed for DirectX Capture
    IDXGIOutputDuplication* fDeskDupl = nullptr;
    ID3D11Texture2D* fAcquiredDesktopImage = nullptr;
    _Field_size_bytes_(m_MetaDataSize) BYTE* fMetaDataBuffer = nullptr;
    unsigned int fMetaDataSize = 0;
    unsigned int fOutputNumber = 0;
    DXGI_OUTPUT_DESC fOutputDesc;
    ID3D11Device* fDevice = nullptr;

    DXGI_OUTDUPL_FRAME_INFO FrameInfo;
    IDXGIResource* DesktopResource = nullptr;

public:
    DXCapture(int x, int y, int awidth, int aheight, uint32_t threadCount = 0)
        : GCanvas(awidth, aheight),
        fOriginX(x),
        fOriginY(y),
        fWidth(awidth),
        fHeight(aheight)
    {
        memset(&fOutputDesc, 0,sizeof(fOutputDesc));
    }

    // Destructor
    ~DXCapture()
    {
        if (fDeskDupl != nullptr) {
            fDeskDupl->Release();
            fDeskDupl = nullptr;
        }

        if (fAcquiredDesktopImage != nullptr) {
            fAcquiredDesktopImage->Release();
            fAcquiredDesktopImage = nullptr;
        }

        if (fMetaDataBuffer != nullptr)
        {
            delete[] fMetaDataBuffer;
            fMetaDataBuffer = nullptr;
        }

        if (fDevice != nullptr) {
            fDevice->Release();
            fDevice = nullptr;
        }
    }

    size_t width() { return fWidth; }
    size_t height() { return fHeight; }

    // take a snapshot
    //
// Get next frame and write it into Data
//
    HRESULT GetFrame(FRAME_DATA* Data)
    {
        HRESULT hr = S_OK;

        //Get new frame
        hr = DeskDupl->AcquireNextFrame(500, &FrameInfo, &DesktopResource);
        if (FAILED(hr))
        {
            if ((hr != DXGI_ERROR_ACCESS_LOST) && (hr != DXGI_ERROR_WAIT_TIMEOUT))
            {
                DisplayErr(L"Failed to acquire next frame in DUPLICATIONMANAGER", L"Error", hr);
            }
            return hr;
        }

        // If still holding old frame, destroy it
        if (AcquiredDesktopImage)
        {
            AcquiredDesktopImage->Release();
            AcquiredDesktopImage = NULL;
        }

        // QI for IDXGIResource
        hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&AcquiredDesktopImage));
        DesktopResource->Release();
        DesktopResource = NULL;
        if (FAILED(hr))
        {
            DisplayErr(L"Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER", L"Error", hr);
            return hr;
        }

        // Get metadata
        if (FrameInfo.TotalMetadataBufferSize)
        {
            // Old buffer too small
            if (FrameInfo.TotalMetadataBufferSize > MetaDataSize)
            {
                if (MetaDataBuffer)
                {
                    delete[] MetaDataBuffer;
                    MetaDataBuffer = NULL;
                }
                MetaDataBuffer = new (std::nothrow) BYTE[FrameInfo.TotalMetadataBufferSize];
                if (!MetaDataBuffer)
                {
                    DisplayErr(L"Failed to allocate memory for metadata in DUPLICATIONMANAGER", L"Error", E_OUTOFMEMORY);
                    MetaDataSize = 0;
                    Data->MoveCount = 0;
                    Data->DirtyCount = 0;
                    return E_OUTOFMEMORY;
                }
                MetaDataSize = FrameInfo.TotalMetadataBufferSize;
            }

            UINT BufSize = FrameInfo.TotalMetadataBufferSize;

            // Get move rectangles
            hr = DeskDupl->GetFrameMoveRects(BufSize, reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(MetaDataBuffer), &BufSize);
            if (FAILED(hr))
            {
                if (hr != DXGI_ERROR_ACCESS_LOST)
                {
                    DisplayErr(L"Failed to get frame move rects in DUPLICATIONMANAGER", L"Error", hr);
                }
                Data->MoveCount = 0;
                Data->DirtyCount = 0;
                return hr;
            }
            Data->MoveCount = BufSize / sizeof(DXGI_OUTDUPL_MOVE_RECT);

            BYTE* DirtyRects = MetaDataBuffer + BufSize;
            BufSize = FrameInfo.TotalMetadataBufferSize - BufSize;

            // Get dirty rectangles
            hr = DeskDupl->GetFrameDirtyRects(BufSize, reinterpret_cast<RECT*>(DirtyRects), &BufSize);
            if (FAILED(hr))
            {
                if (hr != DXGI_ERROR_ACCESS_LOST)
                {
                    DisplayErr(L"Failed to get frame dirty rects in DUPLICATIONMANAGER", L"Error", hr);
                }
                Data->MoveCount = 0;
                Data->DirtyCount = 0;
                return hr;
            }
            Data->DirtyCount = BufSize / sizeof(RECT);

            Data->MetaData = MetaDataBuffer;
        }

        Data->Frame = AcquiredDesktopImage;
        Data->FrameInfo = FrameInfo;

        return hr;
    }

    //
    // Release frame
    //
    void doneWithFrame()
    {
        HRESULT hr = S_OK;

        hr = DeskDupl->ReleaseFrame();
        if (FAILED(hr))
        {
            DisplayErr(L"Failed to release frame in DUPLICATIONMANAGER", L"Error", hr);
            return hr;
        }

        if (AcquiredDesktopImage)
        {
            AcquiredDesktopImage->Release();
            AcquiredDesktopImage = NULL;
        }

        return hr;
    }

    static std::shared_ptr<DXCapture> create(int x, int y, int awidth, int aheight)
    {
        // create a device
        // create an instance
        // initialize with device
        // return instance
    }
};