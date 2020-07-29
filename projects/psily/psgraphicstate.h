#pragma once

#include "blend2d.h"
#include "psmatrix.h"
#include "pstypes.h"

/*
	// Device independent state
	CTM				array		Current Transformation matrix
	position		two numbers		Coordinates of current point
	path			(internal)		current path being built
	clipping path	(internal)	path defining current boundary
	clipping path stack
	color space
	color
	font
	line width
	line cap
	miter limit
	dash pattern
	stroke adjustment

	// Device dependent state
	color rendering		dictionary
	overprint			boolean
	black generation	procedure
	undercolor removal	procedure
	transfer			procedure
	halftone			procedure
	flatness			number
	smoothness			number
	device				(internal)
*/
struct PSGraphicState
{
	PSMatrix CTM;
	//PSStack ClippingPathStack;
	BLRgba32 Color;
	int ColorSpace;
	// DashPattern
	double Flat = 1;
	double LineWidth = 1.0;
	int LineCap = 0;
	int MiterLimit = 10.0;

	PSPoint Position;
	bool StrokeAdjustment = false;

	PSGraphicState()
	{
		CTM = PSMatrix::createIdentity();
		Position = PSPoint();
	}

	PSGraphicState(const PSGraphicState& rhs)
	{
		CTM = rhs.CTM;
		Position = rhs.Position;
		Color = rhs.Color;
		Flat = rhs.Flat;
		LineWidth = rhs.LineWidth;
		LineCap = rhs.LineCap;
		MiterLimit = rhs.MiterLimit;
		StrokeAdjustment = rhs.StrokeAdjustment;
	}
};