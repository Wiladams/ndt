#pragma once

#include "fonthandler.hpp"
#include "pubsub.h"
#include "graphic.hpp"


struct FontFaceSelector : public Graphic
{
	FontHandler &fHandler;

	FontFaceSelector(FontHandler& handler)
		:fHandler(handler)
	{
		refreshNames();
	}

	void refreshNames()
	{
		for (auto& name : fHandler.familyNames())
		{
			// construct a button for each name
			// in the enumeration
			//acons->putsln(name.c_str());
			printf("%s\n", name.c_str());
		}
	}


};