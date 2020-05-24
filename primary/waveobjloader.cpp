#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "trianglemesh.h"
#include "codec_targa.hpp"
#include "waveobjloader.h"

namespace aliaswave {
	BLImage* load_texture(std::string filename, const char* suffix) {
		std::string texfile(filename);
		size_t dot = texfile.find_last_of(".");
		if (dot != std::string::npos) {
			texfile = texfile.substr(0, dot) + std::string(suffix);

			targa::TargaMeta meta;
			BLImage* img = targa::readFromFile(texfile.c_str(), meta);

			return img;
		}
		return nullptr;
	}


// Loader for the obj model format
TriangleMesh * loadModel(const char* filename)
{
	printf("loadModel: %s\n", filename);

	TriangleMesh * mesh = new TriangleMesh();

	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
		return nullptr;

	std::string sline;

	while (!in.eof()) {
		std::getline(in, sline);
		std::istringstream iss(sline.c_str());
		char trash;
		//printf("line: %s\n", sline.c_str());

		if (!sline.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			mesh->addVertex(v);
		} else if (!sline.compare(0, 3, "vn ")) 
		{
			iss >> trash >> trash;
			Vec3f n;
			for (int i = 0; i < 3; i++)
				iss >> n[i];
			mesh->addNormal(n);
		}
		else if (!sline.compare(0, 3, "vt ")) 
		{
			iss >> trash >> trash;
			Vec2f uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			mesh->addUV(uv);
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

			mesh->addFace(vert, norm,uv);
		}

	}
	
	// Load various maps if available
	mesh->setDiffuseMap(load_texture(filename, "_diffuse.tga"));
	mesh->setNormalMap(load_texture(filename, "_nm_tangent.tga"));
	mesh->setSpecularMap(load_texture(filename, "_spec.tga"));

	return mesh;
}


}	// namespace aliaswave
