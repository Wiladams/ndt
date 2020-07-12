#pragma once

#include "gview.h"

class GWindow : public GView
{
	BLRect fClientArea;

public:
	GWindow(int x, int y, int w, int h)
		: GView(BLRect(x,y,w,h))
	{

	}
};