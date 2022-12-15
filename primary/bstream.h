#pragma once

//
// bstream
// This provides a 'stream' like interface to a chunk of memory.
// The idea is, if you have a piece of memory, and you need to 
// serialize and deserialize things into and out of it, you can 
// use this convenience.
// 
// This pattern is seen all the time when reading binary data from
// files, or network buffers.  It's used for image loading, protocol
// parsing, interop between interfaces, etc.
// 
// The functions here are done as straight C instead of C++ classes
// to provide simple and fast implementation without any dependencies.
//
// The functions here do minimal boundary checking.  They are meant
// to be fairly low level, so they will be safe and not run over
// the ends of buffers, but your values will be invalid if
// you don't first check for the amount of space available before 
// you make your calls.
// 
// This puts the burden of safety checks in the hands of the user,
// so they can determine which course of action to take.
// 
// The routines here are all inline, noexcept, so they should be 
// safe to use in a lot of places without incurring additional cost.
//



#include "definitions.h"
#include "datachunk.h"


namespace ndt
{
#ifdef __cplusplus
    extern "C" {
#endif
    //
    // A core type for representing a chunk of data in a stream
    // other routines are meant to operate on this data type
    struct DataCursor : public DataChunk
    {
        //const uint8_t* fStart;
        //const uint8_t* fEnd;
        uint8_t* fCurrent;

#ifdef __cplusplus
        INLINE uint8_t& operator[](size_t i);
        INLINE const uint8_t& operator[](size_t i) const;
        INLINE void operator++();
		INLINE void operator++(int i);
        INLINE operator bool() { return (fEnd-fCurrent)> 0; }
#endif
    };


    
    static INLINE DataCursor make_cursor_chunk(const DataChunk& chunk);
    static INLINE DataCursor make_cursor(const void* starting, const void* ending) noexcept;
    static INLINE DataCursor make_cursor_size(const void* starting, size_t sz) noexcept;
    static INLINE DataCursor make_cursor_range(const DataCursor& dc, size_t pos, size_t sz)noexcept;    // sub-range
    static INLINE DataCursor make_cursor_range_size(const DataCursor& dc, size_t sz)noexcept;           // sub-range
    static INLINE DataChunk make_chunk_to_end(const DataCursor& dc) noexcept;

    static INLINE ptrdiff_t remaining(const DataCursor& dc) noexcept;
    static INLINE ptrdiff_t tell(const DataCursor& dc) noexcept;
    static INLINE uint8_t * tell_pointer(const DataCursor& dc) noexcept;

    static INLINE ptrdiff_t seek_to_begin(DataCursor& dc) noexcept;
    static INLINE ptrdiff_t seek_to_end(DataCursor& dc) noexcept;
    static INLINE ptrdiff_t seek(DataCursor& dc, ptrdiff_t pos) noexcept;
    static INLINE bool skip(DataCursor& dc, ptrdiff_t n) noexcept;
    static INLINE bool skip_to_alignment(DataCursor& dc, size_t num) noexcept;
    static INLINE bool skip_to_even(DataCursor& dc) noexcept;
    static INLINE const uint8_t* cursorPointer(DataCursor& dc) noexcept;


    static INLINE bool isEOF(DataCursor& dc) noexcept;
    static INLINE uint8_t peek_u8(const DataCursor& dc) noexcept;

    // Read out integer values
    static INLINE uint8_t get_u8(DataCursor& dc) noexcept;
    static INLINE uint16_t get_u16_le(DataCursor& dc) noexcept;
    static INLINE uint32_t get_u32_le(DataCursor& dc) noexcept;
    static INLINE uint64_t get_u64_le(DataCursor& dc) noexcept;

    // Write in integer values
    static INLINE bool put_u8(DataCursor& dc, uint8_t a) noexcept;
    static INLINE bool put_u16_le(DataCursor& dc, uint16_t a) noexcept;
    static INLINE bool put_u32_le(DataCursor& dc, uint32_t a) noexcept;
    static INLINE bool put_u64_le(DataCursor& dc, uint64_t a) noexcept;

#ifdef __cplusplus
    }
#endif
}

namespace ndt
{
    static INLINE DataCursor make_cursor_chunk(const DataChunk& chunk)
	{
        DataCursor dc{};
		dc.fStart = chunk.fStart;
		dc.fEnd = chunk.fEnd;
		dc.fCurrent = (uint8_t *)chunk.fStart;
        
		return dc;
	}

    
    static INLINE DataCursor make_cursor(const void* starting, const void* ending) noexcept
    {
        DataCursor c{ (const uint8_t*)starting, (const uint8_t*)ending, (uint8_t*)starting };
        return c;
    }
    static INLINE DataCursor make_cursor_size(const void* starting, size_t sz)  noexcept { return make_cursor(starting, (uint8_t*)starting + sz); }

    // get a subrange of the memory stream
    // returning a new memory stream
    // This is very useful for getting a subset of a data stream
    // with bounds checking.  You must be mindful of the lifetime
    // of the data pointed to though.  
    // There is no provision for 
    // ensuring the data remains valid outside the context of the 
    // original stream.  but, since the data pointer was passed in 
    // from the beginning, that guarantee is already absent.
    static INLINE DataCursor make_cursor_range(const DataCursor &dc, size_t pos, size_t sz) noexcept
    {
        auto dcsize = size(dc);

        if (sz==0 || dcsize == 0)
        {
            return DataCursor{};
        }

        // desired position is beyond end of data
        if (pos >= dcsize) {
            return DataCursor{};
        }

        // The size asked for is greater than the amount remaining
        if ((sz > (dcsize - pos))) {
            return DataCursor{};
        }

        return make_cursor_size(dc.fStart + pos, sz);
    }

    // A convenience to return a range from our current position
    static INLINE DataCursor make_cursor_range_size(DataCursor& dc, size_t sz) { return make_cursor_range(dc, tell(dc), sz); }

    static INLINE DataChunk make_chunk_toend(const DataCursor& dc) noexcept
    {
		return make_chunk(dc.fCurrent, dc.fEnd);
    }
    
    // some operator overloading
    
    // Retrieve attributes of the cursor

    static INLINE ptrdiff_t remaining(const DataCursor& dc) noexcept { return dc.fEnd - dc.fCurrent; }
    static INLINE ptrdiff_t tell(const DataCursor& dc)  noexcept { return dc.fCurrent - dc.fStart; }
    static INLINE uint8_t* tell_pointer(const DataCursor& dc) noexcept { return dc.fCurrent; }

    static INLINE ptrdiff_t seek_to_begin(DataCursor& dc)  noexcept { dc.fCurrent = (uint8_t *)dc.fStart; return tell(dc); }
    static INLINE ptrdiff_t seek_to_end(DataCursor& dc)  noexcept { dc.fCurrent = (uint8_t*)dc.fEnd; return tell(dc); }
    static INLINE ptrdiff_t seek(DataCursor& dc, ptrdiff_t pos)  noexcept { dc.fCurrent = (uint8_t*)dc.fStart + pos; return tell(dc); }
    static INLINE bool skip(DataCursor& dc, ptrdiff_t n)  noexcept { dc.fCurrent += n; return true; }
    
    // Seek forwad to a boundary of the specified
    // number of bytes.
    static INLINE bool skip_to_alignment(DataCursor& dc, size_t num)  noexcept { return skip(dc, tell(dc) % num); }

    // Skip to the next even numbered offset
    static INLINE bool skipToEven(DataCursor& dc)  noexcept { return skip_to_alignment(dc, 2); }

    // Return a pointer to the current position
    static INLINE const uint8_t * cursorPointer(DataCursor& dc)  noexcept { return dc.fCurrent; }


    static INLINE bool isEOF(DataCursor& dc)  noexcept { return remaining(dc) < 1; }
    static INLINE uint8_t peek_u8(const DataCursor& dc)  noexcept { return *dc.fCurrent; }
    static INLINE uint8_t get_u8(DataCursor& dc)  noexcept { return dc.fCurrent < dc.fEnd ? *dc.fCurrent++ : ~0; }    // can't distinguish between valid and EOF
    static INLINE uint16_t get_u16_le(DataCursor& dc) noexcept
    {
        uint16_t r = *(uint16_t*)dc.fCurrent;
        skip(dc, 2);
        return r;

        //return get_u8(dc) | (get_u8(dc) << 8);
    }

    static INLINE uint32_t get_u32_le(DataCursor& dc) noexcept
    {
        uint32_t r = *(uint32_t*)dc.fCurrent;
        skip(dc, 4);
        return r;

        //return get_u8(dc) | (get_u8(dc) << 8) | (get_u8(dc) << 16) | (get_u8(dc) << 24);
    }

    static INLINE uint64_t get_u64_le(DataCursor& dc) noexcept
    {
        uint64_t r = *(uint64_t*)dc.fCurrent;
        skip(dc, 8);
        return r;
        //return (uint64_t)get_u8(dc) | ((uint64_t)get_u8(dc) << 8) | ((uint64_t)get_u8(dc) << 16) | ((uint64_t)get_u8(dc) << 24) |
        //    ((uint64_t)get_u8(dc) << 32) | ((uint64_t)get_u8(dc) << 40) | ((uint64_t)get_u8(dc) << 48) | ((uint64_t)get_u8(dc) << 56);
    }



    static INLINE bool put_u8(DataCursor& dc, uint8_t a)  noexcept { if (isEOF(dc))return false; *dc.fCurrent++ = a; return true; }
    static INLINE bool put_u16_le(DataCursor& dc, uint16_t a)  noexcept
    {

        if (remaining(dc) < sizeof(a))
            return false;

        if (isLE())
        {
            *(uint16_t*)dc.fCurrent = a;
            skip(dc, 2);
        } else {

            uint8_t* bytes = (uint8_t*)&a;

            for (size_t i = 0; i < sizeof(a); i++)
                *dc.fCurrent++ = *bytes++;
        }

        //put_u8(dc, a & 0xff);
        //put_u8(dc, (a & 0xff00) >> 8);

        return true;
    }
    
    static INLINE bool put_u32_le(DataCursor& dc, uint32_t a)  noexcept
    {
        if (remaining(dc) < sizeof(a))
            return false;

        if (isLE())
        {
            *(uint32_t*)dc.fCurrent = a;
            skip(dc, 4);
        } else {
            uint8_t* bytes = (uint8_t*)&a;

            for (size_t i = 0; i < sizeof(a); i++)
                *dc.fCurrent++ = *bytes++;
        }

        //put_u8(dc, a & 0xff);
        //put_u8(dc, (a & 0xff00) >> 8);
        //put_u8(dc, (a & 0xff0000) >> 16);
        //put_u8(dc, (a & 0xff000000) >> 24);

        return true;
    }
    
    static INLINE bool put_u64_le(DataCursor& dc, uint64_t a)  noexcept
    {
        if (remaining(dc) < sizeof(a))
            return false;

        if (isLE())
        {
            *(uint64_t*)dc.fCurrent = a;
            skip(dc, 8);
        } else {
            uint8_t* bytes = (uint8_t*)&a;

            for (size_t i = 0; i < sizeof(a); i++)
                *dc.fCurrent++ = *bytes++;
        }

        //put_u8(dc, a & 0xff);
        //put_u8(dc, (a & 0xff00) >> 8);
        //put_u8(dc, (a & 0xff0000) >> 16);
       // put_u8(dc, (a & 0xff000000) >> 24);
        //put_u8(dc, (a & 0xff00000000) >> 32);
        //put_u8(dc, (a & 0xff0000000000) >> 40);
        //put_u8(dc, (a & 0xff000000000000) >> 48);
        //put_u8(dc, (a & 0xff00000000000000) >> 56);

        return true;
    }
}

namespace ndt
{
#ifdef __cplusplus
	//INLINE operator bool(const DataCursor& dc) { return remaining(dc)>0; }
    INLINE const uint8_t operator*(const DataCursor& dc)  noexcept { return *dc.fCurrent; }
    // INLINE uint8_t & operator*(DataCursor& dc)  noexcept { return dc.fCurrent; }
    INLINE void DataCursor::operator++() { if (fCurrent < fEnd) fCurrent++; }     // prefix notation ++y
	INLINE void DataCursor::operator++(int i) { if (fCurrent < fEnd) fCurrent++; }                      // postfix notation y++
    INLINE DataCursor& operator+=(DataCursor& a, const int b) { skip(a, b); return a; }
    INLINE uint8_t& DataCursor::operator[](size_t i) { return fCurrent[i]; }
    INLINE const uint8_t& DataCursor::operator[](size_t i) const { return fCurrent[i]; }
    
#endif
}

