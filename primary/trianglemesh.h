#pragma once

#include <vector>

#include "geometry.h"


struct TriangleFace
{
	// per vertex
	// we have 
	//	vertex index
	//	vertex normal index
	//	vertex UV index
	//
	std::vector<Vec3i> fVertices;
	std::vector<Vec3i> fNormals;
	std::vector<Vec3i> fUV;

	Vec3f fFaceNormal;

	TriangleFace(const Vec3i &vertices, const Vec3i &normals, const Vec3i &uv)
	{
		fVertices.push_back(vertices);
		fNormals.push_back(normals);
		fUV.push_back(uv);

		//Vec3f v1 = p1 - p0;
		//Vec3f v2 = p2 - p0;

		//fFaceNormal = cross(v1, v2);
	}
};


struct TriangleMesh
{
	std::vector<Vec3f> fVertices;
	std::vector<Vec3f> fNormals;
	std::vector<Vec2f> fUV;

	// Each face consists of
	// a vector of vec3i
	// vector[0] - index of vertex
	// vector[1] - index of uv
	// vector[2] - index of normal
	std::vector<std::vector<Vec3i> > fFaces;

	void addVertex(const Vec3f &vert)
	{
		fVertices.push_back(vert);
	}

	void addNormal(const Vec3f& n)
	{
		fNormals.push_back(n);
	}

	void addUV(const Vec2f& uv)
	{
		fUV.push_back(uv);
	}

	// A face is indicated by 3 indices of vertices
	// that are already in the mesh
	size_t addFace(const size_t idx0, const size_t idx1, const size_t idx2)
	{

	}
};