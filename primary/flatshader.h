#pragma once

#include "IShader.h"
#include "TriangleMesh.h"
#include "MVP3D.h"

struct FlatShader : public virtual IShader {
	BLRgba32 fColor;

	FlatShader(const BLRgba32& c)
		:IShader(),
		fColor(c)
	{}

	// Vertext Shader
	virtual Vec4f vertex(int iface, int nthvert)
	{
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

		// figure out the color for the location
		float diff = MAX(0.f, bn * light_dir);
		ocolor = fColor;
		ocolor = colormul(ocolor, diff);

		return false;
	}
};

