// view3d.cpp : Defines the exported functions for the DLL application.
//



#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"
#include "model.h"
#include "MonthTile.hpp"
#include "threed.h"

using namespace p5;

ThreeD *D = nullptr;


size_t modelIndex = 0;
GMesh *model = nullptr;
GMesh *floorModel = nullptr;

std::vector<GMesh *> models;

CalendarMonthTile mayTile(2020, 5, 8, 8);
CalendarMonthTile juneTile(2020, 6, 212, 8);

void loadModels()
{
	floorModel = ObjModel::loadModel("models/floor.obj");

	models.push_back(ObjModel::loadModel("models/african_head/african_head.obj"));
	models.push_back(ObjModel::loadModel("models/diablo3_pose/diablo3_pose.obj"));
	models.push_back(ObjModel::loadModel("models/boggie/body.obj"));
	//models.push_back(ObjModel::loadModel("models/suzanne.obj"));

	//models.push_back(new Model("obj/Vanquish/vanquish.obj"));

	model = models.at(0);
}


void keyReleased(const KeyEvent& e)
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

void keyPressed(const KeyEvent &e)
{

	switch (keyCode) {
	case VK_RIGHT:
	case VK_LEFT:
		if (keyCode == VK_LEFT) {
			D->moveCameraBy(0.2f, 0, 0);
		}
		else if (keyCode == VK_RIGHT) {
			D->moveCameraBy(-0.2f, 0, 0);
		}

		D->onCameraChange();

		break;

	case VK_UP:
	case VK_DOWN:
		if (keyCode == VK_UP) {
			D->moveCameraBy(0, 0.2f, 0);
		}
		else if (keyCode == VK_DOWN) {
			D->moveCameraBy(0, -0.2f, 0);
		}
		D->onCameraChange();
	}
}

void mouseWheel(const MouseEvent &e)
{
	if (mouseDelta > 0)
		D->moveCameraBy(0, 0, -0.2f);
	else if (mouseDelta < 0) 
		D->moveCameraBy(0, 0, 0.2f);

	mouseDelta = 0;
	D->onCameraChange();
}

void draw()
{
	if (isLayered())
		clear();
	else
		background(120);

	D->clearZBuffer();


	loadPixels();
	D->renderMesh(floorModel);
	D->renderMesh(model);
	updatePixels();

	mayTile.draw(gAppSurface);
	juneTile.draw(gAppSurface);
	//noLoop();
}

void setup()
{
	createCanvas(800, 800);
	//layered();
	setWindowPosition(48, displayHeight - height);
	D = new ThreeD(gAppSurface->getBlend2dImage());

	loadModels();
}
