#pragma once

#include <stdint.h>

struct PixelArray
{
    uint8_t* fData=nullptr;

    size_t fWidth=0;
    size_t fHeight=0;
    ptrdiff_t fStride=0;
    bool fOwnData = false;


    PixelArray()
    {
        attach(nullptr, 0, 0, 0);
    }

    PixelArray(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        attach(d, w, h, s);
    }

    virtual ~PixelArray()
    {
        if (fOwnData)
            printf("Deallocate Pixel Array");
    }

    void attach(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        fData = (uint8_t*)d;
        fWidth = w;
        fHeight = h;

        fStride = s;
    }

    constexpr const int width() const noexcept { return fWidth; }
    constexpr const int height() const noexcept { return fHeight; }


    constexpr const ptrdiff_t stride() const noexcept { return fStride; }

    // Get a pointer to the raw data
    uint8_t * data() { return fData; }

    // Get a pointer to the beginning of a row
    uint8_t* rowPointer(const size_t y)
    {
        return (&fData[y * fStride]);
    }

};
