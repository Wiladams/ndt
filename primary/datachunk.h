#pragma once


#include "definitions.h"


#include <cstdint>
#include <cstring>
#include <iterator>	// for std::data(), std::size()

namespace ndt
{
#ifdef __cplusplus
	extern "C" {
#endif

		//
		// A core type for representing a contiguous sequence of bytes
		// As of C++ 20, there is std::span, but it is not yet widely supported
		// 
		// The DataChunk is used in everything from networking
		// to graphics bitmaps to audio buffers.
		// Having a universal representation of a chunk of data
		// allows for easy interoperability between different
		// subsystems.  
		// 
		// It also allows us to eliminate disparate implementations that
		// are used for the same purpose.
		struct DataChunk
		{
			const uint8_t* fStart;
			const uint8_t* fEnd;

#ifdef __cplusplus

			
			DataChunk(const uint8_t* start, const uint8_t* end) : fStart(start), fEnd(end) {}
			DataChunk() : fStart(nullptr), fEnd(nullptr) {}

			
			INLINE uint8_t& operator[](size_t i);				// Array access
			INLINE const uint8_t& operator[](size_t i) const;	// Array access

			INLINE uint8_t& operator*();				// get current byte value
			INLINE const uint8_t& operator*() const;	// get current byte value

			INLINE DataChunk& operator+= (size_t a);	// advance by the specified amount

			INLINE DataChunk& operator++();				// prefix ++y
			INLINE DataChunk& operator++(int i);		// postfix y++
			INLINE DataChunk& operator--(int i);		// postfix y--
			
			INLINE explicit operator bool() const { return (fEnd - fStart) > 0; };
#endif
		};





		static INLINE DataChunk make_chunk(const void* starting, const void* ending) noexcept;

		static INLINE DataChunk chunk_from_data_size(void* data, size_t sz) noexcept;
		static INLINE DataChunk chunk_from_cstr(const char* str) noexcept;
		
		static INLINE const uint8_t* data(DataChunk& dc) noexcept;
		static INLINE const uint8_t* begin(DataChunk& dc) noexcept;
		static INLINE const uint8_t* end(DataChunk& dc) noexcept;
		//static INLINE size_t size(const DataChunk& dc) noexcept;
		static INLINE size_t chunk_size(const DataChunk& dc) noexcept;
		static INLINE bool chunk_empty(const DataChunk& dc) noexcept;
		static INLINE size_t copy(DataChunk& a, const DataChunk& b) noexcept;
		static INLINE size_t copy_to_cstr(char *str, size_t len, const DataChunk& a) noexcept;
		static INLINE int compare(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE int comparen(const DataChunk& a, const DataChunk& b, int n) noexcept;
		static INLINE int comparen_cstr(const DataChunk& a, const char * b, int n) noexcept;
		static INLINE bool chunk_is_equal(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE bool chunk_is_equal_cstr(const DataChunk& a, const char* s) noexcept;
		
		// Some utility functions for common operations
		static INLINE void chunk_clear(DataChunk& dc) noexcept;
		static INLINE void chunk_truncate(DataChunk& dc) noexcept;
		static INLINE DataChunk& chunk_skip(DataChunk& dc, int n) noexcept;
		static INLINE DataChunk& chunk_skip_to_end(DataChunk& dc) noexcept;


		
#ifdef __cplusplus
	}
#endif
	
#ifdef __cplusplus
	//
	// operators for comparison
	// operator!=;
	// operator<=;
	// operator>=;
	static INLINE bool operator==(const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator==(const DataChunk& a, const char* b) noexcept;
	static INLINE bool operator< (const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator> (const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator!=(const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator<=(const DataChunk& a, const DataChunk& b) noexcept;
	static INLINE bool operator>=(const DataChunk& a, const DataChunk& b) noexcept;
	
#endif


	}

	
namespace ndt
{

	
#ifdef __cplusplus
	extern "C" {
#endif
		
	// DataChunk routines
	static INLINE DataChunk make_chunk(const void* starting, const void* ending) noexcept { return { (const uint8_t*)starting, (const uint8_t*)ending }; }
	//static INLINE DataChunk make_chunk_size(void* data, size_t sz) noexcept { return { (uint8_t*)data, (uint8_t*)data+sz }; }
	//static INLINE DataChunk make_chunk_cstr(const char* data) noexcept { return { (uint8_t*)data, (uint8_t*)data + strlen(data) }; }
	static INLINE DataChunk chunk_from_data_size(void* data, size_t sz) noexcept { return { (uint8_t*)data, (uint8_t*)data + sz }; }
	static INLINE DataChunk chunk_from_cstr(const char* data) noexcept { return { (uint8_t*)data, (uint8_t*)data + strlen(data) }; }

	static INLINE const uint8_t* begin(DataChunk& dc) noexcept { return dc.fStart; }
	static INLINE const uint8_t* end(DataChunk& dc) noexcept { return dc.fEnd; }
	
	static INLINE const uint8_t* data(DataChunk& dc)  noexcept { return dc.fStart; }
	//static INLINE size_t size(const DataChunk& dc)  noexcept { return dc.fEnd - dc.fStart; }
	static INLINE size_t chunk_size(const DataChunk& dc)  noexcept { return dc.fEnd - dc.fStart; }
	static INLINE bool chunk_empty(const DataChunk& dc)  noexcept { return dc.fEnd == dc.fStart; }
	static INLINE size_t copy(DataChunk& a, const DataChunk& b) noexcept 
	{ 
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		memcpy((uint8_t *)a.fStart, b.fStart, maxBytes);
		return maxBytes;
	}

	static INLINE int compare(const DataChunk& a, const DataChunk& b) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static INLINE int comparen(const DataChunk &a, const DataChunk &b, int n) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		if (maxBytes > n)
			maxBytes = n;
		return memcmp(a.fStart, b.fStart, maxBytes);
	}
	
	static INLINE int comparen_cstr(const DataChunk &a, const char *b, int n) noexcept
	{
		size_t maxBytes = chunk_size(a) < n ? chunk_size(a) : n;
		return memcmp(a.fStart, b, maxBytes);
	}
	
	static INLINE bool chunk_is_equal(const DataChunk& a, const DataChunk& b) noexcept
	{
		if (chunk_size(a) != chunk_size(b))
			return false;
		return memcmp(a.fStart, b.fStart, chunk_size(a)) == 0;
	}

	static INLINE bool chunk_is_equal_cstr(const DataChunk &a, const char *cstr) noexcept
	{
		size_t len = strlen(cstr);
		if (chunk_size(a) != len)
			return false;
		return memcmp(a.fStart, cstr, len) == 0;
	}

	static INLINE void chunk_clear(DataChunk& dc) noexcept
	{
		memset((uint8_t *)dc.fStart, 0, chunk_size(dc));
	}
	
	static INLINE void chunk_truncate(DataChunk& dc) noexcept
	{
		dc.fEnd = dc.fStart;
	}
	
	static INLINE DataChunk & chunk_skip(DataChunk &dc, int n) noexcept
	{
		if (n > chunk_size(dc))
			n = chunk_size(dc);
		dc.fStart += n;
		
		return dc;
	}
	
	static INLINE DataChunk& chunk_skip_to_end(DataChunk& dc) noexcept { dc.fStart = dc.fEnd; }

	
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

	INLINE DataChunk& DataChunk::operator++() { return chunk_skip(*this, 1); }			// prefix notation ++y
	INLINE DataChunk& DataChunk::operator++(int i) { return chunk_skip(*this, 1); }       // postfix notation y++
	INLINE DataChunk& DataChunk::operator--(int i) { return chunk_skip(*this, -1); }       // postfix notation y++

	INLINE DataChunk& DataChunk::operator+= (size_t n) { return chunk_skip(*this, n); }

	//INLINE explicit DataChunk::operator bool() const { return (fEnd - fStart) > 0; }
	
	static INLINE bool operator==(const DataChunk& a, const DataChunk& b) noexcept
	{
		if (chunk_size(a) != chunk_size(b))
			return false;
		return memcmp(a.fStart, b.fStart, chunk_size(a)) == 0;
	}
	
	static INLINE bool operator==(const DataChunk& a, const char* b) noexcept
	{
		size_t len = strlen(b);
		if (chunk_size(a) != len)
			return false;
		return memcmp(a.fStart, b, len) == 0;
	}

	static INLINE bool operator!=(const DataChunk& a, const DataChunk& b) noexcept
	{
		if (chunk_size(a) != chunk_size(b))
			return true;
		return memcmp(a.fStart, b.fStart, chunk_size(a)) != 0;
	}
	
	static INLINE bool operator<(const DataChunk &a, const DataChunk&b) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		return memcmp(a.fStart, b.fStart, maxBytes) < 0;
	}

	static INLINE bool operator>(const DataChunk& a, const DataChunk& b) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		return memcmp(a.fStart, b.fStart, maxBytes) > 0;
	}
	
	static INLINE bool operator<=(const DataChunk &a, const DataChunk &b) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		return memcmp(a.fStart, b.fStart, maxBytes) <= 0;
	}
	
	static INLINE bool operator>=(const DataChunk& a, const DataChunk& b) noexcept
	{
		size_t maxBytes = chunk_size(a) < chunk_size(b) ? chunk_size(a) : chunk_size(b);
		return memcmp(a.fStart, b.fStart, maxBytes) >= 0;
	}
	
#endif
}

