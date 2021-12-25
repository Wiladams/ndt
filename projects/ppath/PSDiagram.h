#pragma once

#include "blend2d.h"
#include "maths.hpp"
#include <memory>

class PSDiagram
{
	BLPoint fLastPosition;
	BLPoint fLastVertex;
	BLMatrix2D transformMatrix;
	BLPath fCurrentContour;
	BLPath fBasePath;

public:
	PSDiagram()
	{
		transformMatrix = BLMatrix2D::makeIdentity();
	}

	bool setCurrentPosition(double x, double y)
	{
		fLastPosition.x = x;
		fLastPosition.y = y;
	}

	//bool setBoundingBox();

	bool getLastVertex(BLPoint &vtxOut)
	{
		vtxOut = fLastVertex;

		return true;
	}

	bool attachContour(BLPath &aPath)
	{
		fBasePath.addPath(aPath);
	}

	bool attachCurrentContour()
	{
		attachContour(fCurrentContour);
		fCurrentContour.reset();
	}

	bool newContour()
	{
		attachContour(fCurrentContour);

		// initiate new current contour
		fCurrentContour.reset();
	}

	bool closeContour()
	{
		fCurrentContour.close();
		attachCurrentContour();
	}


	// Path construction
	bool moveTo(double x, double y)
	{
		// commit the last contour to the set of contours

		// reset the current position
		fLastPosition.x = x;
		fLastPosition.y = y;

		// begin a new contour
		newContour();

		fCurrentContour.moveTo(x, y);

		return true;
	}

	bool lineTo(double x, double y)
	{
		// add a line to the current contour
		fCurrentContour.lineTo(x, y);

		// reset the current position
		fLastPosition.x = x;
		fLastPosition.y = y;

		return true;
	}

	bool arc(double x, double y, double r, double angle1, double angle2)
	{
		auto sweep = maths::Radians(angle2 - angle1);
		auto bResult = fCurrentContour.arcTo(x, y, r, r, maths::Radians(angle1), sweep, false);
	
		fCurrentContour.getLastVertex(&fLastVertex);

	}

	bool curveTo(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		fCurrentContour.cubicTo(x1, y1, x2, y2, x3, y3);

		// commit current position
		fLastPosition.x = x3;
		fLastPosition.y = y3;
	}
	
	// commit a fill command to our drawing stack
	bool fill();

	// commit a stroking command to our drawing stack
	bool stroke();

	// Transforms to path
	// scale
	// rotate
	// translate
};