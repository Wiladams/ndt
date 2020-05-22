#pragma once

#include <vector>

#include "geometry.h"

// Each face consists of
// a vector of vec3i
// vector[0] - index of vertex
// vector[1] - index of uv
// vector[2] - index of normal

struct TriangleFace
{
	// per vertex
	// we have 
	//	vertex index
	//	vertex normal index
	//	vertex UV index
	//
	Vec3i fVertices;
	Vec3i fNormals;
	Vec3i fUV;

	Vec3f fFaceNormal;

	TriangleFace(const Vec3i &vertices, const Vec3i &normals, const Vec3i &uv)
		:fVertices(vertices),
		fNormals(normals),
		fUV(uv)
	{
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

	std::vector<TriangleFace> fFaces;

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
	size_t addFace(const Vec3i &verts, const Vec3i &norms, const Vec3i &uv)
	{
		TriangleFace face(verts, norms, uv);
		fFaces.push_back(face);
	}

	size_t nverts() { return fVertices.size(); }
	size_t nfaces() { return fFaces.size(); }
	size_t nnormals() { return fNormals.size(); }
	size_t nuvs() { return fUV.size(); }
};