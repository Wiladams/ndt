#pragma once

#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"

// Display all the glyphs in a font
struct FontGlyphGrid : public Graphic
{


	BLFontFace fFontFace;
	BLFont fFont;
	BLFontDesignMetrics fDesignMetrics;

	uint32_t fFontSize=16;
	uint16_t fGlyphCount;
	uint16_t fFirstGlyph;
	uint32_t fCurrentGlyph;

	int maxColumns=16;
	int cellWidth = 12;
	int cellHeight = 16;


	FontGlyphGrid(int w, int h)
		:Graphic(0,0,w,16*32767)
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
	}

	void drawSelf(IGraphics& ctx) override
	{
		int column = 0;
		int row = 0;

		ctx.fill(0);
		ctx.noStroke();

		for (size_t glyphid = fFirstGlyph; glyphid < (fFirstGlyph + fGlyphCount); glyphid++)
		{
			ctx.push();

			BLPath gpath;
			fFont.getGlyphOutlines(glyphid, gpath);
			double dx = column * cellWidth;
			double dy = row * cellHeight;
			ctx.translate(dx, dy);
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
		translateBy(0, (e.delta / 120.0) * scrollSize);
	}
};