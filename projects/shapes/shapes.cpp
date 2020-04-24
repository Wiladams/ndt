#include "p5.hpp"

void shape1()
{
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
	beginShape(SHAPEMODE::POINTS);
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape();
}

void shape3()
{
	beginShape(SHAPEMODE::LINES);
	vertex(30, 20);
	vertex(85, 20);
	vertex(85, 75);
	vertex(30, 75);
	endShape();
}

void shape4()
{
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

void draw()
{
	background(0xc0);
	stroke(0);

	push();
	scale(1, 1);

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

	pop();
}

void setup()
{
	createCanvas(800, 600);
}