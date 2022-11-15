#pragma once

#include "apphost.h"
#include "graphic.hpp"

#include <memory>

struct DisplayDevice
{
	unsigned long fDevNum;
	DISPLAY_DEVICEA fDevInfo;

	DisplayDevice()
	{
		;
	}

	bool reset(unsigned long devNum)
	{
		fDevNum = devNum;


		memset(&fDevInfo, 0, sizeof(fDevInfo));
		fDevInfo.cb = sizeof(fDevInfo);
		DWORD dwFlags = 0;

		auto bResult = EnumDisplayDevicesA(nullptr, fDevNum, &fDevInfo, dwFlags);

		return true;
	}

	void print(const DISPLAY_DEVICEA& mon)
	{
		printf("%d\n", fDevNum);
		printf("    Device Name: %s\n", fDevInfo.DeviceName);
		printf("  Device String: %s\n", fDevInfo.DeviceString);
		printf("      Device ID: %s\n", fDevInfo.DeviceID);
		printf("     Device Key: %s\n", fDevInfo.DeviceKey);
		printf("    State Flags: 0x%d\n", fDevInfo.StateFlags);
		if (fDevInfo.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			printf("  ATTACHED");
		if (fDevInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			printf("  PRIMARY");
		printf("\n");
	}

	static void interfaces()
	{
		printf("========== INTERFACES ==========\n");
		DISPLAY_DEVICEA info;
		DWORD dwFlags = EDD_GET_DEVICE_INTERFACE_NAME;
		DWORD devNum=0;

		memset(&info, 0, sizeof(info));
		info.cb = sizeof(info);

		while (EnumDisplayDevicesA(nullptr, devNum, &info, dwFlags))
		{
			printf("Device ID: %s\n", info.DeviceID);
			printf("Device String: %s\n", info.DeviceString);
			printf("Device Name: %s\n", info.DeviceName);
			printf("Device Key: %s\n", info.DeviceKey);

			// on to the next one
			memset(&info, 0, sizeof(info));
			info.cb = sizeof(info);
			devNum++;
		}
	}
};
/*


void showDisplayDevices()
{

	DWORD dwFlags = 0;
	DWORD devNum = 0;
	maths::bbox2f b;

	// First get the interface name
	DISPLAY_DEVICEA dd{};
	memset(&dd, 0, sizeof(dd));
	dd.cb = sizeof(dd);

	auto bResult = EnumDisplayDevicesA(nullptr, devNum, &dd, dwFlags);

	if (!bResult)
	{
		printf("Could not find display interface");
		return;
	}

	printDevice(devNum, dd);


	devNum = 0;
	while (true) {
		DISPLAY_DEVICEA mon{};
		memset(&mon, 0, sizeof(mon));
		mon.cb = sizeof(mon);

		auto bResult = EnumDisplayDevicesA(dd.DeviceName, devNum, &mon, dwFlags);

		if (!bResult)
			break;

		printDevice(devNum, mon);

		devNum++;
	}
}
*/

