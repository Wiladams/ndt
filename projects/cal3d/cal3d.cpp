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
GMesh* model = nullptr;
GMesh* floorModel = nullptr;

std::vector<GMesh*> models;
float* zbuffer = nullptr;

Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, 3);
Vec3f    center(0, 0, 0);
Vec3f        up(0, -1, 0);

CalendarMonthTile mayTile(2020, 5, 8, 8);
CalendarMonthTile juneTile(2020, 6, 212, 8);

void setup()
{
	createCanvas(800, 800);
}

void draw()
{

}

