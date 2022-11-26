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

    And finally, if you want to get the capture as a BLImage
    you can use:

    ss.getImage()

    But, the ScreenSnapper is itself a PixelArray/PixelAccessor, so you get
    get at pixels and set pixels directly as well.

    References:
    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
*/


#include "User32PixelMap.h"


class ScreenSnapper : public User32PixelMap
{
    HDC fSourceDC=nullptr;
    int fOriginX = 0;
    int fOriginY = 0;
    BLImage fImage{};

public:
    ScreenSnapper()
    {
    }

    void reset(maths::bbox2f box, HDC srcDC = nullptr)
    {
        auto bsz = maths::size(box);
        reset((int)box.min.x, (int)box.min.y, (int)bsz.x, (int)bsz.y);
    }

    void reset(int x, int y, int w, int h, HDC srcDC=nullptr)
    {
        // First, delete current DC if it exists
        // But really, we don't know whether this is
        // the right thing to do.  Instead, the srcDC should
        // be a unique_ptr, so we don't have to make
        // the decision of its lifetime
        // For now, we'll just leave it alone, as we don't expect
        // this to be reset very frequently
        //if (nullptr != fSourceDC)
        //    ::DeleteDC(fSourceDC);

        if (srcDC == nullptr)
                fSourceDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
        else
                fSourceDC = srcDC;
        
        fOriginX = x;
        fOriginY = y;
        init(w, h);

        BLResult bResult = blImageInitAsFromData(&fImage, (int)width(), (int)height(), BL_FORMAT_PRGB32, data(), (intptr_t)stride(), nullptr, nullptr);
    }

    BLImage& getImage()
    {
        return fImage;
    }

    // take a snapshot
    bool next()
    {
        auto bResult = BitBlt(bitmapDC(), 0, 0, (int)width(), (int)height(),
            fSourceDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        if (bResult == 0)
        {
            auto err = ::GetLastError();
            printf("ScreenSnapper::next(), ERROR: 0x%x\n", err);
        }

        return (bResult != 0);
    }
};

