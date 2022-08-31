#pragma once

#include "IShader.h"
#include "TriangleMesh.h"
#include "MVP3D.h"


// A shader that allows you to apply a single
// color, instead of taking color values from a 
// diffusion map.
// The normal and uv maps still apply, except there's
// just a single color to modulate.
struct ColorShader : public IShader 
{
	Pixel fColor;

	ColorShader(const Pixel &c)
		:IShader(),
		fColor(c)
	{}

	// vertex shader
	virtual vec4f vertex(int iface, int nthvert);

	// fragment shader
	virtual bool fragment(vec3f bar, BLRgba32& color);
};

