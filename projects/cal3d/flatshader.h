
#include <algorithm>
#include "maths.hpp"

#include "IShader.h"
#include "GMesh.h"

struct SimpleVertexShader : public virtual IVertexShader
{

};

struct FlatShader : public virtual IShader 
{
	mat<2, 3, float> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
	mat<4, 3, float> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
	mat<3, 3, float> varying_nrm; // normal per vertex to be interpolated by FS
	mat<3, 3, float> ndc_tri;     // triangle in normalized device coordinates


	GMesh* model;
	Vec3f light_dir;
	Matrix ModelView;
	Matrix Projection;
	Matrix ModelProjection;

	Matrix Viewport;
	
	// Helper to multiply a color value
	inline static BLRgba32 colormul(const BLRgba32& c, const float fac)
	{
		double scalar = constrain(fac, 0, 1);

		uint32_t r = constrain((double)c.r * scalar, 0, 255);
		uint32_t g = constrain((double)c.g * scalar, 0, 255);
		uint32_t b = constrain((double)c.b * scalar, 0, 255);

		return { r,g,b };
	}

	FlatShader(GMesh* aModel, Vec3f& light, Matrix& modelview, Matrix& projection, Matrix& viewport)
		: model(aModel),
		light_dir(light)
	{
		onViewChange(modelview, projection, viewport);
	}

	void onViewChange(Matrix& modelview, Matrix& projection, Matrix& viewport)
	{
		ModelView = modelview;
		Projection = projection;
		Viewport = viewport;

		ModelProjection = Projection * ModelView;
	}

	// Vertext Shader
	// figues out the vertex of the given face and vertex
	virtual Vec4f vertex(int iface, int nthvert)
	{
		//printf("FlatShader::vertex: %d %d\n", iface, nthvert);

		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		varying_nrm.set_col(nthvert, proj<3>(ModelProjection.invert_transpose() * embed<4>(model->normal(iface, nthvert), 0.f)));
		Vec4f gl_Vertex = ModelProjection * embed<4>(model->vert(iface, nthvert));
		varying_tri.set_col(nthvert, gl_Vertex);
		ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));

		return gl_Vertex;
	}

	// fragment shader
	virtual bool fragment(Vec3f bar, BLRgba32& ocolor)
	{
		Vec3f bn = (varying_nrm * bar).normalize();
		Vec2f uv = varying_uv * bar;

		mat<3, 3, float> A;
		A[0] = ndc_tri.col(1) - ndc_tri.col(0);
		A[1] = ndc_tri.col(2) - ndc_tri.col(0);
		A[2] = bn;

		mat<3, 3, float> AI = A.invert();

		Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
		Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

		mat<3, 3, float> B;
		B.set_col(0, i.normalize());
		B.set_col(1, j.normalize());
		B.set_col(2, bn);

		Vec3f n = (B * model->normal(uv)).normalize();

		// BUGBUG, trying to figure out color
		//printf("FlatShader::fragment.uv: %3.2f %3.2f\n", uv[0], uv[1]);

		float diff = MAX(0.f, n * light_dir);
		ocolor = model->diffuse(uv);
		ocolor = colormul(ocolor, diff);

		return false;
	}
};


#pragma once
