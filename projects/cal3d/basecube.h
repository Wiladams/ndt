#pragma once

#include "trianglemesh.h"

// A base cube, where you can NOT work with each
// face independently as there are shared vertices
// This is only good for showing some basic rendering
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/

class BaseCube : public TriangleMesh
{
public:
	// width, height, depth
	BaseCube(size_t w, size_t h, size_t d)
	{
		addVertex(-w, -h, -d);	// 0
		addVertex(w, -h, -d);	// 1
		addVertex(w, h, -d);	// 2
		addVertex(-w, h, -d);	// 3
		addVertex(-w, -h, d);	// 4
		addVertex(w, -h, d);	// 5
		addVertex(w, h, d);		// 6
		addVertex(-w, h, d);	// 7

		// Counter clockwise winding
		addFace(0, 2, 1);
		addFace(0, 3, 2);

		addFace(1, 2, 6);
		addFace(6, 5, 1);

		addFace(4, 5, 6);
		addFace(6, 7, 4);

		addFace(2, 3, 6);
		addFace(6, 3, 7);

		addFace(0, 7, 3);
		addFace(0, 4, 7);

		addFace(0, 1, 5);
		addFace(0, 5, 4);
	}
};