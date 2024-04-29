#include "studio.hpp"
#include "rangen.h"
#include "elements/screensnapshot.hpp"

// A graphic that can display a portion of a BLImage
// automatically scaling it to fit its frame
struct ImageView : GraphicElement
{
	BLImage fImage{};
	maths::vec2f fImageSize{};

	// Given a source image, create a sub-image based on the box
	ImageView(BLImage& img, maths::rectf &imgbox, maths::rectf aframe)
		:GraphicElement(aframe)
	{
		fImageSize = maths::size(imgbox);

		// get a sub-image of the base image
		BLImageData dataOut;
		img.getData(&dataOut);
		uint8_t * pixelData = (uint8_t *)dataOut.pixelData + ((int)imgbox.y * (ptrdiff_t)dataOut.stride + ((int)imgbox.x * 4));
		blImageInitAsFromData(&fImage, fImageSize.x, fImageSize.y, BLFormat::BL_FORMAT_PRGB32, pixelData, dataOut.stride, BLDataAccessFlags::BL_DATA_ACCESS_RW , nullptr, nullptr);
	}

	void draw(IGraphics& ctx)
	{
		ctx.push();

		float tx = frameX();
		float ty = frameY();

		ctx.translate(tx, ty);

		ctx.scaleImage(fImage, 0, 0, fImageSize.x, fImageSize.y, 0, 0, frameWidth(), frameHeight());

		ctx.pop();
	}
};

struct ImagePuzzlePiece : public ImageView
{
	int fID{};
	bool fIsDragging = false;
	maths::vec2f fLastMouse{};

	ImagePuzzlePiece(BLImage& img, maths::rectf &imgbox, maths::rectf &aframe, int id)
		:ImageView(img, imgbox, aframe)
		, fID(id)
	{}

	void mouseEvent(const MouseEvent& e)
	{
		// First check to see if we're moving our window
// around, from dragging in the titleBar area
		switch (e.activity)
		{
			case MOUSEPRESSED:
				fIsDragging = true;
				fLastMouse = { e.x, e.y };
			return;

			break;

		case MOUSERELEASED:
			fIsDragging = false;
			break;

		case MOUSEMOVED:
			if (fIsDragging) {
				// move
				auto dx = (e.x - fLastMouse.x);
				auto dy = (e.y - fLastMouse.y);

				moveBy(dx, dy);

				fLastMouse = { e.x, e.y };
				return;
			}
			break;
		}
	}
};

struct ElementGrid : public GraphicGroup
{
	std::vector<std::shared_ptr<GraphicElement> > fCells{};
	int fNumColumns{};
	int fNumRows{};

	float cellWidth{};
	float cellHeight{};

	ElementGrid(maths::rectf aframe, int rows, int columns)
		:GraphicGroup(aframe)
	{
		reset(columns, rows);
	}

	int offsetForCell(int row, int col) const
	{
		return (row * fNumColumns) + col;
	}

	void reset(int columns, int rows)
	{
		fNumColumns = columns;
		fNumRows = rows;

		cellWidth = frameWidth() / fNumColumns;
		cellHeight = frameHeight() / fNumRows;

		fCells.clear();
		fCells.reserve(fNumColumns * fNumRows);

		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < columns; col++)
			{
				int offset = offsetForCell(row,col);
				fCells.push_back(nullptr);
			}
		}
	}

	std::shared_ptr<GraphicElement> atRowColumn(int row, int col) 
	{ 
		int offset =offsetForCell(row,col);
		return fCells[offset]; 
	}

	void setCell(int row, int col, std::shared_ptr<GraphicElement> g)
	{
		int offset = offsetForCell(row, col);
		fCells[offset] = g;
	}

	maths::rectf frameForCell(int row, int col)
	{
		maths::rectf pbox = { col * cellWidth,row * cellHeight, cellWidth - 1, cellHeight - 1 };
		return pbox;
	}
};

struct ImagePuzzle : ElementGrid
{
	bool fIsDragging{};
	std::shared_ptr<GraphicElement> fSelectedGraphic{};

	float imgCellWidth{};
	float imgCellHeight{};

	maths::rng_state RNG;

	std::vector<std::shared_ptr<ImagePuzzlePiece> > pieces{};

	ImagePuzzle(BLImage& img, maths::rectf aframe, int nColumns, int nRows)
		: ElementGrid(aframe, nRows, nColumns)
	{
		RNG = maths::make_rng(52);

		BLImageData dataOut{};
		img.getData(&dataOut);
		imgCellWidth = dataOut.size.w/nColumns;
		imgCellHeight = dataOut.size.h/nRows;

		for (int row =0;row<fNumRows;row++)
			for (int col = 0; col < fNumColumns; col++)
			{
				int id = offsetForCell(row, col);

				maths::rectf imgbox = {col * imgCellWidth,row * imgCellHeight, imgCellWidth, imgCellHeight };
				maths::rectf pbox = frameForCell(row, col);

				auto piece = std::make_shared<ImagePuzzlePiece>(img, imgbox, pbox, id);

				addGraphic(piece);

				pieces.push_back(piece);

				setCell(row, col, piece);
			}
	}

	void mouseEvent(const MouseEvent& e)
	{
		// Mouse events are given in the coordinate space of the 
		// parent frame, so we want to convert to our local coordinate
		// space before doing anything else.
		// First subtract off the frame origin
		MouseEvent localEvent(e);
		localEvent.x = e.x - frameX();
		localEvent.y = e.y - frameY();

		auto hovered = graphicAt(localEvent.x, localEvent.y);

		switch (e.activity)
		{
		case MOUSEPRESSED:
			fIsDragging = true;
			fSelectedGraphic = hovered;
			if (fSelectedGraphic != nullptr)
			{
				moveToFront(fSelectedGraphic);
				fSelectedGraphic->mouseEvent(localEvent);
			}
			break;

		case MOUSERELEASED:
			fIsDragging = false;
			if (fSelectedGraphic != nullptr)
			{
				fSelectedGraphic->mouseEvent(localEvent);
			}
			fSelectedGraphic = nullptr;

			break;

		case MOUSEMOVED:
			// If dragging, keep sending to the original 
			// graphic, even if we're beyond its bounds
			if (fIsDragging)
			{
				if (fSelectedGraphic != nullptr)
					fSelectedGraphic->mouseEvent(localEvent);
			}
			else {
				if (hovered)
					hovered->mouseEvent(localEvent);
			}
			break;

		default:
			if (fSelectedGraphic != nullptr)
				fSelectedGraphic->mouseEvent(localEvent);
			break;
		}
	}

	void drawBackground(IGraphics& ctx) override
	{
		ctx.push();

		// Draw a thick green border just so we can see it
		ctx.noFill();
		ctx.strokeWeight(4);
		ctx.stroke(0, 255, 0);
		ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

		// Draw horizontal and vertial column divider lines
		ctx.stroke(0);
		ctx.strokeWeight(1);
		for (int row = 1; row < fNumRows; row++)
		{
			ctx.line(0, row * cellHeight, boundsWidth(), row * cellHeight);
		}

		for (int col = 1; col < fNumColumns; col++)
		{
			ctx.line(col*cellWidth, 0, col*cellWidth, boundsHeight());
		}

		ctx.pop();
		ctx.flush();
	}

	void drawForeground(IGraphics& ctx)
	{
		if (fSelectedGraphic == nullptr)
			return;

		// get the center of the selected graphic
		auto selectedCenter = maths::center(fSelectedGraphic->frame());

		ctx.push();

		// calculate the distance to the center of every square
		// if the distance is less than tolerance
		// lock drop the piece within the square
		// if the piece ID matches that of the square
		// lock the piece into place, by making it unmoveable
		ctx.strokeWeight(4.0);

		for (int row = 0; row < fNumRows; row++)
			for (int col = 0; col < fNumColumns; col++)
			{
				int offset = offsetForCell(row,col);

				maths::rectf cellBox = {col * cellWidth,row * cellHeight,cellWidth,cellHeight};

				auto cellCenter = maths::center(cellBox);
				auto dist = maths::distance(selectedCenter, cellCenter);

				if (dist < 24.0f)
				{
					fSelectedGraphic->setFrame(cellBox);
					ctx.stroke(0, 255, 0);
				}
				else {
					ctx.stroke(255, 0, 0);
				}

				ctx.line(selectedCenter.x, selectedCenter.y, cellCenter.x, cellCenter.y);
			}
		
		ctx.pop();
		ctx.flush();
	}

	void shuffle()
	{
		maths::shuffle(pieces, RNG);

		// Go through one by one, reassigning frames
		for (int row = 0; row < fNumRows; row++)
			for (int col = 0; col < fNumColumns; col++)
			{
				int offset = offsetForCell(row,col);
				maths::rectf pbox = {col * cellWidth,row * cellHeight,cellWidth, cellWidth };
				pieces[offset]->setFrame(pbox);
			}
	}
};

ScreenSnapper snapper{};
std::shared_ptr<ImagePuzzle> puzzle = nullptr;

void updateFrame(const FrameCountEvent &fce)
{
	snapper.next();
}

void keyEvent(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;


	case VK_SPACE:
		if (e.activity == KEYRELEASED)
			puzzle->shuffle();
		break;
	}
}

void setup()
{
	snapper.reset(0, 0, displayWidth / 2, displayHeight);
	snapper.next();

	puzzle = std::make_shared<ImagePuzzle>(snapper.getImage(), maths::rectf{ 0,0,(float)displayWidth/2,(float)displayHeight }, 8, 8);

	addGraphic(puzzle);

	subscribe(updateFrame);
	subscribe(keyEvent);
}