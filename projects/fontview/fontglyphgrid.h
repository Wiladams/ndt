#pragma once

#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"

constexpr int maxColumns = 16;
constexpr int cellWidth = 12;
constexpr int gcellHeight = 16;
constexpr uint32_t FontSize = 16;

struct GlyphGridRowHeading : public GraphicElement
{
	static maths::vec2f preferredSize() { return { cellWidth*maxColumns,24 }; }

	maths::vec2f fCellSize{};

	GlyphGridRowHeading(int x, int y, int w, int h)
		:GraphicElement(x,y,w,h)
	{
		fCellSize = { boundsWidth() / maxColumns, boundsHeight() };

	}

	void drawSelf(IGraphics& ctx)
	{
		ctx.strokeWeight(4.0);
		ctx.stroke(255, 0, 255);
		ctx.rect(0, 0, boundsWidth(), boundsHeight());

		float xoffset = 0;
		float yoffset = 0;

		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::TOP);
		ctx.textSize(16);
		ctx.textFont("Consolas");
		ctx.noStroke();
		ctx.fill(0);

		for (int i = 0; i < maxColumns; i++)
		{
			xoffset = (i * fCellSize.x)+(fCellSize.x/2.0f);
			yoffset = 0;
			char num[64];
			sprintf_s(num, "%02X", i);
			ctx.text(num, xoffset, yoffset);
		}
	}
};

struct GlyphGridColumnHeading : public GraphicElement
{
	//static maths::vec2f preferredSize() { return { cellWidth,cellHeight*maxColumns }; }

	GlyphGridColumnHeading(int x, int y, int w, int h)
		:GraphicElement(x,y,w,h)
	{}

	void drawSelf(IGraphics& ctx)
	{
		ctx.strokeWeight(4.0);
		ctx.stroke(255, 0, 255);
		ctx.rect(0, 0, boundsWidth(), boundsHeight());
	}
};

struct GlyphGridBody :public GraphicElement
{
	//static maths::vec2f preferredSize() { return { cellWidth*maxColumns,cellHeight*16 }; }

	maths::vec2f fCellSize{};

	BLFontFace fFontFace{};
	BLFont fFont{};
	BLFontDesignMetrics fDesignMetrics{};


	uint16_t fGlyphCount = 0;
	uint16_t fFirstGlyph = 0;
	uint32_t fCurrentGlyph = 0;

	GlyphGridBody(int x, int y, int w, int h)
		:GraphicElement(x, y, w, h)
	{
		fCellSize = {boundsWidth()/maxColumns, boundsHeight()/16};
	}

	void setFontFace(BLFontFace& other)
	{
		fFontFace = other;
		fFont.createFromFace(fFontFace, fCellSize.y);
		fDesignMetrics = fFontFace.designMetrics();

		fGlyphCount = fFontFace.faceInfo().glyphCount;
		fFirstGlyph = 0;

		int numRows = fGlyphCount / maxColumns;
		float bheight = numRows * fCellSize.y;
		float bwidth = maxColumns * fCellSize.x;

		setBounds({ {0,0},{bwidth,bheight} });
	}

	void drawSelf(IGraphics& ctx) override
	{
		ctx.fill(65, 127);
		ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

		int column = 0;
		int row = 0;

		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);

		for (size_t glyphid = fFirstGlyph; glyphid < (fFirstGlyph + fGlyphCount); glyphid++)
		{
			ctx.push();

			BLPath gpath;
			fFont.getGlyphOutlines(glyphid, gpath);
			double dx = column * fCellSize.x;
			double dy = row * fCellSize.y;
			ctx.translate(dx, dy);

			// For debug, so we can see cell outline
			//ctx.stroke(0);
			//ctx.noFill();
			//ctx.rect(0, 0, cellWidth, cellHeight);

			// draw the path outline
			ctx.fill(255);
			ctx.noStroke();
			ctx.path(gpath);

			ctx.pop();

			column += 1;

			if (column >= maxColumns)
			{
				column = 0;
				row = row + 1;
			}
		}

		// Debug, showing boundary
		ctx.rectMode(RECTMODE::CORNER);
		ctx.stroke(255, 0, 255);
		ctx.rect(0, 0, boundsWidth(), boundsHeight());
	}
};


// FontGlyphGrid
// Pulls all the pieces of the glyph grid together
struct FontGlyphGrid : public Graphic
{
	float fCellWidth = 0;
	float fCellHeight = 0;

	std::shared_ptr< GlyphGridBody> fGridBody = nullptr;
	std::shared_ptr< GlyphGridColumnHeading> fColumnHeading = nullptr;
	std::shared_ptr<GlyphGridRowHeading> fRowHeading = nullptr;

	FontGlyphGrid(float x, float y, float w, float h)
		:Graphic(x,y,w,h)
	{

		maths::vec2f cSize = { 48, 1 };		// Size of column header
		float bodyWidth = boundsWidth() - cSize.x;
		fCellWidth = bodyWidth / maxColumns;
		fCellHeight = 1.33f * fCellWidth;

		maths::vec2f bodySize = { bodyWidth, fCellHeight*16};
		cSize.y = bodySize.y;

		maths::vec2f rSize = { bodySize.x, 24 };	// Size of row header



		// Row heading
		fRowHeading = std::make_shared< GlyphGridRowHeading>(cSize.x, 0, rSize.x, rSize.y);
		fColumnHeading = std::make_shared< GlyphGridColumnHeading>(0, rSize.y, cSize.x, cSize.y);
		fGridBody = std::make_shared< GlyphGridBody>(cSize.x, rSize.y, bodySize.x, bodySize.y);

		//addChild(fGridBody);
		//addChild(fRowHeading);
		//addChild(fColumnHeading);
	}

	void draw(IGraphics& ctx) override
	{
		ctx.push();
		//ctx.translate(fGridBody->frameX(), fGridBody->frameY());
		fGridBody->draw(ctx);
		ctx.pop();
		
		ctx.push();
		//ctx.translate(fRowHeading->frameX(), fRowHeading->frameY());
		fRowHeading->draw(ctx);
		ctx.pop();
		
		ctx.push();
		//ctx.translate(fColumnHeading->frameX(), fColumnHeading->frameY());
		fColumnHeading->draw(ctx);
		ctx.pop();
		
	}

	// Scrolling the mouse makes the column and grid body
	// move up and down
	// The row heading remains constant
	void mouseWheel(const MouseEvent& e) override
	{
		double scrollSize = FontSize;

		printf("fontglyphgrid.wheel: %d\n", e.delta);
		fGridBody->translateBoundsBy(0, (e.delta / 120.0) * scrollSize);
		fColumnHeading->translateBoundsBy(0, (e.delta / 120.0) * scrollSize);
	}

	void setFontFace(BLFontFace& face)
	{
		fGridBody->setFontFace(face);
	}
};