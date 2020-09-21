#pragma once

/*
	References:
	http://www.martinreddy.net/gfx/3d/OBJ.spec
	https://github.com/bitnenfer/objpar
*/
#include "trianglemesh.h"


namespace aliaswave {
	TriangleMesh * loadModel(const char* filename);
};