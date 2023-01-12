#pragma once


#include "definitions.h"

#include <bitset>


namespace ndt {
	// Represent a set of characters as a bitset
	//
	// Typical usage:
	//   charset whitespaceChars("\t\n\f\r ");
	//
	//	 // skipping over whitespace
	//   while (whitespaceChars[c])
	//		c = nextChar();
	//
	//
	//  This is better than simply using the old classic isspace() and other functions
	//  as you can define your own sets, depending on your needs:
	//
	//  charset delimeterChars("()<>[]{}/%");
	//
	//  Of course, there will surely be a built-in way of doing this in C/C++ 
	//  and it will no doubt be tied to particular version of the compiler.  Use that
	//  if it suits your needs.  Meanwhile, at least you can see how such a thing can
	//  be implemented.


	struct charset {
		std::bitset<256> fBits{};

		charset() = default;
		charset(const char achar){addChar(achar);}
		charset(const char* chars){addChars(chars);}

		// Add a single character to the set
		charset& addChar(const unsigned char achar)
		{
			fBits.set(achar);
			return *this;
		}
		
		// Add a string of characters
		charset& addChars(const char* chars)
		{
			size_t len = strlen(chars);
			for (size_t i = 0; i < len; i++)
				fBits.set(chars[i]);

			return *this;
		}
		
		// subtract a single character fromn the set
		charset& operator-=(const unsigned char achar)
		{
			fBits.reset(achar);
			return *this;
		}
		
		// add a single character to the set
		charset& operator+=(const char achar)
		{
			addChar(achar);
			return *this;
		}
		
		// add a string of characters to the set
		charset& operator+=(const char* chars)
		{
			addChars(chars);
			return *this;
		}
		

		
		// Test set membership using array syntax
		inline bool operator [](const size_t idx) const
		{
			return fBits[idx];
		}

		// Test set membership using function syntax
		inline bool operator ()(const size_t idx) const
		{
			return fBits[idx];
		}

		// Test set membership using explicit function syntax
		inline bool contains(const uint8_t idx) const
		{
			return fBits[idx];
		}
	};

	// Just a few helper routines for set theoretic operations
	// This might make constructing regular expressions a bit easier
	// 
	// bitwise OR, the union of two charsets
	INLINE charset operator|(const charset& lhs, const charset& rhs) noexcept 
	{
		charset result{};
		result.fBits = lhs.fBits | rhs.fBits;
		return result;
	}

	// bitwise AND, the intersection of two charsets
	INLINE charset operator&(const charset& lhs, const charset& rhs) noexcept 
	{
		charset result{};
		result.fBits = lhs.fBits & rhs.fBits;
		return result;
	}
	
	// bitwise exclusive OR, the difference between the two sets
	INLINE charset operator^(const charset& lhs, const charset& rhs) noexcept 
	{
		charset result{};
		result.fBits = lhs.fBits ^ rhs.fBits;
		return result;
	}

	// Support adding to sets using single chars, and strings
	INLINE charset operator+(const charset& lhs, const unsigned char achar) noexcept
	{
		charset result(lhs);
		result += achar;
		return result;
	}
	
	INLINE charset operator+(const charset& lhs, const char* chars) noexcept
	{
		charset result(lhs);
		result += chars;
		return result;
	}
	
}
