#pragma once


//	Herein you will find various objects and functions which are
//	useful while scanning, tokenizing, parsing streams of text.


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
		std::bitset<256> bits;

		charset(const char* chars)
		{
			size_t len = strlen(chars);
			for (size_t i = 0; i < len; i++)
				bits.set(chars[i]);
		}

		charset operator+(const char achar)
		{
			charset result(*this);
			result.bits.set(achar);
			return result;
		}
		

		
		// This one makes us look like an array
		inline bool operator [](const size_t idx) const
		{
			return bits[idx];
		}

		// This way makes us look like a function
		inline bool operator ()(const size_t idx) const
		{
			return bits[idx];
		}

		inline bool contains(const uint8_t idx) const
		{
			return bits[idx];
		}
	};


}
