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
    struct BStream
    {
        static constexpr int MSEEK_CUR = 1;
        static constexpr int MSEEK_END = 2;
        static constexpr int MSEEK_SET = 0;
        
        DataChunk fSource{};
        DataChunk fCursor{};
        bool fStreamIsLE{ true };
        
        BStream() = default;
        BStream(const DataChunk& inChunk)
            :fSource(inChunk)
        {
            fCursor = fSource;
        }
        BStream(const void* inData, const size_t sz)
            : fSource((const uint8_t *)inData, (const uint8_t *)inData+sz)
        {
            fCursor = fSource;
        }
        
        BStream& operator=(const BStream& other)
        {
			fSource = other.fSource;
			fCursor = other.fCursor;
			fStreamIsLE = other.fStreamIsLE;
			return *this;
        }
        
        // operator *
        // operator[]
        // operator++
        
		const uint8_t* data() const
		{
			return fCursor.fStart;
		}
        
		size_t size() const
		{
			return chunk_size(fSource);
		}

        size_t seek(ptrdiff_t offset, ptrdiff_t relativeTo = MSEEK_SET)
        {
			switch (relativeTo)
			{
			case MSEEK_CUR:
				fCursor.fStart = fCursor.fStart+offset;
				break;
			case MSEEK_END:
				fCursor.fStart = fSource.fEnd - offset;
				break;
			case MSEEK_SET:
				fCursor.fStart = fSource.fStart + offset;
				break;
			}
			
            // clamp to boundaries
            if (fCursor.fStart < fSource.fStart)
				fCursor.fStart = fSource.fStart;
			if (fCursor.fStart > fSource.fEnd)
				fCursor.fStart = fSource.fEnd;
            
            return fCursor.fStart - fSource.fStart;
		}
        
        size_t tell()
        {
			return fCursor.fStart - fSource.fStart;
        }

		size_t remaining()
		{
			return fSource.fEnd - fCursor.fStart;
		}

        // returns whether we're currently sitting at End Of File
        bool isEOF() const
        {
			return fCursor.fStart >= fSource.fEnd;
        }
        
        bool skip(ptrdiff_t n)  noexcept { fCursor += n; return true; }
        
        // Return a chunk that represents the bytes requested, 
        // up to the amount remaining
        DataChunk read(size_t sz)
        {
			size_t maxBytes = sz > remaining() ? remaining() : sz;
			DataChunk result = chunk_from_data_size((void *)fCursor.fStart, maxBytes);
            skip(maxBytes);

			return result;
        }
        
        // Copy the data out from the chunk
        // copy up to remaining() bytes
        // return the number of bytes copied
        size_t read_copy(void* outData, size_t sz)
        {
			size_t maxBytes = sz > remaining() ? remaining() : sz;
			memcpy(outData, fCursor.fStart, maxBytes);
            skip(maxBytes);
            
            return maxBytes;
        }
        
		// Read a single byte
        uint8_t read_u8()  noexcept 
        {
			uint8_t result = *fCursor.fStart;
			skip(1);

			return result;
        }    
        
		// Read a unsigned 16 bit value
        // assuming stream is in little endian format
        uint16_t read_u16_le() noexcept
        {
            uint16_t r = *(uint16_t*)fCursor.fStart;
            skip(2);
            
            return r;

            //return get_u8(dc) | (get_u8(dc) << 8);
        }

        // Read a unsigned 32-bit value
		// assuming stream is in little endian format
        uint32_t read_u32_le() noexcept
        {
            uint32_t r = *(uint32_t*)fCursor.fStart;
            skip(4);
            
            return r;

            //return get_u8(dc) | (get_u8(dc) << 8) | (get_u8(dc) << 16) | (get_u8(dc) << 24);
        }

		// Read a unsigned 64-bit value
		// assuming stream is in little endian format
        uint64_t read_u64_le() noexcept
        {
            uint64_t r = *(uint64_t*)fCursor.fStart;
            skip(8);
            
            return r;
            //return (uint64_t)get_u8(dc) | ((uint64_t)get_u8(dc) << 8) | ((uint64_t)get_u8(dc) << 16) | ((uint64_t)get_u8(dc) << 24) |
            //    ((uint64_t)get_u8(dc) << 32) | ((uint64_t)get_u8(dc) << 40) | ((uint64_t)get_u8(dc) << 48) | ((uint64_t)get_u8(dc) << 56);
        }

    };
}

/*
namespace ndt
{
    static INLINE uint8_t * tell_pointer(const DataCursor& dc) noexcept;

    static INLINE ptrdiff_t seek_to_begin(DataCursor& dc) noexcept;
    static INLINE ptrdiff_t seek_to_end(DataCursor& dc) noexcept;
    static INLINE ptrdiff_t seek(DataCursor& dc, ptrdiff_t pos) noexcept;
    static INLINE bool skip_to_alignment(DataCursor& dc, size_t num) noexcept;
    static INLINE bool skip_to_even(DataCursor& dc) noexcept;
    static INLINE const uint8_t* cursorPointer(DataCursor& dc) noexcept;

    static INLINE uint8_t peek_u8(const DataCursor& dc) noexcept;

    // Write in integer values
    static INLINE bool put_u8(DataCursor& dc, uint8_t a) noexcept;
    static INLINE bool put_u16_le(DataCursor& dc, uint16_t a) noexcept;
    static INLINE bool put_u32_le(DataCursor& dc, uint32_t a) noexcept;
    static INLINE bool put_u64_le(DataCursor& dc, uint64_t a) noexcept;
}

namespace ndt
{

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



    static INLINE DataChunk make_chunk_toend(const DataCursor& dc) noexcept
    {
		return make_chunk(dc.fCurrent, dc.fEnd);
    }
    
    // some operator overloading
    
    // Retrieve attributes of the cursor

    static INLINE ptrdiff_t remaining(const DataCursor& dc) noexcept { return dc.fEnd - dc.fCurrent; }

    static INLINE uint8_t* tell_pointer(const DataCursor& dc) noexcept { return dc.fCurrent; }

    static INLINE ptrdiff_t seek_to_begin(DataCursor& dc)  noexcept { dc.fCurrent = (uint8_t *)dc.fStart; return tell(dc); }
    static INLINE ptrdiff_t seek_to_end(DataCursor& dc)  noexcept { dc.fCurrent = (uint8_t*)dc.fEnd; return tell(dc); }
    static INLINE ptrdiff_t seek(DataCursor& dc, ptrdiff_t pos)  noexcept { dc.fCurrent = (uint8_t*)dc.fStart + pos; return tell(dc); }

    
    // Seek forwad to a boundary of the specified
    // number of bytes.
    static INLINE bool skip_to_alignment(DataCursor& dc, size_t num)  noexcept { return skip(dc, tell(dc) % num); }

    // Skip to the next even numbered offset
    static INLINE bool skipToEven(DataCursor& dc)  noexcept { return skip_to_alignment(dc, 2); }

    // Return a pointer to the current position
    static INLINE const uint8_t * cursorPointer(DataCursor& dc)  noexcept { return dc.fCurrent; }

    static INLINE uint8_t peek_u8(const DataCursor& dc)  noexcept { return *dc.fCurrent; }




    static INLINE bool put_u8(DataCursor& dc, uint8_t a)  noexcept { if (isEOF(dc))return false; *dc.fCurrent = a; dc.fCurrent++;  return true; }
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

	//INLINE operator bool(const DataCursor& dc) { return remaining(dc)>0; }
    INLINE const uint8_t operator*(const DataCursor& dc)  noexcept { return *dc.fCurrent; }
    // INLINE uint8_t & operator*(DataCursor& dc)  noexcept { return dc.fCurrent; }
    INLINE void DataCursor::operator++() { if (fCurrent < fEnd) fCurrent++; }     // prefix notation ++y
	INLINE void DataCursor::operator++(int i) { if (fCurrent < fEnd) fCurrent++; }                      // postfix notation y++
    INLINE DataCursor& operator+=(DataCursor& a, const int b) { skip(a, b); return a; }
    INLINE uint8_t& DataCursor::operator[](size_t i) { return fCurrent[i]; }
    INLINE const uint8_t& DataCursor::operator[](size_t i) const { return fCurrent[i]; }
    
}

*/
