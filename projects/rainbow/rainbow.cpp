#include "p5.hpp"

using namespace p5;

Color rColors[] = {
	{255,0,0,127},
	{255,165,0, 127},
	{255, 255,0,127},
	{0,255,0,127},
	{0,0,255,127},
	{93,118,203,127},
	{143,94,154,127},
};
int nColors = sizeof(rColors) / sizeof(Color);

BLGradient gradient(BLLinearGradientValues(0, 0, 0, 0));
BLImage potOfGold;

// Given an array of gradient stops, return the highest index that is
// lower than the specified offset
static inline size_t searchClosestLast(const BLGradientStop* array, size_t size, const double& value) noexcept 
{
	if (!size)
		return 0;

	const BLGradientStop* base = array;
	while (size_t half = size / 2u) {
		const BLGradientStop* middle = base + half;
		size -= half;
		if (middle[0].offset <= value)
			base = middle;
	}

	return size_t(base - array);
}

// Given a list of gradient stops
// return a color value at a specific
// offset
Color getGradientValue(const BLGradient &grad, double offset)
{
	auto stops = grad.stops();
	size_t nStops = grad.size();

	// ensure we're in the range 0..1 inclusive
	offset = constrain(offset, 0, 1.0);

	size_t lowIndex = searchClosestLast(stops, nStops, offset);
	size_t highIndex = lowIndex < nStops - 1 ? lowIndex + 1: lowIndex;

	//printf("lowIndex: %zd  highIndex: %zd\n", lowIndex, highIndex);
	Color from = BLRgba32(stops[lowIndex].rgba);
	Color to = BLRgba32(stops[highIndex].rgba);
	double f = map(offset, stops[lowIndex].offset, stops[highIndex].offset, 0, 1);

	return lerpColor(from, to, f);
}

void preload()
{
	// Create the gradient to represent the rainbow colors
	for (int i = 0; i < nColors; i++) {
		double offset = map(i, 0, nColors - 1, 0, 1);
		gradient.addStop(offset, rColors[i]);
	}

	// Load the various images
	auto err = potOfGold.readFromFile("potofgold.jpg");
	if (err)
		printf("could not load pot of gold (%d)\n", err);
}

void keyReleased(const KeyEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void draw()
{
	static const int baseSize = 400;


	if (isLayered()) {
		clear();
	} else
	{
		background(255);
	}


	// draw pot of gold at end of rainbow
	p5::image(potOfGold, width - potOfGold.width(), height - potOfGold.height());
	gAppSurface->flush();

	// Draw the actual rainbow
	int yoffset = 0;

	for (int x = 0; x <= baseSize; x++) {
		double offset = map(x, 0, baseSize, 0, 1);

		auto c = getGradientValue(gradient, offset);

		stroke(c);
		strokeWeight(1);
		
		int finalX = map(x, 0, baseSize, width, width - potOfGold.width());
		int finalY = height - potOfGold.height()+16;

		bezier(
			x, height, 
			width * 0.3, yoffset,
			width * 0.6, yoffset,
			finalX, finalY
		);

		/*
				bezier(
			x, height, 
			width * 0.3, yoffset,
			width * 0.6, yoffset,
			width - margin-x, height
		);
		*/
		yoffset += 1;
	}



	noLoop();
}

void setup()
{
	//createCanvas(1024, 768);
	createCanvas(displayWidth, displayHeight);
	layered();
	setWindowPosition(0, 0);
}