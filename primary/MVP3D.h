#pragma once


#include "grmath.h"

// Structure containing ModelView, Viewport, Projection
// to be used in 3D calculations.
struct MVP3D {
	mat4f fModelView;
	mat4f fProjection;
	mat4f fViewport;
};
