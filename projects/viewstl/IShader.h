#pragma once

#include "blend2d.h"
#include "grmath.h"
#include "trianglemesh.h"
#include "MVP3D.h"


struct IVertexShader {
	mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
	mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
	mat<3, 3, float> varying_nrm; // normal per vertex to be interpolated by FS
	mat<3, 3, float> ndc_tri;     // triangle in normalized device coordinates


	virtual vec4 vertex(int iface, int nthvert) = 0;
	virtual ~IVertexShader() {}
};

struct IFragmentShader {
	virtual bool fragment(vec3 bar, BLRgba32& color) = 0;
	virtual ~IFragmentShader() {}

	// 
// This exists here because we're using BLRgba32 for color, which
// does not have a convenient multiply operator on it.
	inline static BLRgba32 colormul(const BLRgba32& c, float intensity)
	{
		intensity = (float)maths::Clamp(intensity, 0, 1);
		return { (uint32_t)(c.r() * intensity), (uint32_t)(c.g() * intensity), (uint32_t)(c.b() * intensity)};
	}
};

// Representation of a compound shader
struct IShader : public IVertexShader, public IFragmentShader 
{
	mat4 ModelProjection;
	vec3 light_dir;		// Need a more complex model of light

	TriangleMesh* model;

	IShader()
		:model(nullptr)
	{}



	virtual ~IShader() {};
	
	bool init(TriangleMesh* aModel, const vec3& light, const MVP3D& mvp)
	{
		model = aModel;
		light_dir = light;

		onViewChange(mvp);

		return true;
	}

	virtual void onViewChange(const MVP3D& mvp)
	{
		ModelProjection = mvp.fProjection * mvp.fModelView;
	}

	virtual vec4 vertex(int iface, int nthvert) = 0;
	virtual bool fragment(vec3 bar, BLRgba32 &color) = 0;
};

