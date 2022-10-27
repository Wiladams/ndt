#pragma once

#include "graphic.hpp"
#include "yearofmonths.h"

//	class Decade
//
//	Display 10 years at a time
//
class Decade : public Graphic
{
	int fBaseYear = 2022;

public:
	Decade(int baseYear)
		:fBaseYear(baseYear)
	{
		// set a horizontal layout
		setLayout(std::make_shared<HorizontalLayout>());

		// Add children to the graphic
		for (int year = baseYear; year < baseYear + 10; year++)
		{
			addChild(std::make_shared<YearOfMonths>(year));
		}

		auto b = bounds();
		printf("Decade bounds: %3.2f, %3.2f  %3.2f, %3.2f\n", b.min.x, b.min.y, b.max.x, b.max.y);

		setFrame(bounds());

		//printf("Decade\n");
		//printf("  Frame: %3.2f,%3.2f  %3.2f,%3.2f\n", frame().x, frame().y, frame().w, frame().h);
		//printf("  Bound: %3.2f,%3.2f  %3.2f,%3.2f\n", bounds().x, bounds().y, bounds().w, bounds().h);

		//auto sz = YearOfMonths::preferredSize();
		//setFrame({ 0,0,(double)(sz.x*10),(double)(sz.y) });
	}
};
