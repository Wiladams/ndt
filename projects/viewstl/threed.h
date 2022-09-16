#pragma once

#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <algorithm>


#include "p5.hpp"
#include "trianglemesh.h"
#include "tinygl/GL/gl.h"

//#include "IShader.h"
#include "MVP3D.h"


using namespace p5;


#ifdef max
#undef max
#undef min
#endif

class ThreeD {
	int fWidth;
	int fHeight;

	MVP3D fMVP;

	vec3 fLightDirection{ 1, 1, 1 };
	vec3 fEye{ 1, 1, 3 };
	vec3 fCenter{ 0, 0, 0 };
	vec3 fUp{ 0, -1, 0 };

	float* zbuffer = nullptr;

	// OpenGL-like functions to do various
	// calculations
	static mat4 ogl_lookat(vec3& eye, vec3& center, vec3& up)
	{
		vec3 z = normalizev3(subv3(eye, center));

		vec3 x = normalizev3(crossv3(up, z));
		vec3 y = normalizev3(crossv3(z, x));
		mat4 Minv = identitymat4();
		mat4 Tr = identitymat4();
		for (int i = 0; i < 3; i++)
		{
			Minv[0][i] = x[i];
			Minv[1][i] = y[i];
			Minv[2][i] = z[i];
			Tr[i][3] = -center[i];
		}

		mat4 m = Minv * Tr;

		return m;
	}

	static mat4 ogl_projection(float coeff = 0.0f)
	{
		mat4 res = identitymat4();
		res[3][2] = coeff;

		return res;
	}

	static mat4 ogl_viewport(const int x, const int y, const int w, const int h)
	{
		mat4 res = identitymat4();

		res[0][3] = x + w / 2.0f;
		res[1][3] = y + h / 2.0f;
		res[2][3] = 1.0f;
		res[0][0] = w / 2.0f;
		res[1][1] = h / 2.0f;
		res[2][2] = 0;

		return res;
	}

/*
	static vec3 barycentric(const vec2 &A, const vec2 &B, const vec2 &C, const vec2 &P)
	{
		vec3 s[2]{};
		for (int i = 2; i--; )
		{
			s[i].d[0] = C.d[i] - A.d[i];
			s[i].d[1] = B.d[i] - A.d[i];
			s[i].d[2] = A.d[i] - P.d[i];
		}
		vec3 u = crossv3(s[0], s[1]);

		if (std::abs(u.d[2]) > 1e-2) 
		{
			// dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
			return vec3{ 1.0f - (u.d[0] + u.d[1]) / u.d[2], u.d[1] / u.d[2], u.d[0] / u.d[2] };
		}

		return vec3{ -1, 1, 1 }; // in this case generate negative coordinates, it will be thrown away by the rasterizer
	}
	*/



public:
	ThreeD(int w, int h)
		: fWidth(w)
		,fHeight(h)
	{
		zbuffer = new float[(size_t)fWidth * fHeight];

		fMVP.fViewport = ogl_viewport(fWidth / 8, fHeight / 8, fWidth * 3 / 4, fHeight * 3 / 4);
		onCameraChange();
		fLightDirection = proj<3>((fMVP.fProjection* fMVP.fModelView*embed<4>(fLightDirection, 0.f))).normalize();
	}

	vec3 &getLightDirection() { return fLightDirection; }
	MVP3D &getMVP() { return fMVP; }
	vec3& getCameraLocation() { return fEye; }


	void lookAt(float x, float y, float z)
	{
		fCenter = { x,y,z };
	}

	void moveCameraBy(float dx, float dy, float dz)
	{
		fEye = addv3(fEye, { dx, dy, dz });

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
		mat4 lookAt(fEye, fCenter, fUp);
		fMVP.fModelView = ogl_lookat(fEye, fCenter, fUp);
		fMVP.fProjection = ogl_projection(-1.0f / (fEye - fCenter).length());
	}

	void clearZBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}

	//bool renderMesh(TriangleMesh *aModel, IShader &shader)
	bool renderMesh(TriangleMesh* aModel)
	{
		if (nullptr == aModel)
			return false;

		// Assume each face consists of 3 vertices (triangles)
		// This can generalize to more vertices, but limiting
		// to three makes things much easier
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < aModel->nfaces(); i++)
		{
			TriangleFace face = aModel->fFaces[i];
			glColor4f(1.0f, 0.01f, 0.01f, 0.0f);
			vec3 v1 = aModel->fVertices[face.fVertices.d[0]];
			vec3 v2 = aModel->fVertices[face.fVertices.d[1]];
			vec3 v3 = aModel->fVertices[face.fVertices.d[2]];

			glVertex3f(v1.d[0], v1.d[1], v1.d[2]);
			glVertex3f(v2.d[0], v2.d[1], v2.d[2]);
			glVertex3f(v3.d[0], v3.d[1], v3.d[2]);
		}

		glEnd();


		return true;
	}
};
