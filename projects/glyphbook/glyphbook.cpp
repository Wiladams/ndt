#include "p5.hpp"

#include "FontFaceIcon.h"
#include "FontIconPage.h"
#include "slider.h"
#include "fontsummary.hpp"

using namespace p5;

std::shared_ptr<FontIconPage> page = nullptr;
std::shared_ptr<GWindow> iconWin = nullptr;
std::shared_ptr<GWindow> summaryWin = nullptr;
std::shared_ptr<FontSummary> summaryPage = nullptr;

void onSlide(const float pos)
{
	//printf("onSlide: %f\n", pos);
	float maxY = page->bounds().h - iconWin->frame().h;
	float transY = maths::map(pos, 0, 1, 0, maxY);

	page->translateBoundsTo(0, -transY);

}

void onSelectFace(const std::string& facename)
{
	//printf("onSelectFace: %s\n", facename.c_str());
	summaryPage->setFamily(facename);
}

void draw()
{
	if (isLayered())
		clear();
	else
		background(0xC0);

}

void setup()
{
	createCanvas(1920, 1024);
	//layered();
	//fullscreen();

	loadFontDirectory("c:\\Windows\\Fonts");

	// Create the window that will contain the font summary
// Create an initial font summary to go in the window
	summaryWin = window(200, 200, 1024, 768);
	summaryPage = std::make_shared<FontSummary>(0, 0, 1024, 768, "Consolas");
	summaryWin->addChild(summaryPage);


	// Create window that's going to hold the font icons
	iconWin = window(0, 0, 580, 800);
	page = std::make_shared<FontIconPage>(0, 0, 560, 800);
	page->subscribe(onSelectFace);
	//page->subscribe(*summaryPage);


	// Create a slider to go into the icon window
	auto sldr = Slider::create(
		{ float(iconWin->width() - 20), 40 },
		{ float(iconWin->width() - 20), float(iconWin->height() - 8) });
	sldr->subscribe(onSlide);

	iconWin->addChild(page);
	iconWin->addChild(sldr);

}