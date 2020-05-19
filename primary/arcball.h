#pragma once

/*
Reference
https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/obj-viewer/obj-viewer.cpp
*/

#include "geometry.h"
#include "apphost.h"

struct ArcBall
{
	int fWidth;
	int fHeight;

	int last_mx = 0;
	int last_my = 0;
	int cur_mx = 0;
	int cur_my = 0;
	int arcball_on = false;

	ArcBall(int w, int h);

	void mouseDown(const MouseEvent &e);
	void mouseMove(const MouseEvent& e);
	void mouseRelease(const MouseEvent& e);


	Vec3f get_arcball_vector(int x, int y);

};
