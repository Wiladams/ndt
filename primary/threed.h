#pragma once

#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "p5.hpp"
#include "trianglemesh.h"

#include "Surface.h"
#include "diffuseshader.h"

#include "IShader.h"
#include "MVP3D.h"

using namespace p5;


#ifdef max
#undef max
#undef min
#endif

class ThreeD {
	IGraphics *fGC;
	int fWidth;
	int fHeight;

	MVP3D fMVP;

	vec3f fLightDirection{ 1, 1, 1 };
	vec3f fEye{ 1, 1, 3 };
	vec3f fCenter{ 0, 0, 0 };
	vec3f fUp{ 0, -1, 0 };

	float* zbuffer = nullptr;

	// OpenGL-like functions to do various
	// calculations
	static mat4f ogl_lookat(vec3f& eye, vec3f& center, vec3f& up)
	{
		vec3f z = (eye - center).normalize();
		vec3f x = cross(up, z).normalize();
		vec3f y = cross(z, x).normalize();
		mat4f Minv = mat4f::identity();
		mat4f Tr = mat4f::identity();
		for (int i = 0; i < 3; i++)
		{
			Minv[0][i] = x[i];
			Minv[1][i] = y[i];
			Minv[2][i] = z[i];
			Tr[i][3] = -center[i];
		}

		mat4f m = Minv * Tr;

		return m;
	}

	static mat4f ogl_projection(float coeff = 0.f)
	{
		mat4f res = mat4f::identity();
		res[3][2] = coeff;

		return res;
	}

	static mat4f ogl_viewport(const int x, const int y, const int w, const int h)
	{
		mat4f res = mat4f::identity();

		res[0][3] = x + w / 2.f;
		res[1][3] = y + h / 2.f;
		res[2][3] = 1.f;
		res[0][0] = w / 2.f;
		res[1][1] = h / 2.f;
		res[2][2] = 0;

		return res;
	}


	static vec3f barycentric(const vec2f &A, const vec2f &B, const vec2f &C, const vec2f &P)
	{
		vec3f s[2];
		for (int i = 2; i--; )
		{
			s[i][0] = C[i] - A[i];
			s[i][1] = B[i] - A[i];
			s[i][2] = A[i] - P[i];
		}
		vec3f u = cross(s[0], s[1]);
		if (std::abs(u[2]) > 1e-2) {
			// dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
			return vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
		}

		return vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizer
	}




public:
	ThreeD(std::shared_ptr<IGraphics> ctx, int w, int h)
		:fGC(ctx),
		fWidth(w),
		fHeight(h)
	{
		zbuffer = new float[(size_t)fWidth * fHeight];

		fMVP.fViewport = ogl_viewport(fWidth / 8, fHeight / 8, fWidth * 3 / 4, fHeight * 3 / 4);
		onCameraChange();
		fLightDirection = proj<3>((fMVP.fProjection* fMVP.fModelView*embed<4>(fLightDirection, 0.f))).normalize();
	}

	vec3f &getLightDirection() { return fLightDirection; }
	MVP3D &getMVP() { return fMVP; }
	vec3f& getCameraLocation() { return fEye; }


	void lookAt(float x, float y, float z)
	{
		fCenter = { x,y,z };
	}

	void moveCameraBy(float dx, float dy, float dz)
	{
		fEye += {dx, dy, dz};
		onCameraChange();
	}

	void moveCameraTo(float x, float y, float z)
	{
		fEye = { x,y,z };
		onCameraChange();
	}
	/*
	void setCameraLocation(float x, float y, float z)
	{
		fEye = { x, y, z };
		onCameraChange();
	}
	*/
	void onCameraChange()
	{
		fMVP.fModelView = ogl_lookat(fEye, fCenter, fUp);
		fMVP.fProjection = ogl_projection(-1.0f / (fEye - fCenter).length());
	}

	void clearZBuffer()
	{
		// Zero out the zbuffer in case the scene has changed
		for (int i = fWidth * fHeight; i--; )
		{
			zbuffer[i] = -std::numeric_limits<float>::max();
		}
	}

	// The ultimate routine which draws a triangle, using a shader
	void renderTriangle(mat<4, 3, float>& clipc, IShader& shader)
	{
		mat<3, 4, float> pts = (fMVP.fViewport * clipc).transpose(); // transposed to ease access to each of the points
		mat<3, 2, float> pts2;
		
		for (int i = 0; i < 3; i++)
			pts2[i] = proj<2>(pts[i] / pts[i][3]);

		vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		vec2f clmp((float)fWidth - 1, (float)fHeight - 1);

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
				bboxmax[j] = std::min(clmp[j], std::max(bboxmax[j], pts2[i][j]));
			}
		}

		vec2i P;
		BLRgba32 fragcolor;
		for (P.x = (int)bboxmin.x; P.x <= bboxmax.x; P.x++) {
			for (P.y = (int)bboxmin.y; P.y <= bboxmax.y; P.y++) {
				vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
				vec3f bc_clip = vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
				bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
				float frag_depth = clipc[2] * bc_clip;

				if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || zbuffer[P.x + (P.y * fWidth)]>frag_depth)
					continue;

				bool discard = shader.fragment(bc_clip, fragcolor);

				if (!discard) {
					zbuffer[P.x + (P.y * fWidth)] = frag_depth;

					fGC->set(P.x, P.y, fragcolor);
				}
			}
		}
	}

	bool renderMesh(TriangleMesh *aModel, IShader &shader)
	{
		if (nullptr == aModel)
			return false;

		shader.init(aModel, getLightDirection(), getMVP());

		// Assume each face consists of 3 vertices (triangles)
		// This can generalize to more vertices, but limiting
		// to three makes things much easier
		for (int i = 0; i < aModel->nfaces(); i++) {
			for (int j = 0; j < 3; j++) {
				// use the vertex shader to calculate positions
				// this is great for doing vertex shader manipulations
				auto vtx = shader.vertex(i, j);
			}

			// use the fragment shader to render the pixels
			// of the triangle
			renderTriangle(shader.varying_tri, shader);
		}

		return true;
	}
};
