#pragma once


#include "grmath.h"

// Structure containing ModelView, Viewport, Projection
// to be used in 3D calculations.
struct MVP3D {
	Matrix fModelView;
	Matrix fProjection;
	Matrix fViewport;
};
