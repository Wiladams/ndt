#pragma once

#include "geometry.h"
#include "blend2d.h"

//using namespace geometry;

struct IShader {
	virtual ~IShader() {};
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, BLRgba32 &color) = 0;
};
