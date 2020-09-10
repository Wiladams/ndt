#include "p5.hpp"
#include "timertopic.h"

#include <memory>

using namespace p5;

class TickDisplay : public Graphic
{
	size_t fHours=0;
	size_t fMinutes=0;
	size_t fSeconds=0;
	size_t fTenths=0;

public:
	TickDisplay()
		:Graphic(0, 0, 200, 144)
	{
	}

	void drawBackground(IGraphics* ctx)
	{
		ctx->push();
		ctx->fill(160);
		ctx->noStroke();

		ctx->rect(0, 0, fFrame.w, fFrame.h);
		ctx->pop();
	}

	void drawForeground(IGraphics* ctx)
	{
		ctx->push();
		ctx->fill(10, 10, 10, 70);
		ctx->rect(4, 24, fFrame.w - 8, fFrame.h - 48);
		ctx->pop();
	}

	void drawSelf(IGraphics* ctx)
	{
		ctx->push();

		ctx->noStroke();


		ctx->textSize(18);

		ctx->fill(255);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->text("CHRONOGRAPH", 100, 20);

		ctx->fill(0, 220, 220);
		ctx->text(std::to_string(fHours).c_str(), 100, 44);
		ctx->text(std::to_string(fMinutes).c_str(), 100, 68);
		ctx->text(std::to_string(fSeconds).c_str(), 100, 92);
		ctx->text(std::to_string(fTenths).c_str(), 100, 116);
		ctx->pop();
	}

	void setSeconds(double d)
	{
		double gSecondsPerHour = 60 * 60;
		
		double hours = d / (gSecondsPerHour);
		fHours = (size_t)(hours);
		
		double minutes = (hours - fHours) * 60;
		fMinutes = (size_t)minutes;

		double seconds = (minutes - fMinutes) * 60;
		fSeconds = (size_t)seconds;

		double tenths = (seconds - fSeconds) * 10;
		fTenths = (size_t)tenths;
	}
};


TimerTopic ttopic(10);
std::shared_ptr<TickDisplay> _tdisplay=nullptr;




void tickSubscriber(const Topic<double>& p, double e)
{
	//std::cout << "tick: " << e << std::endl;
	if (nullptr != _tdisplay)
		_tdisplay->setSeconds(e);
	
	_tdisplay->draw(gAppSurface);
	gAppSurface->flush();

	windowRefresh();
}

void draw()
{
	clear();
	//_tdisplay->draw(gAppSurface);
}

void setup()
{
	// turn off looping timer
	noLoop();

	fullscreen();

	_tdisplay = std::make_shared<TickDisplay>();

	//auto win = window(0, 0, 200, 144);
	//win->addChild(_tdisplay);

	ttopic.subscribe(tickSubscriber);
	ttopic.start();

	//frameRate(20);
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