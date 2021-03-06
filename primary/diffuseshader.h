#pragma once
#include "IShader.h"
#include "trianglemesh.h"
#include "MVP3D.h"

//
// DiffuseShader
// A typical shader that relies on having a diffusion map
// to determine the color of a pixel, then alters that
// based on lighting, orientation, etc.
struct DiffuseShader : public IShader {	

	DiffuseShader()
		:IShader()
	{}

	DiffuseShader(TriangleMesh *aModel, vec3f &light, MVP3D &mvp)
	{
		init(aModel, light, mvp);
	}


	// vertex shader
	virtual vec4f vertex(int iface, int nthvert);

	// fragment shader
	virtual bool fragment(vec3f bar, BLRgba32 &color);
};
