#pragma once

#include "apphost.h"
#include "graphic.hpp"

#include <memory>

// A graphical Representation of a display monitor
// References
// https://learn.microsoft.com/en-us/windows/win32/gdi/using-multiple-monitors-as-independent-displays
//
struct GMonitor : public GraphicElement
{
	HMONITOR fMonitorHandle = nullptr;
	MONITORINFO fMonitorInfo;
	HDC fDC = nullptr;

	GMonitor()
		:fMonitorHandle(nullptr)
		, fMonitorInfo{}
		, fDC(nullptr)
	{
		;
	}

	bool reset(HMONITOR handle, HDC hdc)
	{
		fMonitorHandle = handle;
		fDC = hdc;

		ZeroMemory(&fMonitorInfo, sizeof(fMonitorInfo));
		fMonitorInfo.cbSize = sizeof(fMonitorInfo);

		GetMonitorInfoA(fMonitorHandle, &fMonitorInfo);

		setFrame({ {(float)fMonitorInfo.rcMonitor.left,(float)fMonitorInfo.rcMonitor.top},{(float)fMonitorInfo.rcMonitor.right,(float)fMonitorInfo.rcMonitor.bottom} });
		setBounds({ {0,0},{frameWidth(),frameHeight()} });

		return true;
	}

	HDC getMonitorDC() { return fDC; }

	void draw(IGraphics& ctx) override
	{
		// draw a simple rectangle
		// positioning it properly in the canvas
		ctx.fill(0x2F);
		ctx.stroke(255);
		ctx.strokeWeight(6);
		ctx.rect(frameX(), frameY(), frameWidth(), frameHeight());
	}

	void print()
	{
		//maths::bbox2f monbox{ {fMonitorInfo.rcMonitor.left,fMonitorInfo.rcMonitor.top},{fMonitorInfo.rcMonitor.right,fMonitorInfo.rcMonitor.bottom} };
		auto szmonbox = maths::size(frame());

		printf("============  MONITOR ==================\n");
		printf("RC Monitor: (%3.0f %3.0f %3.0f %3.0f)\n", frameX(), frameY(), frameWidth(), frameHeight());
		printf("RC Work: %d %d %d %d\n", fMonitorInfo.rcWork.left, fMonitorInfo.rcWork.top, fMonitorInfo.rcWork.right, fMonitorInfo.rcWork.bottom);
		printf("Flags: 0x%X\n", fMonitorInfo.dwFlags);
	}

	// Factory functions
	static BOOL CALLBACK enumMon(HMONITOR hmon, HDC hdc, LPRECT clipRect, LPARAM mons)
	{
		std::vector<std::shared_ptr<GMonitor>>* monitors = (std::vector<std::shared_ptr<GMonitor>>*)mons;
		std::shared_ptr<GMonitor> mon = std::make_shared<GMonitor>();
		mon->reset(hmon, hdc);

		monitors->push_back(mon);

		return TRUE;
	}

	// Generate a list of all the connected monitors
	// return the bounding box of their extents
	//
	static maths::bbox2f monitors(std::vector<std::shared_ptr<GMonitor>>& mons)
	{
		std::vector<HMONITOR> handles{};
		HDC hdc = ::GetDC(NULL);
		
		// First create a list of monitor handles
		auto bResult = EnumDisplayMonitors(hdc, NULL, enumMon, (LPARAM)(&handles));

		// Some error, so return zero monitors
		if (!bResult)
			return { {0,0},{0,0} };

		// accumulate the overall size in a bounding box
		maths::bbox2f vbox{ {0,0},{0,0} };

		for (auto& hmon : handles)
		{
			std::shared_ptr<GMonitor> mon = std::make_shared<GMonitor>();
			mon->reset(hmon);
			mons.push_back(mon);

			maths::expand(vbox, mon->frame());
		}

		return vbox;
	}

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
};

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