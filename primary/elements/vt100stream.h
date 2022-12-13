#pragma once

#include "binstream.hpp"
#include <cctype>



//
// Turn vt100 stream into command sequences
//
//constexpr uint8_t TAB			= 011;	// 9
constexpr uint8_t ESC			= 033;	// 0x1B;
constexpr uint8_t LEFTBRACKET	= 0x5B;
//constexpr uint8_t XON			= 0x11;
//constexpr uint8_t XOFF			= 0x13;


struct VT100Stream
{
	BinStream& fStream;

private:
	static bool isParamChar(uint8_t c)
	{
		return (isdigit(c) || c == ';' || c == '"' || c == '?');
	}

public:
	VT100Stream(BinStream& src)
		: fStream(src)
	{

	}

	// Read a numeric value that's encoded as
	// ascii digits
	int readParam()
	{
		int n = 0;
		while (true)
		{
			uint8_t c = fStream.peekOctet();
			if (isdigit(c))
			{
				n = n * 10;
				n = n + c;
				fStream.skip(1);
			}
			else
				break;
		}

		return n;
	}

	void decodeControlSequence()
	{
		uint8_t c = fStream.readOctet();


		switch (c)
		{
			case 'A':
				break;
			
			case 'B':
			break;

			case 'C':
			break;

			case 'D':
			break;

			case 'F':
				break;

			case 'P':	// Cursor management
			{
				// read 'n'
				uint8_t fc = fStream.readOctet();

				switch (fc) {
				case 'A':
					// cursorUp	(CUU)
					break;

				case 'D':
					// cursorBack	(CUB)
					break;

				case 'B':
					// cursorDown	(CUD)
					break;

				case 'C':
					// cursorForward	(CUF)
					break;

				case 'H':
					// setCursor	(CUP)
					//n; PnH
					break;
				}
			}
				break;


		}
	}

	bool next()
	{
		uint8_t c = fStream.readOctet();

		switch (c)
		{
			case 24:			// CAN
			case 26:			// SUB
				// do nothing 
			break;

			case ESC:	// ESC
				c = fStream.readOctet();
				if (c == LEFTBRACKET)
					decodeControlSequence();
				else {
					// insert character into console?
				}
			break;
		}

		return !fStream.isEOF();
	}
};