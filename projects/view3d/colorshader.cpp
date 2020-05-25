
#include "colorshader.hpp"
#include "maths.hpp"


// Vertext Shader
// figues out the vertex of the given face and vertex
Vec4f ColorShader::vertex(int iface, int nthvert)
{
	varying_uv.set_col(nthvert, model->uv(iface, nthvert));
	varying_nrm.set_col(nthvert, proj<3>(ModelProjection.invert_transpose() * embed<4>(model->normal(iface, nthvert), 0.f)));
	Vec4f gl_Vertex = ModelProjection * embed<4>(model->vert(iface, nthvert));
	varying_tri.set_col(nthvert, gl_Vertex);
	ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
	
	return gl_Vertex;
}


bool ColorShader::fragment(Vec3f bar, BLRgba32& ocolor)
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


	// figure out the color for the location
	float diff = MAX(0.f, n * light_dir);
	//float diff = MAX(0.f, bn * light_dir);
	ocolor = fColor;
	ocolor = colormul(ocolor, diff);

	return false;
}

