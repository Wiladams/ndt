#pragma once
#include <vector>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "p5.hpp"
#include "GMesh.h"
#include "geometry.h"
#include "dproc_gl.h"
#include "diffuseshader.h"

using namespace p5;

class ThreeD {
	BLImage &fImage;
	int fWidth;
	int fHeight;

	Matrix ModelView;
	Matrix Projection;
	Matrix Viewport;

	Vec3f fLightDirection{ 1, 1, 1 };
	Vec3f fEye{ 1, 1, 3 };
	Vec3f fCenter{ 0, 0, 0 };
	Vec3f fUp{ 0, -1, 0 };

	float* zbuffer = nullptr;

public:
	ThreeD(BLImage & fb)
		:fImage(fb)
	{
		fWidth = fb.width();
		fHeight = fb.height();
		zbuffer = new float[(size_t)fWidth * fHeight];

		Viewport = ogl_viewport(fWidth / 8, fHeight / 8, fWidth * 3 / 4, fHeight * 3 / 4);
		onCameraChange();
		fLightDirection = proj<3>((Projection*ModelView*embed<4>(fLightDirection, 0.f))).normalize();
	}

	void lookAt(float x, float y, float z)
	{
		fCenter = { x,y,z };
	}

	void moveCameraBy(float dx, float dy, float dz)
	{
		fEye += {dx, dy, dz};
		onCameraChange();
	}

	void setCameraLocation(float x, float y, float z)
	{
		fEye = { x, y, z };
		onCameraChange();
	}

	void onCameraChange()
	{
		ModelView = ogl_lookat(fEye, fCenter, fUp);
		Projection = ogl_projection(-1.0f / (fEye - fCenter).norm());
	}

	void clearZBuffer()
	{
		ogl_clearzbuffer(zbuffer, width, height);
	}

	void renderMesh(GMesh* aModel)
	{
		DiffuseShader shader(aModel, fLightDirection, ModelView, Projection, Viewport);

		// Assume each face consists of 3 vertices (triangles)
		// This can generalize to more vertices, but limiting
		// to three makes things much easier
		for (int i = 0; i < aModel->nfaces(); i++) {
			for (int j = 0; j < 3; j++) {
				// use the vertex shader to calculate positions
				// this is great for doing vertex shader manipulations
				auto vtx = shader.vertex(i, j);
				//printf("vtx: %3.2f, %3.2f, %3.2f, %3.2f\n", vtx[0], vtx[1], vtx[2], vtx[3]);
			}

			// use the fragment shader to render the pixels
			// of the triangle
			ogl_triangle(Viewport, shader.varying_tri, shader, fImage, zbuffer);
		}
	}
};
