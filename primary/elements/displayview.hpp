#pragma once

#include "graphic.hpp"
#include "elements/screensnapshot.hpp"
#include "elements/gmonitor.h"

struct DisplayView : public GraphicElement
{
	DisplayMonitor& fMonitor;
	ScreenSnapper fSnapper;

	DisplayView(DisplayMonitor& mon, const maths::bbox2f& aframe)
		:GraphicElement(aframe)
		, fMonitor{ mon }
	{
		fSnapper.reset(mon.frame(), mon.getDC());
		setBounds(maths::bbox2f{ {0,0},{frameWidth(),frameHeight()} });
	}

	void draw(IGraphics& ctx) override
	{
		ctx.push();

		float tx = frameX();
		float ty = frameY();

		ctx.translate(tx, ty);

		fSnapper.next();
		ctx.scaleImage(fSnapper.getImage(), 0, 0, fSnapper.width(), fSnapper.height(), 0, 0, frameWidth(), frameHeight());

		ctx.pop();
	}
};