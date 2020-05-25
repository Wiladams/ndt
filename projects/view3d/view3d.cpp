// view3d.cpp : Defines the exported functions for the DLL application.
//

#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"
#include "MonthTile.hpp"
#include "threed.h"
#include "waveobjloader.h"
#include "colorshader.hpp"
#include "flatshader.h"

using namespace p5;

ThreeD *D = nullptr;

struct ModelShader {
	TriangleMesh* fModel;
	IShader* fShader;

	ModelShader(TriangleMesh* model, IShader* shader)
		:fModel(model),
		fShader(shader)
	{}
};


size_t modelIndex = 0;
ModelShader *model = nullptr;
TriangleMesh* floorModel = nullptr;



std::vector<ModelShader *> models;

CalendarMonthTile mayTile(2020, 5, 8, 8);
CalendarMonthTile juneTile(2020, 6, 212, 8);




void loadModels()
{
	floorModel = aliaswave::loadModel("models/floor.obj");

	models.push_back(new ModelShader(aliaswave::loadModel("models/african_head/african_head.obj"), new DiffuseShader()));
	models.push_back(new ModelShader(aliaswave::loadModel("models/african_head/african_head.obj"), new ColorShader(BLRgba32(220, 190, 68))));
	models.push_back(new ModelShader(aliaswave::loadModel("models/african_head/african_head.obj"), new FlatShader(BLRgba32(220, 190, 68))));

	//models.push_back(new ModelShader(aliaswave::loadModel("models/diablo3_pose/diablo3_pose.obj"), new ColorShader(BLRgba32(220, 190, 68))));
	//models.push_back(new ModelShader(aliaswave::loadModel("models/boggie/body.obj"), new FlatShader(BLRgba32(220, 190, 68))));

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

	// Clear out zbuffer each time through
	D->clearZBuffer();

	DiffuseShader dshader;


	// Make sure there are no outstanding 2D calls
	// and get a handle in the pixel pointer
	loadPixels();
	
	D->renderMesh(floorModel, dshader);
	D->renderMesh(model->fModel, *model->fShader);

	updatePixels();

	// Draw whatever 2D stuff we want on top
	mayTile.draw(gAppSurface);
	juneTile.draw(gAppSurface);
}

void setup()
{
	createCanvas(800, 800);
	//layered();
	//setWindowPosition(48, displayHeight - height);
	D = new ThreeD(gAppSurface, width, height);

	loadModels();
}
