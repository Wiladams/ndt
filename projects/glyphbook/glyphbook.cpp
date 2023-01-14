#include "p5.hpp"


#include "FontIconPage.h"
#include "elements/slider.h"

#include "fontsummary.hpp"

using namespace p5;

std::shared_ptr<FontIconPage> iconPage = nullptr;
std::shared_ptr<GWindow> iconWin = nullptr;
std::shared_ptr<GWindow> summaryWin = nullptr;
std::shared_ptr<FontSummary> summaryPage = nullptr;

// Action of the slider
void onSlide(const float pos)
{
	//printf("onSlide: %f\n", pos);
	float maxY = iconPage->boundsHeight() - iconWin->frameHeight();
	float transY = maths::map(pos, 0, 1, 0, maxY);

	iconPage->translateBoundsTo(0, -transY);
}


void draw()
{
	if (isLayered())
		clear();
	else
		background(0xC0);
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


void setup()
{
	//createCanvas(1920, 1024);
	//layered();
	fullscreen();

	loadFontDirectory("c:\\Windows\\Fonts");
	//loadFontDirectory("d:\\Fonts");

	// Create the window that will contain the font summary
// Create an initial font summary to go in the window
	summaryWin = window(200, 200, 1024, 768);
	summaryPage = std::make_shared<FontSummary>(0, 0, 1024, 768, "Consolas");
	summaryWin->addGraphic(summaryPage);


	// Create window that's going to hold the font icons
	iconWin = window(0, 0, 580, 800);
	//iconWin->setMoveable(false);
	iconPage = std::make_shared<FontIconPage>(0, 0, 560, 800);
	iconPage->subscribe([](std::string& name) {summaryPage->setFamily(name); });
	



	// Create a slider to go into the icon window
	auto sldr = Slider::create(
		{ float(iconWin->frameWidth() - 20), 40},
		{ float(iconWin->frameWidth() - 20), float(iconWin->frameHeight() - 8)});
	sldr->subscribe(onSlide);

	iconWin->addGraphic(iconPage);
	iconWin->addGraphic(sldr);

}