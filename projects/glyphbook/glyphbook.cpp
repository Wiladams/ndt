#include "p5.hpp"

#include "FontFaceIcon.h"
#include "FontIconPage.h"
#include "elements/slider.h"

#include "fontsummary.hpp"

using namespace p5;

std::shared_ptr<FontIconPage> page = nullptr;
std::shared_ptr<GWindow> iconWin = nullptr;
std::shared_ptr<GWindow> summaryWin = nullptr;
std::shared_ptr<FontSummary> summaryPage = nullptr;

// This is a functor that reacts to a face
// being selected.
struct FaceSelector
{
	std::shared_ptr<FontSummary> fPage=nullptr;

	FaceSelector() = default;
	FaceSelector(std::shared_ptr<FontSummary> page) : fPage{ page } {;}

	void setPage(std::shared_ptr<FontSummary> page){fPage = page;}

	void operator()(const std::string& facename) {fPage->setFamily(facename);}
};

void onSlide(const float pos)
{
	//printf("onSlide: %f\n", pos);
	float maxY = page->boundsHeight() - iconWin->frameHeight();
	float transY = maths::map(pos, 0, 1, 0, maxY);

	page->translateBoundsTo(0, -transY);

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

FaceSelector selectAFace;

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
	iconWin->setMoveable(false);
	page = std::make_shared<FontIconPage>(0, 0, 560, 800);
	selectAFace.setPage(summaryPage);
	page->subscribe(selectAFace);

	//page->subscribe(onSelectFace);
	//page->subscribe(*summaryPage);


	// Create a slider to go into the icon window
	auto sldr = Slider::create(
		{ float(iconWin->frameWidth() - 20), 40},
		{ float(iconWin->frameWidth() - 20), float(iconWin->frameHeight() - 8)});
	sldr->subscribe(onSlide);

	iconWin->addGraphic(page);
	iconWin->addGraphic(sldr);

}