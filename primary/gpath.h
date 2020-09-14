#pragma once


#include "blend2d.h"
#include "graphic.hpp"

/*
	GPath

	Represents a path object.  This exists because the BLPath object
	of blend2d does not have relative vertex commands.  no curveBy,
	lineBy, etc.
*/
class GPath : public BLPath
{
	BLPoint fLastPoint;

public:
	bool contains(double x, double y)
	{
		return hitTest({ x,y }, BL_FILL_RULE_NON_ZERO) != 0;
	}

	// In this case, all parameters are relative to the last
	// to increments must accumulate
	void cubicBy(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		BLResult bResult = getLastVertex(&fLastPoint);

		cubicTo(fLastPoint.x + x1, fLastPoint.y + y1,
			fLastPoint.x + x1 + x2, fLastPoint.y + y1 + y2,
			fLastPoint.x + x1 + x2 + x3, fLastPoint.y + y1 + y2 + y3);
	}

	void lineBy(double x, double y)
	{
		BLResult bResult = getLastVertex(&fLastPoint);
		lineTo(fLastPoint.x + x, fLastPoint.y + y);
	}

	void moveBy(double x, double y)
	{
		BLResult bResult = getLastVertex(&fLastPoint);
		moveTo(fLastPoint.x + x, fLastPoint.y + y);
	}
};
