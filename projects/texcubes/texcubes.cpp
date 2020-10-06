#include "p5.hpp"
using namespace p5;

#include "arttypes.h"
#include "screensnapshot.hpp"

int gCanvasWidth = 512;
int gCanvasHeight = 384;


AVector camLoc(0.0f, 0.0f, 0.0f);
AVector camDir(-0.2f, 0.0f, 0.0f);
float fov = FOV_90;
std::shared_ptr<ACamera> cam;
std::shared_ptr<ACanvas> can;
auto cantex = std::make_shared<ScreenSnapshot>(0, 0, gCanvasWidth, gCanvasHeight);

ATmapCoord tc[4] = {
	ATmapCoord(0,0),
	ATmapCoord(0,0.999),
	ATmapCoord(0.999, 0.999),
	ATmapCoord(0.999, 0)
};



// cube vertices in local coordinates
float cubeLocal[] = {
	-50, -50, 50,
	50, -50, 50,
	50, -50, -50,
	-50,-50,-50,
	-50,50,50,
	50,50,50,
	50,50,-50,
	-50,50,-50
};

const size_t NUM_CUBE_FACES = 6;
const size_t NUM_CUBE_VERTICES = 8;
const size_t MAX_CUBES = 5;

APolygon cubeFaces[] = {
	{ 4,nullptr, {0,3,2,1},nullptr, cantex },
};

void setup()
{
	createCanvas(gCanvasWidth, gCanvasHeight);
}