#pragma once
#include <stdint.h>

template <typename TP>
struct PixelAccessor
{
    uint8_t* fData = nullptr;

    size_t fWidth = 0;
    size_t fHeight = 0;
    ptrdiff_t fStride = 0;

    PixelAccessor() { ; }

    PixelAccessor(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        reset(d, w, h, s);
    }

    virtual ~PixelAccessor() { ; }

    // Connect to a specific source of data
    void reset(void* d=nullptr, const size_t w=0, const size_t h=0, const ptrdiff_t s=0)
    {
        fData = (uint8_t*)d;
        fWidth = w;
        fHeight = h;

        fStride = s;
    }

    constexpr const size_t width() const noexcept { return fWidth; }
    constexpr const size_t height() const noexcept { return fHeight; }

    // Get a pointer to the raw data
    uint8_t* data() noexcept { return fData; }              // this one allows for editing
    const uint8_t* data() const noexcept { return fData; }  // this one's not editable
    constexpr const ptrdiff_t stride() const noexcept { return fStride; }

    // Get a pointer to the beginning of a row
    uint8_t* rowPointer(const size_t y)
    {
        return (&fData[y * fStride]);
    }

    // Return a pointer to a specific pixel
    TP * pixelPointer(const size_t x, const size_t y)
    {
        return &((TP*)rowPointer(y))[x];
    }

    // Retrieve a single pixel
// This one does no bounds checking, so the behavior is undefined
// if the coordinates are beyond the boundary
    INLINE TP getPixel(const int x, const int y)
    {
        return *pixelPointer(x, y);
    }

    // Set a single pixel value
// Assume range checking has already occured
// Perform SRCCOPY operation on a pixel
    INLINE void setPixel(const int x, const int y, const TP& c)
    {
        pixelPointer(x,y)[0] = c;
    }

    // Set every pixel to a specified value
// we can use this fast intrinsic to fill
// the whole area
    virtual void setAllPixels(const TP & c)
    {
        // Could use memset_l if we check size
        // of TP == 4
        for (size_t row = 0; row < height(); row++)
            for (size_t col = 0; col < width(); col++)
                setPixel(col, row, c);
    }
};
