#include "p5.hpp"
#include "stopwatch.hpp"
#include "layout.h"

#include "elements/screensnapshot.hpp"
#include "elements/textlabel.hpp"

#include <memory>
#include <iostream>

using namespace p5;

// A graphic to display a screen snapshot
struct ScreenSnapGraphic : public GraphicElement
{
	ScreenSnapper fSnapper{};

	ScreenSnapGraphic(const maths::bbox2f captureArea, const maths::bbox2f gframe, HDC srcDC = nullptr)
		:GraphicElement(gframe)
	{
		fSnapper.reset(captureArea, srcDC);
		fSnapper.next();
	}

	void drawSelf(IGraphics& ctx)
	{
		fSnapper.next();

		ctx.scaleImage(fSnapper.getImage(), 
			0, 0, (double)fSnapper.width(), (double)fSnapper.height(),
			boundsX(), boundsY(), boundsWidth(), boundsHeight());
	}
};

struct ScrollView : public Graphic
{
	ScrollView(const maths::bbox2f& box)
		:Graphic(box) {}

	virtual bool mouseWheel(const MouseEvent& e) override
	{
		float scrollSize = 4;

		printf("ScrollView::mouseWheel: %3.0f\n", e.delta);
		translateBoundsBy(0, e.delta * scrollSize);

		return true;
	}

	void scrollDown(float scrollSize = 4)
	{
		translateBoundsBy(0, -(scrollSize));
	}

	void scrollUp(float scrollSize=4)
	{
		translateBoundsBy(0, (scrollSize));
	}

	virtual bool keyEvent(const KeyboardEvent& e) override
	{
		printf("ScrollView::keyEvent()\n");
		if (e.keyCode == VK_UP)
			scrollUp();

		if (e.keyCode == VK_DOWN)
			scrollDown();

		return true;
	}

	virtual bool keyPressed(const KeyboardEvent& e) override
	{
		printf("ScrollView::keyPressed()\n");
		if (e.keyCode == VK_UP)
			scrollUp();

		if (e.keyCode == VK_DOWN)
			scrollDown();

		return true;
	}
};

struct CreditsView : public Graphic
{
	StopWatch fClock;
	maths::bbox2f fThanksFrame{};

	CreditsView(const maths::bbox2f &gframe, std::string title, std::vector<std::string>values)
		: Graphic(gframe)
	{
		// Setup the place where the message will be placed
		// This could be a GraphicElement
		fThanksFrame = { {0,0},{frameWidth(),48} };

		// Start with a vertical layout

		// create one element to hold the title
		auto tlabel = TextLabel::create(title.c_str(), 0, 0, 32);
		auto labelFrame = ILayoutGraphics::alignFrame(fThanksFrame, tlabel->frame(), 
			ILayoutGraphics::LayoutAlignment::CENTER, 
			ILayoutGraphics::LayoutAlignment::CENTER);
		tlabel->setFrame(labelFrame);
		tlabel->setTextFillColor(Pixel(255, 255, 255));

		// Create a vertical list of text labels for names
		auto nameGroup = std::make_shared<ScrollView>(maths::bbox2f{ {0,48},{frameWidth(),frameHeight()}});
		nameGroup->setLayout(std::make_shared<ColumnLayout>(nameGroup->frameWidth(),ILayoutGraphics::LayoutAlignment::CENTER, 4));

		//for (auto& value : values)
		for (auto &value : gFontHandler->familyNames())
		{
			auto alabel = TextLabel::create(value.c_str(), 0, 0, 24);
			alabel->setTextFillColor(Pixel(255, 255, 255));
			nameGroup->addChild(alabel);

			//printf("value: %s\n", value.c_str());
		}

		addChild(tlabel);
		addChild(nameGroup);
	}

	void drawBackground(IGraphics& ctx)
	{
		ctx.fill(127);
		ctx.rect(0, 0, boundsWidth(), boundsHeight());
	}
};

std::vector<std::string>names
{
	"William"
	,"Mubeen"
	,"Rahim"
	,"Raima"
};

void setup()
{
	//loadFontDirectory("c:\\windows\\fonts");
	
	std::cout << alignof(std::max_align_t) << '\n';
	std::cout << alignof(maths::vec2f) << '\n';

	createCanvas(1024, 768, "credits");

	auto sview = std::make_shared<ScreenSnapGraphic>(maths::bbox2f{ {0,0},{(float)displayWidth / 2, (float)displayHeight} }, maths::bbox2f{ {0,0},{800,(float)canvasHeight} });
	auto cview = std::make_shared<CreditsView>(maths::bbox2f{ {801,0},{(float)canvasWidth,(float)canvasHeight} },"THANKS",names);

	addWindow(sview);
	addWindow(cview);
}