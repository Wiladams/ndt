#pragma once

//
// The ScreenGrid represents an array of cells which can 
// be set in an orderly fashion.
// 
// There is the concept of a 'cursor', which represents the x,y
// position of the cell that will be written to next.  This 
// cursor moves along automatically as cells are written.
// 
// The class is templatized, so that any kind of cell can be 
// contained.  The cell grid is only reposible for the semantics
// of the cursor, and performing items such as clearing cells
// or setting them to a single value.
// 
// Other classes are responsible for actually displaying the contents
// of the cells, so nothing related to displaying cells actually 
// happens within here.
// 
// The semantics for grid management are based on the VT100 terminal type
// so all the commands the VT100 terminal are capable of can be implemented
// using this cell grid as a convenient base.
//



#include <stdint.h>


template <typename CellT>
struct ScreenGrid
{
	size_t fHeight = 0;		// Number of rows in the console
	size_t fWidth = 0;	// Number of columns in the console

	int fCursorX = 0;
	int fCursorY = 0;
	CellT *fCharArray;

	// Attributes
	bool fAutoWrap;
	bool fAutoScroll;


	ScreenGrid(const size_t cols, const size_t rows, const char* fontname = "Consolas")
		:fHeight(rows)
		, fWidth(cols)
	{
		fCharArray = new CellT[cols * rows];

		fAutoWrap = false;
		fAutoScroll = true;
	}

	~ScreenGrid()
	{
		delete fCharArray;
	}

	// Properties
	// How many characters wide is the screen
	constexpr size_t width() const noexcept { return fWidth; }

	// How many characters tall is the screen
	constexpr size_t height() const noexcept { return fHeight; }

	//
	// Attributes
	// 
	void autoWrap(bool doWrap)
	{
		fAutoWrap = doWrap;
	}

	//
	// CURSOR CONTROL
	// 

	// Return current cursor position
	void cursorPosition(int &x, int &y)
	{ 
		x = fCursorX;
		y = fCursorY;

	}

	// Move cursor to position (0,0)
	void cursorHome()
	{
		fCursorX = 0;
		fCursorY = 0;
	}

	// Set cursor to an absolute position
	// reject if not within range
	void cursorTo(int x, int y)
	{
		fCursorX = x;
		fCursorY = y;
	}

	void cursorUp()
	{
		fCursorY -= 1;
		if (fCursorY < 0)
			fCursorY = 0;
	}

	// Move cursor down one row
	// If positioned at bottom margin, stop
	void cursorDown() 
	{
		fCursorY += 1;

		if (fCursorY >= height())
		{
			if (fAutoScroll)
			{
				scrollUp();
			}
			fCursorY = height() - 1;
		}

	}

	// Move cursor left
	// If positioned at left margin, stop
	void cursorLeft() {
		fCursorX -= 1;
		if (fCursorX < 0)
			fCursorX = 0;
	}

	// Move cursor right
	// If positioned at right margin, stop
	void cursorRight()
	{
		fCursorX += 1;
		if (fCursorX >= fWidth)
			fCursorX = fWidth - 1;
	}

	void cursorToLineBegin() { fCursorX = 0; }

	void cursorToLineEnd() { fCursorX = fWidth - 1; }

	// check within bounds
	void cursorToColumn(int col)
	{
		if ((col < 0) || (col >= fWidth))
			return;

		fCursorX = col;
	}

	void copyRow(int fromRow, int toRow)
	{
		int fromOffset = cellOffset(0, fromRow);
		int toOffset = cellOffset(0, toRow);

		for (size_t i = 0; i < width(); i++)
		{
			fCharArray[toOffset + i] = fCharArray[fromOffset + i];
		}
	}

	void scrollUp()
	{
		// quick sanity check
		//if (nRows > height() - 2)
		//	return;


			// copy all the rows up one, starting from the top
			for (size_t row = 1; row < height(); row++)
			{
				copyRow(row, row - 1);
			}

			// Erase the bottom row of the screen
			eraseLine(height() - 1);

	}

	//
	// Setting cell values
	// Calculate the character offset of 
	// a cursor position
	int cellOffset(int x, int y)
	{
		return y * fWidth + x;
	}

	// Set ScreenChar at a particular location
	// don't do bounds checking here
	void setCell(int x, int y, const CellT& info)
	{
		int offset = cellOffset(x, y);
		if (offset < 0)
			return;

		fCharArray[offset] = info;
	}

	// output a single character with bounds checking
	// advance the cursor as well
	bool outScreenCell(const CellT& info)
	{
		if (fCursorX == width()) {
			// We're already at the edge of
			// the screen.  If autowrap is
			// not turned on, then we need to just return without
			// doing anything
			if (!fAutoWrap)
				return false;

			// OK, so we know we're doing autowrap.
			// If we're already at the bottom edge
			// of the screen
			if (fCursorY == height())
			{
				// We need another row.  If autoscroll
				// is not turned on, then we're done.
				if (!fAutoScroll)
					return false;
			}

			// scroll up one row
			scrollUp();

			fCursorX = 0;
			fCursorY = height() - 1;
		}

		if ((fCursorX >= fWidth) || (fCursorY >= fHeight))
			return false;

		setCell(fCursorX, fCursorY, info);

		fCursorX += 1;

		return true;
	}

	// Output a set of characters
	void outChars(const CellT* chars, int nChars)
	{
		for (size_t i = 0; i < nChars; i++)
		{
			if (!outScreenCell(chars[i]))
				break;
		}
	}

	// Set all characters in the array to the specified
	// ScreenChar
	void setAllCells(const CellT& info)
	{
		for (int row = 0; row < fHeight; row++)
			for (int col = 0; col < fWidth; col++)
				setCell(col, row, info);
	}


	//
	// SCREEN CLEARING
	//

	// set all positions in the screen 
	// to a default ScreenChar
	void eraseScreen()
	{
		CellT info{};
		setAllCells(info);
	}

	void eraseLine(int y)
	{
		for (int x = 0; x < width(); x++)
			setCell(x, y, CellT());
	}

	void eraseCursorLine() 
	{ 
		eraseLine(fCursorY);
	}

	void eraseToEndOfScreen() 
	{
		eraseToEndOfLine();
		int y = fCursorY + 1;
		if (y >= height())
			return;

		while (y < height())
		{
			eraseLine(y);
			y++;
		}
	}

	void eraseFromBeginningOfScreen() {}

	void eraseToEndOfLine() 
	{
		for (int x = fCursorX; x < width(); x++)
			setCell(x, y, CellT());
	}

	void eraseFromBeginningOfLine() {}

};
