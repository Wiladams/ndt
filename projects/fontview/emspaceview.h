#ifndef emspaceview_h
#define emspaceview_h

//
// Display the graphic details of a font glyph
//

#include "apphost.h"
#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"

class EMSpaceView : public GraphicElement
{
public:
	static constexpr int edgeMargin = 24;
	static constexpr int edgeTopMargin = 48;
	static constexpr int edgeBottomMargin = 24;


	int coordsWidth=0;
	int coordsHeight=0;


	BLFontFace fFontFace{};
	BLFont fFont{};
	BLFontDesignMetrics fDesignMetrics{};
	
	uint16_t fGlyphCount=0;
	uint16_t fFirstGlyph=0;
	uint32_t fCurrentGlyph=0;

	double emLeft = 0;
	double emRight = 0;
	double emTop = 0;
	double emBottom = 0;

	double zeroX = 0;
	double zeroY = 0;

	EMSpaceView(int w, int h)
		:GraphicElement(0,0,w,h)
	{
		coordsWidth = frameWidth() - (edgeMargin * 2);
		coordsHeight = frameHeight() - (edgeMargin * 2);
	}

	void setFace(const BLFontFace& face)
	{
		fFontFace = face;

		fGlyphCount = face.faceInfo().glyphCount;
		fFirstGlyph = 0;
		fCurrentGlyph = fFirstGlyph;

		fDesignMetrics = fFontFace.designMetrics();
		float fontSize = fFontFace.designMetrics().unitsPerEm;
		fFont.createFromFace(fFontFace, fontSize);

		emLeft = fFont.metrics().xMin;
		emRight = fFont.metrics().xMax;
		emTop = fFont.metrics().yMax;
		emBottom = fFont.metrics().yMin;

		zeroX = maths::map(0, emLeft, emRight, edgeMargin, edgeMargin + coordsWidth);
		zeroY = maths::map(0, emTop, emBottom, edgeMargin, edgeMargin + coordsHeight);
	}

	void setGlyphId(uint16_t gid)
	{
		fCurrentGlyph = gid;
	}

	void decrementGlyph()
	{
		fCurrentGlyph -= 1;
		if (fCurrentGlyph <= fFirstGlyph)
			fCurrentGlyph = fFirstGlyph;
	}

	void incrementGlyph()
	{
		fCurrentGlyph += 1;
		if (fCurrentGlyph > fFirstGlyph + fGlyphCount)
			fCurrentGlyph = fFirstGlyph+fGlyphCount - 1;
	}

	void drawBackground(IGraphics& ctx) override
	{
		// Draw the frame outline of the whole space
		ctx.stroke(0,0,255);
		ctx.noFill();
		ctx.rect(0, 0, frameWidth(), frameHeight());
	}

	void drawGlyphOutline(IGraphics& ctx)
	{
		// need to create an inverse scale matrix
		// so we can get things in design metrics units
		// This is what the font sets up
		//fontI->matrix.reset(xScale, 0.0, 0.0, -yScale);
		BLFontMatrix fMatrix = fFont.matrix();
		
		double xScale = fMatrix.m00;
		double yScale = fMatrix.m11;
		//printf(" xScale: % f, yScale: % f\n", xScale, yScale);
		//printf(" Size: %f Units Per Em: %d\n", fFont.size(), fFont.designMetrics().unitsPerEm);
		
		BLMatrix2D invMatrix{};
		invMatrix.reset(1.0 / xScale, 0.0, 0.0, 1.0 / yScale,0.0,0.0);

		BLPath path;
		BLResult err = blFontGetGlyphOutlines(&fFont, fCurrentGlyph, &invMatrix, &path, nullptr, nullptr);

		if (err) {
			printf("drawGlyphOutline, blFontGetGlyphOutlines, err: %d\n", err);
			return; 
		}

		//double emLeft = (double) - 16384 / fFont.designMetrics().unitsPerEm;
		//double emRight = (double)16383 / fFont.designMetrics().unitsPerEm;
		//double emBottom = (double) - 16384 / fFont.designMetrics().unitsPerEm;
		//double emTop = (double)16383 / fFont.designMetrics().unitsPerEm;
		
		//double emLeft = fDesignMetrics.glyphBoundingBox.x0;
		//double emRight = fDesignMetrics.glyphBoundingBox.x1;
		//double emTop = fDesignMetrics.glyphBoundingBox.y1-edgeMargin;
		//double emBottom = fDesignMetrics.glyphBoundingBox.y0;

		double emLeft = fFont.metrics().xMin;
		double emRight = fFont.metrics().xMax;
		double emTop = fFont.metrics().yMax;
		double emBottom = fFont.metrics().yMin;

		//printf("emSpace: %f,%f  %f,%f\n", emLeft, emTop, emRight, emBottom);
		



		ctx.push();
		//ctx.translate(edgeMargin + (coordsWidth / 2.0), edgeTopMargin+(coordsHeight / 2.0));
		//ctx.scale(coordsWidth/gbWidth, -coordsHeight/gbHeight);

		// display the outline
		BLPathView pview = path.view();
		BLPath dpath;

		ctx.noStroke();
		ctx.strokeWeight(4);

		for (size_t i = 0; i < pview.size; i++)
		{

			double vx = maths::map(pview.vertexData[i].x,
				emLeft, emRight,
				edgeMargin, edgeMargin + coordsWidth);

			double vy = maths::map(pview.vertexData[i].y,
				emBottom, emTop,
				edgeMargin + coordsHeight, edgeMargin );

			//printf("[%zd] CMD: %d PT: %f, %f\n",
			//	i, pview.commandData[i],
			//	vx, vy);
			
			switch (pview.commandData[i])
			{
			case BL_PATH_CMD_MOVE:
				dpath.moveTo(vx, vy);

				//ctx.fill(0,0,0);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 10);
				break;

			case BL_PATH_CMD_ON:
				// save pt in queue
				dpath.lineTo(vx,vy);
				//ctx.fill(255,0,0);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 2);
				break;

			case BL_PATH_CMD_QUAD:
				ctx.fill(0, 255, 0);
				//dpath.quadTo(p1, p2);
				dpath.lineTo(vx,vy);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 2);
				break;

			case BL_PATH_CMD_CUBIC:
				ctx.fill(0);
				dpath.lineTo(vx,vy);
				//dpath.cubicTo(p1, p2, p3);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 2);
				break;

			case BL_PATH_CMD_CLOSE:
				dpath.close();
				ctx.stroke(0);
				ctx.noFill();
				ctx.path(dpath);

				dpath.reset();
				// ctx.fill(0);
				break;

			}
		}



		ctx.pop();


	}

	void drawLabels(IGraphics &ctx)
	{
		// Position of graphic in the frame of parent

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
		// vertical axis
		ctx.line(zeroX, edgeMargin, zeroX, edgeMargin+coordsHeight);
		// Horizontal axis
		ctx.line(edgeMargin, zeroY, edgeMargin+coordsWidth, zeroY);

		// Bounding box
		// Area label
		ctx.noStroke();
		ctx.fill(0);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::TOP);
		ctx.textSize(16);
		ctx.text("Glyph Bounding Box", frameWidth() / 2.0, 0.0);

		// Coordinate labels
		// upper left
		ctx.noStroke();
		ctx.fill(0);
		ctx.textSize(edgeMargin * 0.75);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		ctx.vartext(edgeMargin / 2.0, frameHeight() - edgeMargin, "(%d, %d)",
			fDesignMetrics.glyphBoundingBox.x0, fDesignMetrics.glyphBoundingBox.y0);

		// upper right
		ctx.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
		ctx.vartext(frameWidth() - (edgeMargin / 2.0), 0.0, "(%d, %d)",
			fDesignMetrics.glyphBoundingBox.x1, fDesignMetrics.glyphBoundingBox.y1);

	}

	void drawSelf(IGraphics& ctx) override
	{
		ctx.push();

		// Do this first to get the right coordinate orientation
		drawLabels(ctx);

		drawGlyphOutline(ctx);

		ctx.pop();
	}

	virtual void mouseWheel(const MouseEvent& e) override
	{
		printf("emspaceview.mouseWheel\n");

	}

	virtual void keyEvent(const KeyboardEvent& e) override
	{
		switch (e.activity)
		{
		case KEYPRESSED:
			keyPressed(e);
			break;

		case KEYRELEASED:
			keyReleased(e);
			break;

		case KEYTYPED:
			keyTyped(e);
			break;
		}
	}


	void keyPressed(const KeyboardEvent& e) override
	{
		switch (e.keyCode)
		{
		case VK_RIGHT:
		{
			incrementGlyph();
		}
		break;

		case VK_LEFT:
		{
			decrementGlyph();
		}
		break;

		}
	}
};

#endif	// emspaceview_h
