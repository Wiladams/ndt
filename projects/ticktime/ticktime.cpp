#include "p5.hpp"
#include "ticktopic.h"
#include "secondtime.h"


#include <memory>

using namespace p5;


class TickDisplay : public Graphic
{
	SecondTime fClock;

public:
	TickDisplay()
		:Graphic(300, 0, 200, 144)
	{
	}

	void drawBackground(std::shared_ptr<IGraphics> ctx)
	{
		ctx->push();
		ctx->fill(160);
		ctx->noStroke();

		ctx->rect(0, 0, fFrame.w, fFrame.h);
		ctx->pop();
	}

	void drawForeground(std::shared_ptr<IGraphics> ctx)
	{
		ctx->push();
		ctx->fill(10, 10, 10, 70);
		ctx->rect(4, 24, fFrame.w - 8, fFrame.h - 48);
		ctx->pop();
	}

	void drawSelf(std::shared_ptr<IGraphics> ctx)
	{

		ctx->push();


		ctx->noStroke();


		ctx->textSize(18);

		ctx->fill(255);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->text("CHRONOGRAPH", 100, 20);

		ctx->fill(0, 220, 220);
		ctx->text(std::to_string(fClock.fHours).c_str(), 100, 44);
		ctx->text(std::to_string(fClock.fMinutes).c_str(), 100, 68);
		ctx->text(std::to_string(fClock.fSeconds).c_str(), 100, 92);
		ctx->text(std::to_string(fClock.fTenths).c_str(), 100, 116);
		ctx->pop();
	}

	void setSeconds(double d)
	{
		fClock.setSeconds(d);
	}
};


TickTopic ttopic(10);
std::shared_ptr<TickDisplay> _tdisplay=nullptr;




void tickSubscriber(const Topic<double>& p, double e)
{
	//std::cout << "tick: " << e << std::endl;
	if (nullptr != _tdisplay)
		_tdisplay->setSeconds(e);
	
	_tdisplay->draw(gAppSurface);
	gAppSurface->flush();



	screenRefresh();
}

void draw()
{
	clear();
}

void setup()
{
	// turn off looping timer
	noLoop();

	fullscreen();

	_tdisplay = std::make_shared<TickDisplay>();

	ttopic.setFrequency(10);
	ttopic.subscribe(tickSubscriber);
	ttopic.start();
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;
	}
}