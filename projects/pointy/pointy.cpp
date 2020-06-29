#include "p5.hpp"

using namespace p5;

void setup()
{
	createCanvas(800, 800);
	uint32_t deviceCount=0;
	POINTER_DEVICE_INFO* pointerDevices = nullptr;
	BOOL bResult = ::GetPointerDevices(&deviceCount, pointerDevices);

	printf("GetPointerDevices: %d\n", deviceCount);

	noLoop();
}