#pragma once


#include "tinygl/3dMath.h"

// Structure containing ModelView, Viewport, Projection
// to be used in 3D calculations.
struct MVP3D {
	mat4 fModelView;
	mat4 fProjection;
	mat4 fViewport;
};
