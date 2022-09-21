#ifndef emspaceview_h
#define emspaceview_h


#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"

struct EMSpaceView : public Graphic
{
	static constexpr int edgeMargin = 24;
	static constexpr int edgeTopMargin = 48;
	static constexpr int edgeBottomMargin = 24;
	//static constexpr int boundsWidth = 800;
	//static constexpr int boundsHeight = 800;

	int coordsWidth=0;
	int coordsHeight=0;


	BLFontFace fontFace;

	EMSpaceView(int w, int h)
		:Graphic(0,0,w,h)
	{
		coordsWidth = width() - edgeMargin * 2;
		coordsHeight = height() - edgeMargin * 2;
	}

	void setFace(const BLFontFace& face)
	{
		fontFace = face;
	}

	void drawBackground(IGraphics& ctx)
	{
		// Draw the frame outline of the whole space
		ctx.stroke(0,0,255);
		ctx.noFill();
		ctx.rect(0, 0, width(), height());
	}

	void drawSelf(IGraphics& ctx)
	{
		BLFontDesignMetrics designMetrics = fontFace.designMetrics();

		ctx.push();
		// Position of graphic in the frame of parent
		//ctx.translate(462, 8);

		// Draw the frame outline of the whole space
		//ctx.stroke(0);
		//ctx.noFill();
		//ctx.rect(0, 0, width(), height());

		// Draw the outline of the actual coordinate space
		ctx.stroke(0x2f);
		ctx.rect(edgeMargin, edgeMargin, coordsWidth, coordsHeight);

		// Draw coordinate axiis
		ctx.stroke(255, 0, 0);
		ctx.strokeWeight(0.5);
		ctx.line(edgeMargin + (coordsWidth / 2.0), edgeMargin, edgeMargin + (coordsWidth / 2.0), height() - edgeMargin);
		ctx.line(edgeMargin, edgeMargin + (coordsHeight / 2.0), width() - edgeMargin, edgeMargin + (coordsHeight / 2.0));

		// Bounding box
		// Area label
		ctx.noStroke();
		ctx.fill(0);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::TOP);
		ctx.textSize(16);
		ctx.text("Glyph Bounding Box", width() / 2.0, 0.0);

		// Coordinate labels
		// upper left
		ctx.noStroke();
		ctx.fill(0);
		ctx.textSize(edgeMargin * 0.75);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		ctx.vartext(edgeMargin / 2.0, height() - edgeMargin, "(%d, %d)",
			designMetrics.glyphBoundingBox.x0, designMetrics.glyphBoundingBox.y0);

		// upper right
		ctx.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
		ctx.vartext(width() - (edgeMargin / 2.0), 0.0, "(%d, %d)",
			designMetrics.glyphBoundingBox.x1, designMetrics.glyphBoundingBox.y1);


		// Translate coordinate center to be middle of the area
		ctx.translate(edgeMargin + coordsWidth / 2.0, edgeMargin + coordsHeight / 2.0);

		// flip y coordinate axis to point positive upward
		ctx.scale(1.0, -1.0);

		// Whenever we do text now, we need to flip
		// coordinates temporarily
		ctx.push();
		ctx.scale(1.0, -1.0);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		ctx.text("(0,0)", edgeMargin, edgeMargin);
		ctx.pop();

		ctx.pop();
	}
};

#endif	// emspaceview_h
