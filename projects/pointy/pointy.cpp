#include "p5.hpp"

using namespace p5;

/*
typedef struct tagPOINTER_DEVICE_INFO {
  DWORD               displayOrientation;
  HANDLE              device;
  POINTER_DEVICE_TYPE pointerDeviceType;
  HMONITOR            monitor;
  ULONG               startingCursorId;
  USHORT              maxActiveContacts;
  WCHAR               productString[POINTER_DEVICE_PRODUCT_STRING_MAX];
} POINTER_DEVICE_INFO;
*/
/*
typedef enum tagPOINTER_DEVICE_TYPE {
  POINTER_DEVICE_TYPE_INTEGRATED_PEN,
  POINTER_DEVICE_TYPE_EXTERNAL_PEN,
  POINTER_DEVICE_TYPE_TOUCH,
  POINTER_DEVICE_TYPE_TOUCH_PAD,
  POINTER_DEVICE_TYPE_MAX
} POINTER_DEVICE_TYPE;
*/
const char* pointerType(int aType)
{
	switch (aType) {
	case 0: return "POINTER_DEVICE_TYPE_INTEGRATED_PEN"; break;
	case 1: return "POINTER_DEVICE_TYPE_EXTERNAL_PEN"; break;
	case 2: return "POINTER_DEVICE_TYPE_TOUCH"; break;
	case 3: return "POINTER_DEVICE_TYPE_TOUCH_PAD"; break;
	case 4: return "POINTER_DEVICE_TYPE_MAX"; break;
	case 5: return "POINTER_DEVICE_TYPE"; break;
	}

	return "";
}

void printPointerDevice(const POINTER_DEVICE_INFO& info)
{
	printf("      Handle: %p\n", info.device);
	printf("        Type: %s\n", pointerType(info.pointerDeviceType));
	printf("Max Contacts: %d\n", info.maxActiveContacts);
	//wprintf(L"%s\n", info.productString);
}

void setup()
{
	createCanvas(800, 800);
	uint32_t deviceCount=0;
	POINTER_DEVICE_INFO* pointerDevices = nullptr;
	BOOL bResult = ::GetPointerDevices(&deviceCount, pointerDevices);

	printf("GetPointerDevices: %d\n", deviceCount);

	if (deviceCount < 1)
		return;

	// allocate some space
	pointerDevices = new POINTER_DEVICE_INFO[deviceCount];
	bResult = ::GetPointerDevices(&deviceCount, pointerDevices);

	for (int i = 0; i < deviceCount; i++)
	{
		printPointerDevice(pointerDevices[i]);
	}

	::RegisterPointerDeviceNotifications(gAppWindow->getHandle(), 1);

	delete[] pointerDevices;

	frameRate(1);
}

void draw()
{
	//noLoop();
}

void handlePointer(const PointerEvent &e)
{	
	printf("handlePointer: \n");
}
