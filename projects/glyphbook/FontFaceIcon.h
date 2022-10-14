#pragma once

#include "graphic.hpp"
#include "fonthandler.hpp"

struct FontFaceIcon : public Graphic, public Topic<std::string &>
{
	static maths::vec2f getPreferredSize() { return { 150,160 }; }

	std::string fFaceName;
	bool fIsHovering = false;

	FontFaceIcon(const char *facename)
		:Graphic(0, 0, getPreferredSize().x, getPreferredSize().y)
	{
		fFaceName.append(facename);
	}

	void mouseReleased(const MouseEvent& e)
	{
		notify(fFaceName);
	}

	void drawPlacard(IGraphics& ctx)
	{
		// If the mouse is hovering
		// draw some special hovering state

		ctx.strokeWeight(1);
		ctx.stroke(30);
		ctx.strokeJoin(BL_STROKE_JOIN_ROUND);
		ctx.fill(210);

		BLPath p;
		auto f = getFrame();
		p.moveTo(0, 0);
		p.lineTo(f.w - 30, 0);
		p.quadTo(f.w - 20, 0, f.w - 20, 20);

		p.quadTo(f.w, 20, f.w, 30);
		p.lineTo(f.w, 100);
		p.lineTo(0, 100);

		p.close();
		
		ctx.path(p);

		// Make a little triangle tab
		BLPath p2;
		ctx.stroke(30);
		ctx.fill(160);
		p2.moveTo(f.w - 30, 0);
		p2.lineTo(f.w, 30);
		ctx.path(p2);

	}

	void drawSampleText(IGraphics& ctx)
	{
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textFont(fFaceName.c_str());
		ctx.textSize(32);
		ctx.noStroke();
		ctx.fill(0);
		ctx.text("Abg", getFrame().w / 2.0, 68);
	}

	void drawName(IGraphics& ctx)
	{
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textFont("Consolas");
		ctx.textSize(12);
		ctx.text(fFaceName.c_str(), getFrame().w / 2.0, 140);
	}

	void drawBackground(IGraphics& ctx) override
	{
		drawPlacard(ctx);
		drawSampleText(ctx);
		drawName(ctx);
	}
};