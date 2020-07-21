#pragma once

#include "gview.h"

class GWindow : public GView
{
protected:
	BLRect fClientArea;

public:
	GWindow(int x, int y, int w, int h)
		: GView(BLRect(x,y,w,h)),
		fClientArea(0,0,w,h)
	{

	}

};