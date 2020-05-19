// view3d.cpp : Defines the exported functions for the DLL application.
//



#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"
#include "model.h"
#include "geometry.h"
#include "dproc_gl.h"
#include "diffuseshader.h"
#include "quaternion.h"
#include "MonthTile.hpp"

using namespace p5;

Matrix ModelView;
Matrix Projection; 
Matrix Viewport;

size_t modelIndex = 0;
GMesh *model = nullptr;
GMesh *floorModel = nullptr;

std::vector<GMesh *> models;
float *zbuffer = nullptr;

Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, 3);
Vec3f    center(0, 0, 0);
Vec3f        up(0, -1, 0);

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

void onCameraChange()
{
	ModelView = ogl_lookat(eye, center, up);
	Projection = ogl_projection(-1.0f / (eye - center).norm());
}

void renderMesh(GMesh *aModel)
{
	DiffuseShader shader(aModel, light_dir, ModelView, Projection, Viewport);
	//FlatShader shader(aModel, light_dir, ModelView, Projection, Viewport);

	for (int i = 0; i<aModel->nfaces(); i++) {
		for (int j = 0; j<3; j++) {
			auto vtx = shader.vertex(i, j);
			//printf("vtx: %3.2f, %3.2f, %3.2f, %3.2f\n", vtx[0], vtx[1], vtx[2], vtx[3]);
		}
		ogl_triangle(Viewport, shader.varying_tri, shader, gAppSurface->getBlend2dImage(), zbuffer);
	}
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
			eye.x += 0.2f;
		}
		else if (keyCode == VK_RIGHT) {
			eye.x -= 0.2f;
			}

		onCameraChange();

		break;

	case VK_UP:
	case VK_DOWN:
		if (keyCode == VK_UP) {
			eye.y += 0.2f;
		}
		else if (keyCode == VK_DOWN) {
			eye.y -= 0.2f;
		}
		onCameraChange();
	}
}

void mouseWheel(const MouseEvent &e)
{
	if (mouseDelta > 0) 
		eye.z -= 0.2f;
	else if (mouseDelta < 0) 
		eye.z += 0.2f;

	mouseDelta = 0;
	onCameraChange();
}

void draw()
{
	if (isLayered())
		clear();
	else
		background(120);

	ogl_clearzbuffer(zbuffer, width, height);

	loadPixels();
	renderMesh(floorModel);
	renderMesh(model);
	updatePixels();

	mayTile.draw(gAppSurface);
	juneTile.draw(gAppSurface);
	//noLoop();
}

void setup()
{
	createCanvas(800, 800);
	layered();
	setWindowPosition(48, displayHeight - height);

	zbuffer = new float[(size_t)width*height];
	loadModels();
	Viewport = ogl_viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	onCameraChange();
	light_dir = proj<3>((Projection*ModelView*embed<4>(light_dir, 0.f))).normalize();
}
