#pragma once

// Reference: 
// VT terminal  http://bitsavers.org/pdf/dec/terminal/vt100/EK-VT100-UG-002_VT100_User_Guide_Jan79.pdf
//

#include "fonthandler.hpp"
#include "graphic.hpp"
#include "Graphics.h"
#include "elements/cellgrid.h"

// Some typical colors
constexpr Pixel darkAmber{ 0xffffb000 };
constexpr Pixel lightAmber{ 0xffffcc00 };
constexpr Pixel green1{ 0xff33ff00 };			// 524nm
constexpr Pixel green2{ 0xff00ff33 };			// 506nm
constexpr Pixel green3{ 0xff00ff66 };			// 502nm
constexpr Pixel appleII{ 0xff33ff33 };		// apple II 
constexpr Pixel appleIIc{ 0xff66ff66 };		// apple IIc

// Representation of a single character cell to be displayed on the screen
template <typename T>
struct ScreenCell
{
	//static int defaultCode;
	//static int defaultAttributes;

	T code;
	uint32_t attributes;
	uint32_t backgroundColor;
	uint32_t foregroundColor;

	ScreenCell()
		:code(T())
		, attributes(0)
		, backgroundColor(0x00000000)
		, foregroundColor(0xff00ff00)
	{
	}

	ScreenCell(const T& acode, uint32_t fg, uint32_t bg, uint32_t attr)
	{
		code = acode;	// space character
		attributes = attr;
		backgroundColor = bg;
		foregroundColor = fg;
	}
};

//
// GConsole
// As a specialization of CharacterScreen, the GConsole has all the cursor
// management and character placement commands.  In addition, it
// knows how to draw itself into a graphics context.

struct GConsole : public GraphicElement, public CellGrid<ScreenCell<char> >
{
	int charWidth = 7;
	int charHeight = 12;
	static constexpr float fontSize = 12;
	BLFontFace fFontFace;
	BLFont fFont;

	explicit GConsole(const GConsole&)
		:GraphicElement()
		, CellGrid<ScreenCell<char> >(80,20)
	{}
	
	GConsole(const size_t cols, const size_t rows, const char* fontname = "Consolas")
		: GraphicElement()
		, CellGrid<ScreenCell<char> >(cols, rows)
	{
		reset(cols, rows, fontname);
	}

	void reset(const size_t cols, const size_t rows, const char* fontname = "Consolas")
	{
		CellGrid<ScreenCell<char> >::reset(cols, rows);

		gFontHandler->queryFontFace(fontname, fFontFace);
		fFont.createFromFace(fFontFace, fontSize);

		//BLFontMetrics metrics = fFont.metrics();

		//float vSize = metrics.ascent + metrics.descent;
		//printf("metrics.vsize: %f\n", vSize);

		//putsln("");
		setFrame({ 0, 0, float(fWidth * charWidth, float(fHeight * charHeight) });
		setBounds({ 0, 0, float(fWidth * charWidth), float(fHeight * charHeight) });
	}
	
	// Simplification commands

	// move down one row, and to leftmost column
	void newLine()
	{
		cursorDown();
		cursorToLineBegin();
	}

	// place a single character
	void putc(char c)
	{
		ScreenCell<char> info{};
		info.code = c;
		outScreenCell(info);
	}

	void puts(const char* txtBuff)
	{
		int offset = 0;
		while (txtBuff[offset])
		{
			ScreenCell<char> info{};
			info.code = txtBuff[offset];
			outScreenCell(info);
			offset++;
		}
	}

	void putsln(const char* txtBuff)
	{
		puts(txtBuff);
		newLine();
	}

	// turn the formatted string into ScreenChar array
// and put it into the frame buffer
	void printf(const char* format, ...)
	{
		char txtBuff[512];
		va_list args = nullptr;
		va_start(args, format);

		vsprintf_s(txtBuff, format, args);
		va_end(args);

		// take the txtBuff, and put the characters
		// into our console buffer, using default attributes
		int offset = 0;
		while (txtBuff[offset])
		{
			switch (txtBuff[offset])
			{
			case '\n':
			{
				newLine();
			}
			break;

			default:
			{
				ScreenCell<char> info{};
				info.code = txtBuff[offset];
				outScreenCell(info);
			}
			}
			offset++;
		}

	}

	//
	// Draw to the frame buffer
	//
	void drawBackground(IGraphics& ctx) override
	{
		// draw background color of the terminal
		// typically black
		//ctx.fill(0,1);
		ctx.fill(7, 54, 66,1);
		ctx.rect(0, 0, (fWidth * charWidth), fHeight * charHeight);
	}

	void drawSelf(IGraphics& ctx) override
	{
		ctx.push();
		
		// draw the actual characters one by one
		ctx.textFace(fFontFace);
		ctx.textSize(fontSize);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		//ctx.fill(darkAmber);
		ctx.fill(133, 153, 0);
		ctx.noStroke();

		for (size_t col = 0; col < fWidth; col++)
			for (size_t row = 0; row < fHeight; row++)
			{
				int offset = cellOffset(col, row);

				switch (fCellArray[offset].code)
				{
					default: 
					{
						if (isprint(fCellArray[offset].code))
						{
							int x = col * charWidth;
							int y = row * charHeight;
							char buff[2];
							buff[0] = fCellArray[offset].code;
							buff[1] = 0;
							ctx.text(buff, x, y);
						}
					}
				}
			}

		ctx.pop();
	}
};
