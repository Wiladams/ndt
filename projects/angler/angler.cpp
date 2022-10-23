#include "p5.hpp"
#include "countdowntickclock.hpp"
#include "Recorder.hpp"

#include <memory>

using namespace p5;

float DURATION = 10*1;
std::shared_ptr<CountdownTickClock> indic=nullptr;
std::shared_ptr<Recorder> recorder = nullptr;


void draw()
{
	if (isLayered())
		clear();
	else
		background(92);

	indic->draw(*gAppSurface);

	if (recorder != nullptr)
		recorder->saveFrame();

	//noLoop();
}

// Implement onComposed(), because this is after draw()
// and after the windows have been composited.
// at this point, the framebuffer is complete before
// it will be displayed on the screen.
void onComposed()
{
	if (recorder != nullptr)
		recorder->saveFrame();
}

void setup()
{
	frameRate(10);

	createCanvas(320, 320, "angler");
	//layered();


	recorder = std::make_shared<Recorder>(gAppSurface, "angler-");

	//loadDefaultFonts();
	textFont("Consolas");
	
	//BLFontFace ff = loadFont("c:\\Windows\\Fonts\\consola.ttf");
	//gAppSurface->textFace(ff);

	//recorder->record();

	// create, and Start the animation
	indic = std::make_shared<CountdownTickClock>(DURATION);
	indic->start();
}


void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case 'R':	// toggle recording
		recorder->toggleRecording();
		break;

	case VK_ESCAPE:
		halt();
		break;

	case VK_SPACE:	// Restart the indicator
		indic->start();
		break;
	}
}
