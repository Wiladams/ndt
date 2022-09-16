// view3d.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"

#include "threed.h"
#include "stlcodec.h"
//#include "flatshader.h"

using namespace p5;

ThreeD* D = nullptr;

struct ModelShader {
	TriangleMesh* fModel;
	IShader* fShader;

	ModelShader(TriangleMesh* model, IShader* shader)
		:fModel(model),
		fShader(shader)
	{}
};


size_t modelIndex = 0;
ModelShader* model = nullptr;
TriangleMesh* floorModel = nullptr;

float viewAngle = 90;
float viewRadius = 100;
float viewHeight = 50;

std::vector<ModelShader*> models;


void loadModels()
{
	//TriangleMesh* mesh1 = stl::readFromFile("models/se2.stl");
	//TriangleMesh* mesh1 = stl::readFromFile("models/cube.stl");
	//TriangleMesh* mesh2 = stl::readFromFile("models/uno-3083g.stl");
	//TriangleMesh* mesh3 = stl::readFromFile("models/mp01.stl");
	TriangleMesh* mesh4 = stl::readFromFile("models/tubeconnector.stl");
	//TriangleMesh* mesh5 = stl::readFromFile("models/Seahorse.stl");
	//TriangleMesh* mesh6 = stl::readFromFile("models/bent_plate.stl");

	//models.push_back(new ModelShader(mesh1, new FlatShader(BLRgba32(220, 190, 68))));
	//models.push_back(new ModelShader(mesh2, new FlatShader(BLRgba32(220, 220, 220))));
	//models.push_back(new ModelShader(mesh3, new FlatShader(BLRgba32(220, 190, 68))));
	models.push_back(new ModelShader(mesh4, new FlatShader(BLRgba32(220, 190, 68))));
	//models.push_back(new ModelShader(mesh5, new FlatShader(BLRgba32(220, 190, 68))));
	//models.push_back(new ModelShader(mesh6, new FlatShader(BLRgba32(220, 190, 68))));

	model = models.at(0);
}


void changePosition(float angle, float radius, float height)
{
	//printf("changePosition: angle: %f, radius: %f, height: %f\n", angle, radius, height);

	float x = radius * cos(radians(angle));
	float z = radius * sin(radians(angle));
	D->moveCameraTo(x, height, z);
	
	if (nullptr != model)
		redraw();
}

void keyReleased(const KeyboardEvent& e)
{
	switch (keyCode) {
	case VK_SPACE:
		modelIndex = modelIndex + 1;

		if (modelIndex == models.size())
		{
			modelIndex = 0;
		}

		model = models.at(modelIndex);

		break;

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

void draw()
{
	if (isLayered())
		clear();
	else
		background(120);

	// Clear out zbuffer each time through
	D->clearZBuffer();

	// Make sure there are no outstanding 2D calls
	// and get a handle in the pixel pointer
	loadPixels();
	D->renderMesh(model->fModel, *model->fShader);
	updatePixels();

	//noLoop();
}

void setup()
{
	createCanvas(600, 600);
	//layered();
	//setCanvasPosition(48, displayHeight - height);

	D = new ThreeD(gAppSurface, canvasWidth, canvasHeight);
	changePosition(viewAngle, viewRadius, viewHeight);

	loadModels();
}

