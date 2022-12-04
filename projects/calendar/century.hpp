#pragma once

#include "graphic.hpp"
#include "decade.hpp"

//
// class Century
//
// Display 100 years at a time
//
class Century : public GraphicGroup
{
	//int fBaseYear=2000;

public:
	Century(int baseYear) :
		GraphicGroup()
	{
		setLayout(std::make_shared < ColumnLayout>(100));

		// Add decades to the graphic
		for (int year = baseYear; year < (baseYear + 100); year += 10) {
			addGraphic(std::make_shared<Decade>(year));
		}

		setFrame(bounds());

		//printf("Century\n");
		//printf("  Frame: %3.2f,%3.2f  %3.2f,%3.2f\n", frame().x, frame().y, frame().w, frame().h);
		//printf("  Bound: %3.2f,%3.2f  %3.2f,%3.2f\n", bounds().x, bounds().y, bounds().w, bounds().h);
	}
};
