#ifndef emspaceview_h
#define emspaceview_h

//
// Display the graphic details of a font glyph
//

#include "Graphics.h"
#include "blend2d.h"
#include "graphic.hpp"

struct EMSpaceView : public Graphic
{
	static constexpr int edgeMargin = 24;
	static constexpr int edgeTopMargin = 48;
	static constexpr int edgeBottomMargin = 24;


	int coordsWidth=0;
	int coordsHeight=0;


	BLFontFace fFontFace;
	BLFont fFont;
	BLFontDesignMetrics fDesignMetrics;
	
	uint16_t fGlyphCount;
	uint16_t fFirstGlyph;
	uint32_t fCurrentGlyph;


	EMSpaceView(int w, int h)
		:Graphic(0,0,w,h)
	{
		coordsWidth = width() - edgeMargin * 2;
		coordsHeight = height() - edgeMargin * 2;
	}

	void setFace(const BLFontFace& face)
	{
		fFontFace = face;

		fGlyphCount = face.faceInfo().glyphCount;
		fFirstGlyph = 0;
		fCurrentGlyph = fFirstGlyph;

		fDesignMetrics = fFontFace.designMetrics();

		fFont.createFromFace(fFontFace, fDesignMetrics.ascent+fDesignMetrics.descent);

	}

	void setGlyphId(uint16_t gid)
	{
		fCurrentGlyph = gid;
	}

	void drawBackground(IGraphics& ctx)
	{
		// Draw the frame outline of the whole space
		ctx.stroke(0,0,255);
		ctx.noFill();
		ctx.rect(0, 0, width(), height());
	}

	void drawGlyphOutline(IGraphics& ctx)
	{
		BLPath path;
		//uint32_t glyphId = 'A';


		BLResult err = fFont.getGlyphOutlines(fCurrentGlyph, path);

		if (err)
			return;

/*
		  BL_PATH_CMD_MOVE = 0,
  //! On-path command (interpreted as line-to or the end of a curve).
  BL_PATH_CMD_ON = 1,
  //! Quad-to control point.
  BL_PATH_CMD_QUAD = 2,
  //! Cubic-to control point (always used as a pair of commands).
  BL_PATH_CMD_CUBIC = 3,
  //! Close path.
  BL_PATH_CMD_CLOSE = 4,

  */
		double gbWidth = fDesignMetrics.glyphBoundingBox.x1 - fDesignMetrics.glyphBoundingBox.x0;
		double gbHeight = fDesignMetrics.glyphBoundingBox.y1 - fDesignMetrics.glyphBoundingBox.y0;


		ctx.push();
		//ctx.translate(edgeMargin + (coordsWidth / 2.0), edgeTopMargin+(coordsHeight / 2.0));
		ctx.scale(coordsWidth/gbWidth, -coordsHeight/gbHeight);

		// display the outline
		BLPathView pview = path.view();
		BLPath dpath;

		ctx.noStroke();
		ctx.strokeWeight(4);

		for (size_t i = 0; i < pview.size; i++)
		{
			printf("[%d] CMD: %d PT: %f, %f\n", 
				i, pview.commandData[i],
				pview.vertexData[i].x, pview.vertexData[i].y);


			switch (pview.commandData[i])
			{
			case BL_PATH_CMD_MOVE:
				dpath.moveTo(pview.vertexData[i]);
				//ctx.fill(0,0,0);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 10);
				break;

			case BL_PATH_CMD_ON:
				// save pt in queue
				dpath.lineTo(pview.vertexData[i]);
				//ctx.fill(255,0,0);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 2);
				break;

			case BL_PATH_CMD_QUAD:
				ctx.fill(0, 255, 0);
				//dpath.quadTo(p1, p2);
				dpath.lineTo(pview.vertexData[i]);
				//ctx.circle(pview.vertexData[i].x, pview.vertexData[i].y, 2);
				break;

			case BL_PATH_CMD_CUBIC:
				ctx.fill(0);
				dpath.lineTo(pview.vertexData[i]);
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
			fDesignMetrics.glyphBoundingBox.x0, fDesignMetrics.glyphBoundingBox.y0);

		// upper right
		ctx.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
		ctx.vartext(width() - (edgeMargin / 2.0), 0.0, "(%d, %d)",
			fDesignMetrics.glyphBoundingBox.x1, fDesignMetrics.glyphBoundingBox.y1);


		// Translate coordinate center to be middle of the area
		ctx.translate(edgeMargin + coordsWidth / 2.0, edgeMargin + coordsHeight / 2.0);

		// flip y coordinate axis to point positive upward
		ctx.scale(1.0, -1.0);

		// Whenever we do text now, we need to flip
		// coordinates temporarily
		//ctx.push();
		//ctx.scale(1.0, -1.0);
		//ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		//ctx.text("(0,0)", edgeMargin, edgeMargin);
		//ctx.pop();
	}

	void drawSelf(IGraphics& ctx)
	{
		
		ctx.push();

		// Do this first to get the right coordinate orientation
		drawLabels(ctx);

		drawGlyphOutline(ctx);

		ctx.pop();
	}
};

#endif	// emspaceview_h
