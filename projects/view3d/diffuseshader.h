#pragma once
#include "IShader.h"
#include "GMesh.h"

struct DiffuseShader : public IShader {
	mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
	mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
	mat<3, 3, float> varying_nrm; // normal per vertex to be interpolated by FS
	mat<3, 3, float> ndc_tri;     // triangle in normalized device coordinates
	
	
	GMesh *model;
	Vec3f light_dir;
	Matrix ModelView;
	Matrix Projection;
	Matrix ModelProjection;

	Matrix Viewport;

	DiffuseShader(GMesh *aModel, Vec3f &light, Matrix &modelview, Matrix &projection, Matrix &viewport)
	{
		model = aModel;
		light_dir = light;

		onViewChange(modelview, projection, viewport);
	}

	void onViewChange(Matrix &modelview, Matrix &projection, Matrix &viewport);

	// vertex shader
	virtual Vec4f vertex(int iface, int nthvert);

	// fragment shader
	virtual bool fragment(Vec3f bar, BLRgba32 &color);
};
