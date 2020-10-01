#include "p5.hpp"
using namespace p5;

#include "arttypes.h"
#include "screensnapshot.hpp"

int gCanvasWidth = 640;
int gCanvasHeight = 480;


AVector camLoc(0.0f, 0.0f, 0.0f);
AVector camDir(-0.2f, 0.0f, 0.0f);
float fov = FOV_90;
std::shared_ptr<ACamera> cam;
std::shared_ptr<ACanvas> can;
//auto cantex = std::make_shared<SolidColorTexture>(0, 0, 1);		// solid color texture
auto cantex = std::make_shared<ScreenSnapshot>(0, 0, gCanvasWidth, gCanvasHeight);



APolyVertex squareScreen[4];
AVector squareView[4];
APolygon square = { 4,squareScreen, {0,1,2,3},nullptr, cantex };
float squareWorld[] = {
	50,-50,-400,
	50,50,-400,
	-50,50,-400,
	-50,-50,-400
};




void printMatrix(const AMatrix& m)
{
	printf("%3.2f  %3.2f  %3.2f\n", m.m[0], m.m[1], m.m[2]);
	printf("%3.2f  %3.2f  %3.2f\n", m.m[3], m.m[4], m.m[5]);
	printf("%3.2f  %3.2f  %3.2f\n", m.m[6], m.m[7], m.m[8]);
}

void draw()
{
	clear();
	can->clear();
	cantex->next();

	// Rotate camera
	AVector camDir = cam->direction;
	camDir.z += 0.05f;
	cam->setDirection(camDir);
	AVector camLoc = cam->location;
	camLoc.z -= 1;
	cam->setLocation(camLoc);

	// Transform & project polygon to screen coords
	for (size_t i = 0; i < 4; i++)
	{
		cam->worldToView(*(AVector*)&squareWorld[i * 3], squareView[i]);
		if (!cam->project(squareView[i], squareScreen[i]))
		{
			// halt if nothing to draw
			//halt();
		}
	}

	// Draw polygon on canvas
	can->drawFlatConvexPolygon(square);
	can->sync();

	// blit the canvas to the screen
	image(can->getImage(), 0, 0);
}

void test_matrix()
{
	AVector v(1, 1, 1);
	AMatrix m;
	m.setIdentity();
	printf("Identity\n");
	printMatrix(m);

	m.scaleXYZ({ 10,20,30 });
	AVector vresult = v * m;

	printf("result: [%3.2f, %3.2f, %3.2f]\n", vresult.x, vresult.y, vresult.z);

}

void setup() 
{
	createCanvas(gCanvasWidth, gCanvasHeight);
	frameRate(30);
	cam = std::make_shared<ACamera>(camLoc, camDir, gCanvasWidth, gCanvasHeight, fov);
	can = std::make_shared<ACanvas>(gCanvasWidth, gCanvasHeight);
	can->setTexture(cantex);
}