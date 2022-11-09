#include "p5.hpp"

using namespace p5;

void showDisplayDevices()
{
	DISPLAY_DEVICEA dd{};
	DWORD dwFlags = 0;
	DWORD devNum = 0;

	while (true) {
		memset(&dd, 0, sizeof(dd));
		dd.cb = sizeof(dd);

		auto bResult = EnumDisplayDevicesA(nullptr, devNum, &dd, dwFlags);
		
		
		if (!bResult)
			break;

		/*
		    DWORD  cb;
    CHAR   DeviceName[32];
    CHAR   DeviceString[128];
    DWORD  StateFlags;
    CHAR   DeviceID[128];
    CHAR   DeviceKey[128];
	*/
		printf("%d\n", devNum);
		printf("    Device Name: %s\n", dd.DeviceName);
		printf("  Device String: %s\n", dd.DeviceString);
		printf("      Device ID: %s\n", dd.DeviceID);
		printf("     Device Key: %s\n", dd.DeviceKey);
		printf("    State Flags: 0x%d\n", dd.StateFlags);
		if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			printf("  ATTACHED\n");
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			printf("  PRIMARY\n");

		devNum++;
	} 
}

void setup()
{
	createCanvas(640, 480, "displays");

	showDisplayDevices();
	
	noLoop();
}