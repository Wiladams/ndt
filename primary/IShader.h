#pragma once

#include "geometry.h"
#include "blend2d.h"
#include "trianglemesh.h"
#include "MVP3D.h"


struct IVertexShader {
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual ~IVertexShader() {}
};

struct IFragmentShader {
	virtual bool fragment(Vec3f bar, BLRgba32& color) = 0;
	virtual ~IFragmentShader() {}
};

struct IShader : public IVertexShader, public IFragmentShader 
{
	virtual ~IShader() {};
	
	//virtual setModel(TriangleMesh &mesh);

	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, BLRgba32 &color) = 0;
};

