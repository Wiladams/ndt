#pragma once

#include "IShader.h"
#include "TriangleMesh.h"
#include "MVP3D.h"


// A shader that allows you to apply a single
// color, instead of taking color values from a 
// diffusion map.
// The normal and uv maps still apply, except there's
// just a single color to modulate.
struct ColorShader : public IShader {
	BLRgba32 fColor;

	ColorShader(const BLRgba32 &c)
		:IShader(),
		fColor(c)
	{}

	// vertex shader
	virtual Vec4f vertex(int iface, int nthvert);

	// fragment shader
	virtual bool fragment(Vec3f bar, BLRgba32& color);
};

