#pragma once

/*
Reference
https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/obj-viewer/obj-viewer.cpp
*/


#include "apphost.h"
#include "vec3.h"

struct ArcBall
{
	int fWidth;
	int fHeight;

	int last_mx = 0;
	int last_my = 0;
	int cur_mx = 0;
	int cur_my = 0;
	int fIsDragging = false;

private:
/**
* Get a normalized vector from the center of the virtual ball O to a
* point P on the virtual ball surface, such that P is aligned on
* screen's (X,Y) coordinates.  If (X,Y) is too far away from the
* sphere, return the nearest point on the virtual ball surface.
*/
	vec3 get_arcball_vector(int x, int y)
	{
		vec3 P = vec3(1.0 * x / fWidth * 2 - 1.0,
			1.0 * y / fHeight * 2 - 1.0,
			0);
		P.y = -P.y;
		float OP_squared = P.x * P.x + P.y * P.y;
		if (OP_squared <= 1 * 1)
			P.z = sqrt(1 * 1 - OP_squared);  // Pythagore
		else
			P.unit();

		return P;
	}

public:
	ArcBall(int w, int h)
		:fWidth(w),
		fHeight(h),
		fIsDragging(false)
	{}

	void mouseDown(const MouseEvent& e)
	{
		if (e.lbutton) {
			fIsDragging = true;
			last_mx = e.x;
			last_my = e.y;
			cur_mx = last_mx;
			cur_my = last_my;
		}
	}
	
	void mouseMove(const MouseEvent& e)
	{
		if (fIsDragging) {  // if left button is pressed
			cur_mx = e.x;
			cur_my = e.y;
		}
	}
	
	void mouseRelease(const MouseEvent& e)
	{
		fIsDragging = false;
	}


	vec3 get_arcball_vector(int x, int y);

	void update(double tm)
	{
		if (cur_mx != last_mx || cur_my != last_my) {
			vec3 va = get_arcball_vector(last_mx, last_my);
			vec3 vb = get_arcball_vector(cur_mx, cur_my);
			float angle = acos(MIN(1.0f, dot(va, vb)));
			vec3 axis_in_camera_coord = cross(va, vb);
			//glm::mat3 camera2object = glm::inverse(glm::mat3(transforms[MODE_CAMERA]) * glm::mat3(mesh.object2world));
			//vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
			//mesh.object2world = glm::rotate(mesh.object2world, glm::degrees(angle), axis_in_object_coord);
			last_mx = cur_mx;
			last_my = cur_my;
		}
	}
};

// onDisplay()
// Handle arcball 
/*
if (cur_mx != last_mx || cur_my != last_my) {
	glm::vec3 va = get_arcball_vector(last_mx, last_my);
	glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
	float angle = acos(min(1.0f, glm::dot(va, vb)));
	glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
	glm::mat3 camera2object = glm::inverse(glm::mat3(transforms[MODE_CAMERA]) * glm::mat3(main_object.object2world));
	glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
	main_object.object2world = glm::rotate(main_object.object2world, angle, axis_in_object_coord);
	last_mx = cur_mx;
	last_my = cur_my;
}
*/