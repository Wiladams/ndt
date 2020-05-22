#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"

#include "geometry.h"
#include "dproc_gl.h"
#include "diffuseshader.h"
#include "MonthTile.hpp"
#include "threed.h"
#include "trianglemesh.h"

using namespace p5;

ThreeD* D = nullptr;


size_t modelIndex = 0;
TriangleMesh model;




//CalendarMonthTile mayTile(2020, 5, 8, 8);
//CalendarMonthTile juneTile(2020, 6, 212, 8);

void setup()
{
	createCanvas(800, 800);
	D = new ThreeD(gAppSurface->getBlend2dImage());
	
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
	D->renderMesh(model);
}

