#pragma once

#include <stdint.h>


struct PixelAccessor
{
    uint8_t* fData=nullptr;

    size_t fWidth=0;
    size_t fHeight=0;
    ptrdiff_t fStride=0;

    PixelAccessor()
    {
        attach(nullptr, 0, 0, 0);
    }

    virtual ~PixelAccessor(){;}

    void reset()
    {
        fData = nullptr;
        fWidth = 0;
        fHeight = 0;
        fStride = 0;
    }

    // Connect to a specific source of data
    void attach(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        fData = (uint8_t*)d;
        fWidth = w;
        fHeight = h;

        fStride = s;
    }

    constexpr const size_t width() const noexcept { return fWidth; }
    constexpr const size_t height() const noexcept { return fHeight; }


    constexpr const ptrdiff_t stride() const noexcept { return fStride; }

    // Get a pointer to the raw data
    uint8_t * data() { return fData; }

    // Get a pointer to the beginning of a row
    uint8_t* rowPointer(const size_t y)
    {
        return (&fData[y * fStride]);
    }

};

template <typename T>
struct PixelBuffer : public PixelAccessor
{
    PixelBuffer() { reset(); }
    PixelBuffer(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        attach(d, w, h, s);
    }

    virtual ~PixelBuffer() { ; }

};