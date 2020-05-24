#pragma once

#include <vector>

#include "geometry.h"
#include "imagesampler.h"

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

	ndt::ImageSampler fDiffuseSampler=nullptr;
	ndt::ImageSampler fNormalSampler = nullptr;
	ndt::ImageSampler fSpecularSampler = nullptr;

	bool setDiffuseMap(BLImage *img)
	{
		return fDiffuseSampler.init(img, true);
	}

	bool setNormalMap(BLImage* img)
	{
		return fNormalSampler.init(img, true);
	}

	bool setSpecularMap(BLImage* img)
	{
		return fSpecularSampler.init(img, true);
	}

	void addVertex(const Vec3f &vert)
	{
		fVertices.push_back(vert);
	}

	void addNormal(const Vec3f& n)
	{
		Vec3f aNormal(n);
		fNormals.push_back(aNormal.normalize());
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

		return fFaces.size();
	}

	size_t nverts() { return fVertices.size(); }
	size_t nfaces() { return fFaces.size(); }
	size_t nnormals() { return fNormals.size(); }
	size_t nuvs() { return fUV.size(); }

	// Getting values out of the mesh

	
	Vec3f vert(int i) 
	{
		return fVertices[i];
	}

	Vec3f vert(int iface, int nthvert) 
	{
		return fVertices[fFaces[iface].fVertices[nthvert]];
	}

	Vec2f uv(int iface, int nthvert) 
	{
		if (nuvs() == 0)
			return {};

		return fUV[fFaces[iface].fUV[nthvert]];
	}

	Vec3f normal(int iface, int nthvert)
	{
		if (nnormals() == 0)
			return {};

		int idx = fFaces[iface].fNormals[nthvert];
		return fNormals[idx];
	}


	// Using Samplers
	BLRgba32 diffuse(Vec2f uvf) {
		if (!fDiffuseSampler.isValid())
			return BLRgba32();

		auto c = fDiffuseSampler(uvf[0], uvf[1]);
		return c;
	}

	Vec3f normal(Vec2f uvf) {
		if (!fNormalSampler.isValid())
			return Vec3f();

		auto c = fNormalSampler(uvf[0], uvf[1]);

		Vec3f res;
		res[2] = (float)c.b / 255.f * 2.f - 1.f;
		res[1] = (float)c.g / 255.f * 2.f - 1.f;
		res[0] = (float)c.r / 255.f * 2.f - 1.f;

		return res;
	}


	float specular(Vec2f uvf) {
		if (!fSpecularSampler.isValid())
			return 0.0;

		auto c = fSpecularSampler(uvf[0], uvf[1]);
		return c.b / 1.0f;
	}

};