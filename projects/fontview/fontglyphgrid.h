#pragma once

#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"



// Display all the glyphs in a font
struct FontGlyphGrid : public Graphic
{
	BLFontFace fFontFace{};
	BLFont fFont{};
	BLFontDesignMetrics fDesignMetrics{};

	uint32_t fFontSize=16;
	uint16_t fGlyphCount=0;
	uint16_t fFirstGlyph=0;
	uint32_t fCurrentGlyph=0;

	int maxColumns=16;
	int cellWidth = 12;
	int cellHeight = 16;


	FontGlyphGrid(float x, float y, float w, float h)
		:Graphic(x,y,w,h)
	{
		cellWidth = w / maxColumns;
		cellHeight = cellWidth;
	}

	void setFontFace(BLFontFace& other)
	{
		fFontFace = other;
		fFont.createFromFace(fFontFace, cellHeight);
		fDesignMetrics = fFontFace.designMetrics();

		fGlyphCount = fFontFace.faceInfo().glyphCount;
		fFirstGlyph = 0;

		int numRows = fGlyphCount / maxColumns;
		float bheight = numRows * cellHeight;
		float bwidth = maxColumns * cellWidth;

		setBounds({ {0,0},{bwidth,bheight} });
	}

	void drawSelf(IGraphics& ctx) override
	{
		ctx.fill(65,127);
		ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

		int column = 0;
		int row = 0;

		for (size_t glyphid = fFirstGlyph; glyphid < (fFirstGlyph + fGlyphCount); glyphid++)
		{
			ctx.push();

			BLPath gpath;
			fFont.getGlyphOutlines(glyphid, gpath);
			double dx = column * cellWidth;
			double dy = row * cellHeight;
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
	}

	void mouseWheel(const MouseEvent& e) override
	{
		double scrollSize = fFontSize;

		printf("fontglyphgrid.wheel: %d\n", e.delta);
		translateBoundsBy(0, (e.delta / 120.0) * scrollSize);
	}
};