#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "trianglemesh.h"

namespace aliaswave {
// Loader for the obj model format
static bool loadModel(const char* filename, TriangleMesh &mesh)
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
		return false;

	std::string sline;

	while (!in.eof()) {
		std::getline(in, sline);
		std::istringstream iss(sline.c_str());
		char trash;
		if (!sline.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			mesh.addVertex(v);
		} else if (!sline.compare(0, 3, "vn ")) 
		{
			iss >> trash >> trash;
			Vec3f n;
			for (int i = 0; i < 3; i++)
				iss >> n[i];
			mesh.addNormal(n);
		}
		else if (!sline.compare(0, 3, "vt ")) 
		{
			iss >> trash >> trash;
			Vec2f uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			mesh.addUV(uv);
		}
		else if (!sline.compare(0, 2, "f ")) 
		{
			// attention, this Vec3i means vertex/uv/normal
			std::vector<Vec3i> f;

			Vec3i tmp;
			iss >> trash;
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) 
			{
				for (int i = 0; i < 3; i++) 
					tmp[i]--; // in wavefront obj all indices start at 1, not zero

				f.push_back(tmp);
			}
			Vec3i vert(f[0][0], f[1][0],f[2][0]);
			Vec3i uv(f[0][1],f[1][1],f[2][1]);
			Vec3i norm(f[0][2],f[1][2],f[2][2]);

			mesh.addFace(vert, norm,uv);
		}
	}
	
	return true;
}


}	// namespace aliaswave
