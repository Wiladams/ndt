#pragma once

#include "datachunk.h"
#include "textscan.h"

namespace ndt
{
	static ndt::charset wspChars(" \r\n\t\f\v");
	
#ifdef __cplusplus
	extern "C" {
#endif
		
		static INLINE size_t copy_to_cstr(char* str, size_t len, const DataChunk& a) noexcept;
		static INLINE DataChunk chunk_ltrim(const DataChunk& a, const charset& skippable) noexcept;
		static INLINE DataChunk chunk_rtrim(const DataChunk& a, const charset& skippable) noexcept;
		static INLINE DataChunk chunk_trim(const DataChunk& a, const charset& skippable) noexcept;
		static INLINE DataChunk chunk_skip_wsp(const DataChunk& a) noexcept;
		
		static INLINE DataChunk chunk_subchunk(const DataChunk& a, const size_t start, const size_t sz) noexcept;
		static INLINE bool chunk_starts_with(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE bool chunk_starts_with_char(const DataChunk& a, const uint8_t b) noexcept;
		static INLINE bool chunk_starts_with_cstr(const DataChunk& a, const char* b) noexcept;
		
		static INLINE bool chunk_ends_with(const DataChunk& a, const DataChunk& b) noexcept;
		static INLINE bool chunk_ends_with_char(const DataChunk& a, const uint8_t b) noexcept;
		static INLINE bool chunk_ends_with_cstr(const DataChunk& a, const char* b) noexcept;
		
		static INLINE DataChunk chunk_token(DataChunk& a, const charset& delims) noexcept;
		static INLINE DataChunk chunk_find_char(const DataChunk& a, char c) noexcept;

		// Number Conversions
		static INLINE double chunk_to_double(DataChunk& inChunk) noexcept;


		
#ifdef __cplusplus
	}


#endif
}



namespace ndt 
{
#ifdef __cplusplus
	extern "C" {
#endif

		
		static INLINE size_t copy_to_cstr(char* str, size_t len, const DataChunk& a) noexcept
		{
			size_t maxBytes = size(a) < len ? size(a) : len;
			memcpy(str, a.fStart, maxBytes);
			str[maxBytes] = 0;

			return maxBytes;
		}
		
		// Trim the left side of skippable characters
		static INLINE DataChunk chunk_ltrim(const DataChunk& a, const charset& skippable) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			while (start < end && skippable(*start))
				++start;
			return { start, end };
		}

		// trim the right side of skippable characters
		static INLINE DataChunk chunk_rtrim(const DataChunk& a, const charset& skippable) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			while (start < end && skippable(*(end - 1)))
				--end;

			return { start, end };
		}

		// trim the left and right side of skippable characters
		static INLINE DataChunk chunk_trim(const DataChunk& a, const charset& skippable) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			while (start < end && skippable(*start))
				++start;
			while (start < end && skippable(*(end - 1)))
				--end;
			return { start, end };
		}
		
		static INLINE DataChunk chunk_skip_wsp(const DataChunk& a) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			while (start < end && wspChars(*start))
				++start;
			return { start, end };
		}
		
		static INLINE DataChunk chunk_subchunk(const DataChunk& a, const size_t startAt, const size_t sz) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			if (startAt < size(a))
			{
				start += startAt;
				if (start + sz < end)
					end = start + sz;
				else
					end = a.fEnd;
			} else
			{
				start = end;
			}
			return { start, end };
		}
		
		static INLINE bool chunk_starts_with(const DataChunk& a, const DataChunk& b) noexcept
		{
			return chunk_is_equal(chunk_subchunk(a, 0, size(b)), b);
		}
		
		static INLINE bool chunk_starts_with_char(const DataChunk& a, const uint8_t b) noexcept
		{
			return size(a) > 0 && a.fStart[0] == b;
		}

		static INLINE bool chunk_starts_with_cstr(const DataChunk& a, const char* b) noexcept
		{
			return chunk_starts_with(a, chunk_from_cstr(b));
		}
		
		static INLINE bool chunk_ends_with(const DataChunk& a, const DataChunk& b) noexcept
		{
			return chunk_is_equal(chunk_subchunk(a, size(a)-size(b), size(b)), b);
		}
		
		static INLINE bool chunk_ends_with_char(const DataChunk& a, const uint8_t b) noexcept
		{
			return size(a) > 0 && a.fEnd[-1] == b;
		}
		
		static INLINE bool chunk_ends_with_cstr(const DataChunk& a, const char* b) noexcept
		{
			return chunk_ends_with(a, chunk_from_cstr(b));
		}
		
		// Given an input chunk
		// spit it into two chunks, 
		// Returns - the first chunk before delimeters
		// a - adjusted to reflect the rest of the input after delims
		static INLINE DataChunk chunk_token(DataChunk& a, const charset& delims) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			const uint8_t* tokenEnd = start;
			while (tokenEnd < end && !delims(*tokenEnd))
				++tokenEnd;

			if (delims(*tokenEnd))
			{
				a.fStart = tokenEnd + 1;
			}
			else {
				a.fStart = tokenEnd;
			}

			return { start, tokenEnd };
		}
		

		
		// Given an input chunk
		// find the first instance of a specified character
		// return the chunk preceding the found character
		// or or the whole chunk of the character is not found
		static INLINE DataChunk chunk_find_char(const DataChunk& a, char c) noexcept
		{
			const uint8_t* start = a.fStart;
			const uint8_t* end = a.fEnd;
			while (start < end && *start != c)
				++start;

			return { start, end };
		}
		
		// Take a chunk containing a series of digits and turn
		// it into a 64-bit unsigned integer
		// Stop processing when the first non-digit is seen, 
		// or the end of the chunk
		static INLINE uint64_t chunk_to_u64(DataChunk& s)
		{
			static charset digitChars("0123456789");
			
			uint64_t v = 0;
			
			while (s && digitChars(*s))
			{
				v = v * 10 + (uint64_t)(*s - '0');
				s++;
			}
			
			return v;
		}
		
		static INLINE int64_t chunk_to_i64(DataChunk& s)
		{
			static charset digitChars("0123456789");

			int64_t v = 0;

			bool negative = false;
			if (s && *s == '-')
			{
				negative = true;
				s++;
			}

			while (s && digitChars(*s))
			{
				v = v * 10 + (int64_t)(*s - '0');
				s++;
			}

			if (negative)
				v = -v;

			return v;
		}
		
		// parse floating point number
		// includes sign, exponent, and decimal point
		// The input chunk is altered, with the fStart pointer moved to the end of the number
		static INLINE double chunk_to_double(DataChunk& s) noexcept
		{
			static charset digitChars("0123456789");
			
			double sign = 1.0;

			double res = 0.0;
			long long intPart = 0, fracPart = 0;
			bool hasIntPart = false;
			bool hasFracPart = false;

			// Parse optional sign
			if (*s == '+') {
				s++;
			}
			else if (*s == '-') {
				sign = -1;
				s++;
			}
			
			// Parse integer part
			if (digitChars[*s]) {

				intPart = chunk_to_u64(s);

				res = (double)intPart;
				hasIntPart = true;
			}

			// Parse fractional part.
			if (*s == '.') {
				s++; // Skip '.'
				auto sentinel = s.fStart;
				
				if (digitChars(*s)) {
					fracPart = chunk_to_u64(s);	
					auto ending = s.fStart;
					
					res += (double)fracPart / pow(10.0, (double)(ending - sentinel));
					hasFracPart = true;
				}
			}

			// A valid number should have integer or fractional part.
			if (!hasIntPart && !hasFracPart)
				return 0.0;


			// Parse optional exponent
			if (*s == 'e' || *s == 'E') {
				long long expPart = 0;
				s++; // skip 'E'
				
				double expSign = 1.0;
				if (*s == '+') {
					s++;
				}
				else if (*s == '-') {
					expSign = -1.0;
					s++;
				}
				
				if (digitChars[*s]) {
					expPart = chunk_to_u64(s);
					res *= pow(10.0, expSign * (double)expPart);
				}
			}

			return res * sign;
		}
		
		
#ifdef __cplusplus
	}
#endif
		
}

// a ciyoke if utility routines to help with debugging
namespace ndt {
	void writeChunk(const DataChunk& chunk)
	{
		DataChunk s = chunk;

		while (*s) {
			printf("%c", *s);
			s++;
		}
	}

	void printChunk(const DataChunk& chunk)
	{
		if (chunk)
		{
			writeChunk(chunk);
			printf("\n");
		}
		else
			printf("BLANK==CHUNK\n");

	}
}

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