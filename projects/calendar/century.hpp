#pragma once

#include "graphic.hpp"
#include "decade.hpp"

//
// class Century
//
// Display 100 years at a time
//
class Century : public Graphic
{
	//int fBaseYear=2000;

public:
	Century(int baseYear) :
		Graphic()
	{
		setLayout(std::make_shared < VerticalLayout>());

		// Add decades to the graphic
		for (int year = baseYear; year < (baseYear + 100); year += 10) {
			addChild(std::make_shared<Decade>(year));
		}

		setFrame(bounds());

		//printf("Century\n");
		//printf("  Frame: %3.2f,%3.2f  %3.2f,%3.2f\n", frame().x, frame().y, frame().w, frame().h);
		//printf("  Bound: %3.2f,%3.2f  %3.2f,%3.2f\n", bounds().x, bounds().y, bounds().w, bounds().h);
	}
};
