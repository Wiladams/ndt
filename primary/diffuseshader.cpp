
#include "diffuseshader.h"
#include "maths.hpp"


//
// Vertex shader
// When calculating a vertex, we want to determine the 
// uv component for texture mapping, 
// the normal for the affect of lighting
// the actual 'vertex', which might be displaced
// we return the gl_Vertex, in case anyone wants to consume
// that directly.
vec4f DiffuseShader::vertex(int iface, int nthvert)
{
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	varying_nrm.set_col(nthvert, proj<3>(ModelProjection.invert_transpose()*embed<4>(model->normal(iface, nthvert), 0.f)));
	vec4f gl_Vertex = ModelProjection*embed<4>(model->vert(iface, nthvert));
	varying_tri.set_col(nthvert, gl_Vertex);
	ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
	
	return gl_Vertex;
}



bool DiffuseShader::fragment(vec3f bar, BLRgba32 &ocolor)
{
	vec3f bn = (varying_nrm*bar).normalize();
	vec2f uv = varying_uv*bar;

	mat<3, 3, float> A;
	A[0] = ndc_tri.col(1) - ndc_tri.col(0);
	A[1] = ndc_tri.col(2) - ndc_tri.col(0);
	A[2] = bn;

	mat<3, 3, float> AI = A.invert();

	vec3f i = AI * vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
	vec3f j = AI * vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

	mat<3, 3, float> B;
	B.set_col(0, i.normalize());
	B.set_col(1, j.normalize());
	B.set_col(2, bn);

	// get the normal for the location
	vec3f n = (B*model->normal(uv)).normalize();

	// figure out the color for the location
	float diff = MAX(0.f, n*light_dir);
	ocolor = model->diffuse(uv);
	ocolor = colormul(ocolor,diff);

	return false;
}
