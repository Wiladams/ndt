#pragma once

#pragma comment(lib, "winmm.lib")

#include <joystickapi.h>
#include "maths.hpp"
#include "uievent.h"


struct Joystick {
	unsigned int fID;		// The joystick id
	JOYINFOEX fInfo{};
	JOYCAPSA fCaps{};
	bool fIsValid;
	HWND fAttachedWindow;

	static bool isValid(UINT id)
	{
		JOYCAPSA caps;
		MMRESULT res = joyGetDevCapsA(id, &caps, sizeof(caps));

		return res == JOYERR_NOERROR;
	}

	Joystick(unsigned int id)
		:fID(id),
		fInfo(),
		fIsValid(false),
		fAttachedWindow(nullptr)
	{
		fInfo.dwSize = sizeof(JOYINFOEX);
		fIsValid = getCaps();
	}

	Joystick()
		:Joystick(JOYSTICKID1)
	{}

	bool isValid() const { return fIsValid; }
	bool getCaps()
	{
		return  joyGetDevCapsA(fID, &fCaps, sizeof(fCaps)) == JOYERR_NOERROR;
	}
	const char* name() const { return fCaps.szPname; }
	const char* regKey() const { return fCaps.szRegKey; }
	const char* oemVxD() const { return fCaps.szOEMVxD; }
	size_t numButtons() const { return fCaps.wNumButtons; }
	size_t numAxes() const { return fCaps.wNumAxes; }

	bool hasZ() const { return 0 != (fCaps.wCaps & JOYCAPS_HASZ); }
	bool hasR() const { return 0 != (fCaps.wCaps & JOYCAPS_HASR); }
	bool hasU() const { return 0 != (fCaps.wCaps & JOYCAPS_HASU); }
	bool hasV() const { return 0 != (fCaps.wCaps & JOYCAPS_HASV); }
	
	bool hasPOV() const { return 0 != (fCaps.wCaps & JOYCAPS_HASPOV); }
	bool hasPOV4DIR() const { return 0 != (fCaps.wCaps & JOYCAPS_POV4DIR); }
	bool hasPOVCTS() const { return 0 != (fCaps.wCaps & JOYCAPS_POVCTS); }


	bool getPosition(JoystickEvent& res)
	{
		JOYINFOEX info{0};
		info.dwSize = sizeof(info);
		info.dwFlags = JOY_RETURNALL;

		auto result = joyGetPosEx(fID, &info);
		if (result != JOYERR_NOERROR) {
			return false;
		}

		res.ID = fID;
		res.flags = info.dwFlags;

		// Map the relatively raw axes value 
		// to be within a range (-1.0 .. 1.0 typically)
		res.x = maths::Map(info.dwXpos, fCaps.wXmin, fCaps.wXmax, -1, 1);
		res.y = maths::Map(info.dwYpos, fCaps.wYmin, fCaps.wYmax, 1, -1);
		res.z = maths::Map(info.dwZpos, fCaps.wZmin, fCaps.wZmax, 1, 0); //throttle reverse
		res.r = maths::Map(info.dwRpos, fCaps.wRmin, fCaps.wRmax, -1, 1);
		res.u = maths::Map(info.dwUpos, fCaps.wUmin, fCaps.wUmax, -1, 1);
		res.v = maths::Map(info.dwVpos, fCaps.wVmin, fCaps.wVmax, -1, 1);
		
		res.buttons = info.dwButtons;
		res.numButtonsPressed = info.dwButtonNumber;

		if (info.dwPOV == 0xffff) {
			res.POV = -1;	// centered
		} else {
			res.POV = info.dwPOV / 100;
		}

		return true;
	}

	bool attachToWindow(HWND hWnd)
	{
		return (JOYERR_NOERROR == joySetCapture(hWnd,fID,0,1));
	}

	bool detachFromWindow()
	{
		return (JOYERR_NOERROR == joyReleaseCapture(fID));
	}
};