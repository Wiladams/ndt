#pragma once

#include "graphic.hpp"
//#include "fonthandler.hpp"

struct FontFaceIcon : public GraphicElement, public Topic<std::string &>
{
	static maths::vec2f getPreferredSize() { return { 150,160 }; }

	std::string fFaceName{};
	bool fIsHovering = false;
	BLPath path1;
	BLPath p2;


	FontFaceIcon(const char *facename)
		:GraphicElement(0, 0, getPreferredSize().x, getPreferredSize().y)
	{
		fFaceName.append(facename);

		createPaths();
	}

	void mouseReleased(const MouseEvent& e)
	{
		notify(fFaceName);
	}

	void createPaths()
	{
		// First path is the major part
		// of the placard
		path1.moveTo(0, 0);
		path1.lineTo(boundsWidth() - 30, 0);
		path1.quadTo(frameWidth() - 20, 0, frameWidth() - 20, 20);

		path1.quadTo(boundsWidth(), 20, boundsWidth(), 30);
		path1.lineTo(boundsWidth(), 100);
		path1.lineTo(0, 100);

		path1.close();

		// This is the little dog-ear on the
		// corner of the placard
		p2.moveTo(boundsWidth() - 30, 0);
		p2.lineTo(boundsWidth(), 30);
	}

	void drawPlacard(IGraphics& ctx)
	{
		// If the mouse is hovering
		// draw some special hovering state

		// Main part of placard
		ctx.strokeWeight(1);
		ctx.stroke(30);
		ctx.strokeJoin(BL_STROKE_JOIN_ROUND);
		ctx.fill(210);
		ctx.path(path1);

		// Make a little triangle tab
		ctx.stroke(30);
		ctx.fill(160);
		ctx.path(p2);
	}

	void drawSampleText(IGraphics& ctx)
	{
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textFont(fFaceName.c_str());
		ctx.textSize(32);
		ctx.noStroke();
		ctx.fill(0);
		ctx.text("Abg", boundsWidth() / 2.0f, 68);
	}

	void drawName(IGraphics& ctx)
	{
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textFont("Consolas");
		ctx.textSize(12);
		ctx.text(fFaceName.c_str(), boundsWidth() / 2.0, 140);
	}

	void drawSelf(IGraphics& ctx) override
	{
		drawSampleText(ctx);
		drawName(ctx);
	}

	void drawBackground(IGraphics& ctx) override
	{
		drawPlacard(ctx);
	}
};