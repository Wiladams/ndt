#pragma once

#include "pixelaccessor.h"

template <typename T>
struct PixelBuffer : public PixelAccessor<T>
{
    PixelBuffer() { reset(); }
    PixelBuffer(void* d, const size_t w, const size_t h, const ptrdiff_t s)
    {
        attach(d, w, h, s);
    }

    virtual ~PixelBuffer() { ; }

};