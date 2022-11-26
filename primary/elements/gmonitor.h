#pragma once

#include "apphost.h"
#include "graphic.hpp"

#include <memory>

// A graphical Representation of a display monitor
// References
// https://learn.microsoft.com/en-us/windows/win32/gdi/using-multiple-monitors-as-independent-displays
//
struct DisplayMonitor
{
	HMONITOR fMonitorHandle = nullptr;
	HDC fDC = nullptr;					// Device context representing display monitor
	DEVMODEA   defaultMode{};

	maths::bbox2f fFrame{};
	std::string fDeviceName{};
	


	DisplayMonitor(HMONITOR hmon)
		: fMonitorHandle{ hmon }
	{
		reset(hmon);
	}

	bool reset(HMONITOR handle)
	{
		fMonitorHandle = handle;

		// Get the monitor info
		MONITORINFOEXA mInfo{};
		mInfo = { 0 };
		mInfo.cbSize = sizeof(mInfo);

		auto bResult = ::GetMonitorInfoA(fMonitorHandle, &mInfo);

		// If the call fails, we can't get monitor information
		// for some reason, so just bail
		if (0 == bResult)
			return false;

		// Preserve the boundary information
		fFrame = maths::bbox2f{ {(float)mInfo.rcMonitor.left,(float)mInfo.rcMonitor.top},{(float)mInfo.rcMonitor.right,(float)mInfo.rcMonitor.bottom} };

		// Create a device context for the monitor
		fDeviceName = mInfo.szDevice;

		// Get device mode information
		ZeroMemory(&defaultMode, sizeof(DEVMODEA));
		defaultMode.dmSize = sizeof(DEVMODEA);
		if (!EnumDisplaySettingsA(fDeviceName.c_str(), ENUM_REGISTRY_SETTINGS, &defaultMode))
		{
			OutputDebugString(L"Store default failed\n");
			return false;
		}

		return true;
	}

	bool readDeviceInfo()
	{

	}

	// This is the GDI DeviceContext
	// associated with the monitor
	HDC getDC()
	{
		if (fDC == NULL)
			fDC = CreateDCA(NULL, fDeviceName.c_str(), NULL, NULL);

		return fDC;
	}

	const maths::bbox2f& frame() const { return fFrame; }

	static size_t numberOfMonitors()
	{
		auto num = ::GetSystemMetrics(SM_CMONITORS);
		return num;
	}

	// virtualDisplayBox()
	// 
	// This will return the extent of the virtual display for 
	// the current desktop session. 
	//
	static maths::bbox2f virtualDisplayBox()
	{
		float x = (float)GetSystemMetrics(SM_XVIRTUALSCREEN);
		float y = (float)GetSystemMetrics(SM_YVIRTUALSCREEN);
		float dx = (float)GetSystemMetrics(SM_CXVIRTUALSCREEN);
		float dy = (float)GetSystemMetrics(SM_CYVIRTUALSCREEN);

		return { {x,y},{x + dx,y + dy} };
	}

	// Factory functions
// Although hdc is passed in, and will allow you 
// to draw on the monitor, it only lasts as long as this
// callback, then it's released.
// So, if we want to capture an HDC for the monitor, we
// need to create it separately.
	static BOOL CALLBACK enumMon(HMONITOR hmon, HDC hdc, LPRECT clipRect, LPARAM param)
	{
		std::vector<DisplayMonitor>* mons = (std::vector<DisplayMonitor> *)param;

		DisplayMonitor newmon(hmon);
		mons->push_back(newmon);

		return TRUE;
	}

	// Generate a list of all the connected monitors
	// return the bounding box of their extents
	//
	static maths::bbox2f monitors(std::vector<DisplayMonitor>& mons)
	{
		HDC hdc = ::GetDC(NULL);

		// First create a list of monitor handles
		auto bResult = EnumDisplayMonitors(hdc, NULL, enumMon, (LPARAM)(&mons));

		// Some error, so return zero monitors
		if (!bResult)
			return maths::invalidb2f;

		// accumulate the overall size in a bounding box
		maths::bbox2f vbox = virtualDisplayBox();

		return vbox;
	}

};




*/
/*
// https://learn.microsoft.com/en-us/windows/win32/gdi/enumeration-and-display-control?source=recommendations
void DetachDisplay()
{
	BOOL            FoundSecondaryDisp = FALSE;
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	LONG            Result;
	TCHAR           szTemp[200];
	int             i = 0;
	DEVMODE   defaultMode;

	// initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		ZeroMemory(&defaultMode, sizeof(DEVMODE));
		defaultMode.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings((LPSTR)DisplayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &defaultMode))
			OutputDebugString("Store default failed\n");

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			DEVMODE    DevMode;
			ZeroMemory(&DevMode, sizeof(DevMode));
			DevMode.dmSize = sizeof(DevMode);
			DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION
				| DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
			Result = ChangeDisplaySettingsEx((LPSTR)DisplayDevice.DeviceName,
				&DevMode,
				NULL,
				CDS_UPDATEREGISTRY,
				NULL);

			//The code below shows how to re-attach the secondary displays to the desktop

			//ChangeDisplaySettingsEx((LPSTR)DisplayDevice.DeviceName,
			//                       &defaultMode,
			//                       NULL,
			//                       CDS_UPDATEREGISTRY,
			//                       NULL);

		}

		// Reinit DisplayDevice just to be extra clean

		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	} // end while for all display devices
}

*/