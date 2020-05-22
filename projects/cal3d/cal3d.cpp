#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"
#include "model.h"
#include "geometry.h"
#include "dproc_gl.h"
#include "diffuseshader.h"
#include "MonthTile.hpp"
#include "threed.h"

using namespace p5;

ThreeD* D = nullptr;


size_t modelIndex = 0;
GMesh* model = nullptr;
GMesh* floorModel = nullptr;

std::vector<GMesh*> models;

CalendarMonthTile mayTile(2020, 5, 8, 8);
CalendarMonthTile juneTile(2020, 6, 212, 8);

void setup()
{
	createCanvas(800, 800);
	D = new ThreeD(gAppSurface->getBlend2dImage());
	
	model->addVert({ -1,-1,-1 });
	model->addVert({ 1,-1,-1 });
	model->addVert({ 1,-1,0 });
}

void draw()
{
	D->renderMesh(model);
}

