#include "p5.hpp"
#include "svgcolors.h"

using namespace p5;

void mouseDragged(const MouseEvent& e)
{
	printf("mouseDragged: %d, %d\n", e.x, e.y);
}


BLRgba32 colorFromName(const char* name)
{
	maths::vec4b c{ 127,127,127,255 };
	
	if (name == nullptr)
		return BLRgba32(0, 0, 0, 0);
	

	if (svg::colors.contains(name))
		c = svg::colors[name];

	return BLRgba32(c.r, c.g, c.b, c.a);
}

void testInheritance()
{
	BLGradient templateGrad{};
	templateGrad.addStop(0.10, colorFromName("gold"));
	templateGrad.addStop(0.95, colorFromName("red"));

	// Do a radial gradient

	BLRadialGradientValues values(5, 5, 5, 5, 4);
	BLGradient grad = templateGrad;
	grad.setValues(values);
	BLVar fillVar{};

	blVarAssignWeak(&fillVar, &grad);

	fill(fillVar);
	circle(5, 5, 4);
}

void testDirect()
{
	BLGradient radial{};
	BLRadialGradientValues values(mouseX, mouseY, 180, 180, 180);
	//templateGrad.setExtendMode(BL_EXTEND_MODE_PAD);
	//BLRadialGradientValues values(50, 50, mouseX,mouseY,10);
	//BLLinearGradientValues values(0, 0, 100, 100);
	
	radial.create(values);		// Must do this, NOT setValues() on initial creation
	radial.addStop(0.0, BLRgba32(0xFFFFFFFF));
	radial.addStop(1.0, BLRgba32(0xffff6F3F));

	gAppSurface->getBlend2dContext().setFillStyle(radial);
	//gAppSurface->getBlend2dContext().fillCircle(180, 180, 160);
	
	//BLVar fillVar{};
	//blVarAssignWeak(&fillVar, &radial);

	//ellipseMode(ELLIPSEMODE::RADIUS);

	//fill(fillVar);
	circle(180, 180, 160);
}

void draw()
{
	push();
	//scale(10, 10);
	background(255);

	//testInheritance();
	testDirect();
	
	pop();
}

void setup()
{
	createCanvas(400, 400);
	background(255);
}