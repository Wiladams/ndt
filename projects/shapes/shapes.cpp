#include "p5.hpp"
#include "graphic.hpp"

using namespace p5;

void shape1()
{
	stroke(0);
	fill(255);
	beginShape();
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape(SHAPEEND::CLOSE);
}

void shape2()
{
	stroke(0);
	beginShape(SHAPEMODE::POINTS);
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape();
}

void shape3()
{
	stroke(0);
	beginShape(SHAPEMODE::LINES);
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape();
}

void shape4()
{
	stroke(0);
	noFill();
	beginShape();
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape();
}

void shape5()
{
	stroke(0);
	noFill();
	beginShape();
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape(SHAPEEND::CLOSE);
}

void shape6()
{
	stroke(0);
	fill(255);
	beginShape(SHAPEMODE::TRIANGLES);
	vertex(30, 75);
	vertex(40, 20);
	vertex(50, 75);
	vertex(60, 20);
	vertex(70, 75);
	vertex(80, 20);
	endShape();
}

void shape7()
{
	stroke(0);
	fill(255);
	beginShape(SHAPEMODE::TRIANGLE_STRIP);
	vertex(30, 75);
	vertex(40, 20);
	vertex(50, 75);
	vertex(60, 20);
	vertex(70, 75);
	vertex(80, 20);
	vertex(90, 75);
	endShape();
}

void shape8()
{
	stroke(0);
	fill(255);
	beginShape(SHAPEMODE::TRIANGLE_FAN);
	vertex(57.5, 50);
	vertex(57.5, 15);
	vertex(92, 50);
	vertex(57.5, 85);
	vertex(22, 50);
	vertex(57.5, 15);
	endShape();
}

void shape9()
{
	stroke(0);
	fill(255);
	beginShape(SHAPEMODE::QUADS);
	vertex(30, 20);
	vertex(30, 75);
	vertex(50, 75);
	vertex(50, 20);
	vertex(65, 20);
	vertex(65, 75);
	vertex(85, 75);
	vertex(85, 20);
	endShape();
}

void shape10()
{
	stroke(0);
	fill(255);
	beginShape(SHAPEMODE::QUAD_STRIP);
	vertex(30, 20);
	vertex(30, 75);
	vertex(50, 20);
	vertex(50, 75);
	vertex(65, 20);
	vertex(65, 75);
	vertex(85, 20);
	vertex(85, 75);
	endShape();
}

void shape11()
{
	stroke(0);
	fill(255);
	beginShape();
	vertex(20, 20);
	vertex(40, 20);
	vertex(40, 40);
	vertex(60, 40);
	vertex(60, 60);
	vertex(20, 60);
	endShape(SHAPEEND::CLOSE);
}

class TabbedView : public Graphic {
	int fTabHeight;
	int fTabOffset;
	int fTabLength;
	int fTabRadius;
	BLRect fFrame;
	BLPath fPath;

public:
	TabbedView(const BLRect &frame, int tabHeight, int tabOffset, int tabLength, int tabRadius)
		: fFrame(frame),
		fTabHeight(tabHeight),
		fTabOffset(tabOffset),
		fTabLength(tabLength),
		fTabRadius(tabRadius)
	{
		fPath.moveTo(fFrame.x, fFrame.y+tabHeight);
		fPath.lineTo(fFrame.x+tabOffset, tabHeight);
		fPath.cubicTo(fFrame.x + tabOffset + tabRadius, tabHeight, fFrame.x + tabOffset + tabRadius, 0, fFrame.x + tabOffset + tabRadius + tabRadius, 0);
		fPath.lineTo(fFrame.x + tabOffset + tabRadius * 2 + tabLength, 0);
		fPath.cubicTo(fFrame.x + tabOffset + tabRadius * 2 + tabLength + tabRadius, 0, fFrame.x + tabOffset + tabRadius * 2 + tabLength + tabRadius, tabHeight, fFrame.x + tabOffset + tabRadius * 2 + tabLength + tabRadius + tabRadius, tabHeight);
		fPath.lineTo(fFrame.x+fFrame.w, tabHeight);
		fPath.lineTo(fFrame.x + fFrame.w, tabHeight + fFrame.h - tabHeight);
		fPath.lineTo(fFrame.x, tabHeight + fFrame.h - tabHeight);
		fPath.close();
	}

	void drawBackground(IGraphics* ctx)
	{
		ctx->path(fPath);
	}
};

void shape12()
{
	// https://flatuicolors.com/palette/us
	TabbedView tview1(BLRect(8, 0, width - 16, 200), 24, 16, 96, 4);
	TabbedView tview2(BLRect(8, 0, width-16, 200), 24, 48, 96, 10);
	TabbedView tview3(BLRect(8, 0, width - 16, 200), 24, 120, 96, 10);
	TabbedView tview4(BLRect(8, 0, width - 16, 200), 24, 210, 96, 10);

	stroke(0);
	strokeWeight(1.0);
	fill(178, 190, 195);
	tview1.draw(gAppSurface);

	fill(255, 118, 117);
	tview2.draw(gAppSurface);

	fill(0, 184, 148);
	tview3.draw(gAppSurface);

	stroke(0);
	fill(9, 132, 227);
	tview4.draw(gAppSurface);
}

void draw()
{
	background(0xc0);
	stroke(0);

	push();
	scale(1, 1);

	// First Row
	push();
	shape1();

	translate(100, 0);
	shape2();

	translate(100, 0);
	shape3();

	translate(100, 0);
	shape4();

	pop();


	// Second row
	push();
	translate(0, 100);
	shape5();

	translate(100, 0);
	shape6();

	translate(100, 0);
	shape7();

	translate(100, 0);
	shape8();
	pop();

	push();
	translate(0, 200);
	shape9();

	translate(100, 0);
	shape10();

	translate(100, 0);
	shape11();
	pop();

	// Third Row
	push();
	translate(0, 300);
	shape12();
	pop();

	pop();
}

void setup()
{
	createCanvas(800, 600);
}