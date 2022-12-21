#pragma once


#include "definitions.h"
#include "textscan.h"

#include <cstdint>
#include <cstring>

namespace ndt
{
#ifdef __cplusplus
	extern "C" {
#endif

		//
		// A core type for representing a chunk of data in memory
		// The DataChunk is used in everything from networking
		// to graphics bitmaps to audio buffers.
		// Having a universal representation of a chunk of data
		// allows for easy interoperability between different
		// subsystems.  
		// It also allows us to eliminate disparate implementations that
		// are used for the same purpose.
		struct DataChunk
		{
			const uint8_t* fStart;
			const uint8_t* fEnd;

#ifdef __cplusplus
			INLINE uint8_t& operator[](size_t i);
			INLINE const uint8_t& operator[](size_t i) const;

			INLINE uint8_t& operator*();
			INLINE const uint8_t& operator*() const;

			INLINE DataChunk& operator+= (size_t a);

			INLINE DataChunk& operator++();				// prefix ++y
			INLINE DataChunk& operator++(int i);		// postfix y++
			INLINE DataChunk& operator--(int i);		// postfix y--
			
			INLINE explicit operator bool() const { return (fEnd - fStart) > 0; };
#endif
		};


		// implement ring buffer using queue
		// https://en.wikipedia.org/wiki/Circular_buffer

		struct circular_buff_t
		{
			uint8_t* fBuffer;
			size_t fSize;
			size_t fHead;
			size_t fTail;
		};


		static INLINE DataChunk make_chunk(const void* starting, const void* ending) noexcept;
		static INLINE DataChunk make_chunk_size(void* data, size_t sz) noexcept;
		static INLINE DataChunk make_chunk_cstr(const char* str) noexcept;
		static INLINE const uint8_t* data(DataChunk& dc) noexcept;
		static INLINE const uint8_t* begin(DataChunk& dc) noexcept;
		static INLINE const uint8_t* end(DataChunk& dc) noexcept;
		static INLINE size_t size(const DataChunk& dc) noexcept;
		static INLINE size_t copy(DataChunk& a, const DataChunk& b) noexcept;
		static INLINE size_t copy_to_cstr(char *str, size_t len, const DataChunk& a) noexcept;
		static INLINE int compare(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE int comparen(const DataChunk& a, const DataChunk& b, int n) noexcept;
		static INLINE int comparen_cstr(const DataChunk& a, const char * b, int n) noexcept;
		static INLINE bool is_equal(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE bool is_equal_cstr(const DataChunk& a, const char* s) noexcept;
		
		// Some utility functions for common operations
		static INLINE void chunkClear(DataChunk& dc) noexcept;
		static INLINE DataChunk& chunkSkip(DataChunk& dc, int n) noexcept;
		static INLINE DataChunk& chunkSkipToEnd(DataChunk& dc) noexcept;

		static INLINE DataChunk chunk_ltrim(const DataChunk& a, const charset& skippable) noexcept;
		static INLINE DataChunk chunk_rtrim(const DataChunk& a, const charset& skippable) noexcept;
		static INLINE DataChunk chunk_trim(const DataChunk& a, const charset& skippable) noexcept;
		
#ifdef __cplusplus
	}
#endif
	
#ifdef __cplusplus
	// Operator overloading
	static INLINE bool operator< (const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator==(const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator==(const DataChunk& a, const char* b) noexcept;
#endif


	}

	
namespace ndt
{

	
#ifdef __cplusplus
	extern "C" {
#endif
		
	// DataChunk routines
	static INLINE DataChunk make_chunk(const void* starting, const void* ending) noexcept { return { (const uint8_t*)starting, (const uint8_t*)ending }; }
	static INLINE DataChunk make_chunk_size(void* data, size_t sz) noexcept { return { (uint8_t*)data, (uint8_t*)data+sz }; }
	static INLINE DataChunk make_chunk_cstr(const char* data) noexcept { return { (uint8_t*)data, (uint8_t*)data + strlen(data) }; }
	

	static INLINE const uint8_t* begin(DataChunk& dc) noexcept { return dc.fStart; }
	static INLINE const uint8_t* end(DataChunk& dc) noexcept { return dc.fEnd; }
	
	static INLINE const uint8_t* data(DataChunk& dc)  noexcept { return dc.fStart; }
	static INLINE size_t size(const DataChunk& dc)  noexcept { return dc.fEnd - dc.fStart; }
	static INLINE size_t copy(DataChunk& a, const DataChunk& b) noexcept 
	{ 
		size_t maxBytes = size(a) < size(b) ? size(a) : size(b);
		memcpy((uint8_t *)a.fStart, b.fStart, maxBytes);
		return maxBytes;
	}
	static INLINE size_t copy_to_cstr(char* str, size_t len, const DataChunk& a) noexcept
	{
		size_t maxBytes = size(a) < len ? size(a) : len;
		memcpy(str, a.fStart, maxBytes);
		str[maxBytes] = 0;
		
		return maxBytes;
	}
	static INLINE int compare(const DataChunk& a, const DataChunk& b) noexcept
	{
		size_t maxBytes = size(a) < size(b) ? size(a) : size(b);
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static INLINE int comparen(const DataChunk &a, const DataChunk &b, int n) noexcept
	{
		size_t maxBytes = size(a) < size(b) ? size(a) : size(b);
		if (maxBytes > n)
			maxBytes = n;
		return memcmp(a.fStart, b.fStart, maxBytes);
	}
	
	static INLINE int comparen_cstr(const DataChunk &a, const char *b, int n) noexcept
	{
		size_t maxBytes = size(a) < n ? size(a) : n;
		return memcmp(a.fStart, b, maxBytes);
	}
	
	static INLINE bool is_equal(const DataChunk& a, const DataChunk& b) noexcept
	{
		if (size(a) != size(b))
			return false;
		return memcmp(a.fStart, b.fStart, size(a)) == 0;
	}

	static INLINE bool is_equal_cstr(const DataChunk &a, const char *cstr) noexcept
	{
		size_t len = strlen(cstr);
		if (size(a) != len)
			return false;
		return memcmp(a.fStart, cstr, len) == 0;
	}

	static INLINE void chunkClear(DataChunk& dc) noexcept
	{
		memset((uint8_t *)dc.fStart, 0, size(dc));
	}
	
	static INLINE DataChunk & chunkSkip(DataChunk &dc, int n) noexcept
	{
		if (n > size(dc))
			n = size(dc);
		dc.fStart += n;
		
		return dc;
	}
	
	static INLINE DataChunk& chunkSkipToEnd(DataChunk& dc) noexcept { dc.fStart = dc.fEnd; }
	/*
	static INLINE void skipOverCharset(DataChunk& dc, const charset& cs)
	{
		while (dc && cs.contains(*dc))
			dc++;
	}

	static INLINE void skipUntilCharset(DataChunk& dc, const charset& cs)
	{
		while (dc && !cs.contains(*dc))
			++dc;
	}
	*/



		
	// Trim the left side of skippable characters
    static INLINE DataChunk chunk_ltrim(const DataChunk& a, const charset & skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*start))
			++start;
		return { start, end };
	}

	// trim the right side of skippable characters
	static INLINE DataChunk chunk_rtrim(const DataChunk &a, const charset &skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*(end - 1)))
			--end;
		
		return { start, end };
	}

	// trim the left and right side of skippable characters
	static INLINE DataChunk chunk_trim(const DataChunk &a, const charset &skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*start))
			++start;
		while (start < end && skippable(*(end - 1)))
			--end;
		return { start, end };
	}
	
	// Given an input chunk
	// spit it into two chunks, the first chunk is the first token, the second chunk is the rest of the input
	// the token is defined by the charset
	
	static INLINE DataChunk chunk_token(DataChunk& a, const charset& delims) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* tokenEnd = start;
		while (tokenEnd < end && !delims(*tokenEnd))
			++tokenEnd;
		
		if (delims(*tokenEnd))
		{
			a.fStart = tokenEnd+1;
		}
		else {
			a.fStart = tokenEnd;
		}
		
		return { start, tokenEnd };
	}
	
	// Given an input chunk
	// find the first instance of a specified character
	// return the chunk, starting at that character
	// or an empty chunk
	static INLINE DataChunk chunk_find(const DataChunk& a, char c) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && *start != c)
			++start;
		
		//if (start == end)
		//	return { start, start };
		
		return { start, end };
	}
	
#ifdef __cplusplus
	}
#endif
}

namespace ndt
{
#ifdef __cplusplus
	
	INLINE uint8_t& DataChunk::operator[](size_t i) { return ((uint8_t *)fStart)[i]; }
	INLINE const uint8_t& DataChunk::operator[](size_t i) const { return ((uint8_t *)fStart)[i]; }
	
	INLINE uint8_t& DataChunk::operator*() { static uint8_t zero = 0;  if (fStart < fEnd) return *(uint8_t*)fStart; return  zero; }
	INLINE const uint8_t& DataChunk::operator*() const { static uint8_t zero = 0;  if (fStart < fEnd) return *(uint8_t*)fStart; return  zero; }

	INLINE DataChunk& DataChunk::operator++() { return chunkSkip(*this, 1); }			// prefix notation ++y
	INLINE DataChunk& DataChunk::operator++(int i) { return chunkSkip(*this, 1); }       // postfix notation y++
	INLINE DataChunk& DataChunk::operator--(int i) { return chunkSkip(*this, -1); }       // postfix notation y++

	INLINE DataChunk& DataChunk::operator+= (size_t n) { return chunkSkip(*this, n); }

	//INLINE explicit DataChunk::operator bool() const { return (fEnd - fStart) > 0; }
	
	static INLINE bool operator==(const DataChunk& a, const DataChunk& b) noexcept
	{
		if (size(a) != size(b))
			return false;
		return memcmp(a.fStart, b.fStart, size(a)) == 0;
	}
	
	static INLINE bool operator==(const DataChunk& a, const char* b) noexcept
	{
		size_t len = strlen(b);
		if (size(a) != len)
			return false;
		return memcmp(a.fStart, b, len) == 0;
	}

	static INLINE bool operator<(const DataChunk &a, const DataChunk&b) noexcept
	{
		size_t maxBytes = size(a) < size(b) ? size(a) : size(b);
		return memcmp(a.fStart, b.fStart, maxBytes) < 0;
	}
#endif
}

namespace ndt {
	static INLINE void circular_buff_reset(circular_buff_t& cb, uint8_t* buff, size_t sz)
	{
		cb.fBuffer = buff;
		cb.fSize = sz;
		cb.fHead = 0;
		cb.fTail = 0;
	}

	static INLINE bool circular_buff_empty(circular_buff_t& cb)
	{
		return cb.fHead == cb.fTail;
	}

	static INLINE bool circular_buff_full(circular_buff_t& cb)
	{
		return ((cb.fHead + 1) % cb.fSize) == cb.fTail;
	}

	static INLINE size_t circular_buff_capacity(circular_buff_t& cb)
	{
		return cb.fSize;
	}

	static INLINE size_t circular_buff_size(circular_buff_t& cb)
	{
		return (cb.fSize + cb.fHead - cb.fTail) % cb.fSize;
	}

	static INLINE int circular_buff_put(circular_buff_t& cb, uint8_t data) {
		int r = -1;

		if (!circular_buff_full(cb))
		{
			cb.fBuffer[cb.fHead] = data;
			cb.fHead = (cb.fHead + 1) % cb.fSize;
			r = 0;
		}
		return r;
	}

	static INLINE int circular_buff_get(circular_buff_t& cb, uint8_t& data)
	{
		int r = -1;
		if (!circular_buff_empty(cb))
		{
			data = cb.fBuffer[cb.fTail];
			cb.fTail = (cb.fTail + 1) % cb.fSize;
			r = 0;
		}
		return r;
	}
}