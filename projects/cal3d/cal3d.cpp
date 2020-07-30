#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"

#include "cube.h"
#include "MonthTile.hpp"
#include "threed.h"
#include "trianglemesh.h"
#include "flatshader.h"

using namespace p5;

ThreeD* D = nullptr;


size_t modelIndex = 0;
TriangleMesh model;
cube c1;

float viewAngle = 90;
float viewRadius = 100;
float viewHeight = 50;


//CalendarMonthTile mayTile(2020, 5, 8, 8);
//CalendarMonthTile juneTile(2020, 6, 212, 8);

// Handle changing position for whatever reason
void changePosition(float angle, float radius, float height)
{
	//printf("changePosition: angle: %f, radius: %f, height: %f\n", angle, radius, height);

	float x = radius * cos(radians(angle));
	float z = radius * sin(radians(angle));
	D->moveCameraTo(x, height, z);

}


void setup()
{
	createCanvas(800, 800);
	D = new ThreeD(gAppSurface, width, height);
	changePosition(viewAngle, viewRadius, viewHeight);

	// Vertices
	model.addVertex({ -1,-1,-1 });
	model.addVertex({ 1,-1,-1 });
	model.addVertex({ 1,-1,1 });
	model.addVertex({ -1,-1,1 });

	// Normals
	model.addNormal({ 0,1,0 });
	model.addNormal({ 0,1,0 });
	model.addNormal({ 0,1,0 });
	model.addNormal({ 0,1,0 });

	// UV
	model.addUV({ 0,0 });
	model.addUV({ 1,0 });
	model.addUV({ 1,1 });
	model.addUV({ 0,1 });

	// two faces in a quad
	model.addFace({ 0,1,2 }, { 0,1,2 }, { 0,1,2 });
	model.addFace({ 0,2,3 }, { 0,2,3 }, { 0,2,3 });

}

void draw()
{
	FlatShader shader(BLRgba32(220, 190, 68));
	
	background(120);
	D->clearZBuffer();

	loadPixels();
	//D->renderMesh(&model, shader);
	//D->renderMesh(&c1, shader);
	updatePixels();

}

void keyReleased(const KeyboardEvent& e)
{
	switch (keyCode) {
		case VK_ESCAPE:
			halt();
		break;
	}
}

void keyPressed(const KeyboardEvent& e)
{
	switch (keyCode) {
	case VK_RIGHT: {
		viewAngle += 10;
		auto loc = D->getCameraLocation();
		changePosition(viewAngle, viewRadius, loc.y);
	}
				 break;

	case VK_LEFT: {
		viewAngle -= 10;
		auto loc = D->getCameraLocation();
		changePosition(viewAngle, viewRadius, loc.y);
	}
				break;

	case VK_UP: {
		viewHeight += 10;
		changePosition(viewAngle, viewRadius, viewHeight);
	}
			  break;
	case VK_DOWN: {
		viewHeight -= 10;
		changePosition(viewAngle, viewRadius, viewHeight);
	}
				break;
	}
}

void mouseWheel(const MouseEvent& e)
{
	if (mouseDelta > 0) {
		viewRadius -= 50;
		changePosition(viewAngle, viewRadius, viewHeight);
	}
	else if (mouseDelta < 0) {
		viewRadius += 50;
		changePosition(viewAngle, viewRadius, viewHeight);
	}

	mouseDelta = 0;

}


