#pragma once

#include <vector>
#include <algorithm>
#include <limits>


#include "tinygl/3dMath.h"
//#include "imagesampler.h"

// Each face consists of
// a vector of vec3i
// vector[0] - index of vertex
// vector[1] - index of uv
// vector[2] - index of normal

#ifdef max
#undef max
#undef min
#endif

struct TriangleFace
{
	// per vertex
	// we have 
	//	vertex index
	//	vertex normal index
	//	vertex UV index
	//
	ivec3 fVertices;
	ivec3 fNormals;
	ivec3 fUV;

	vec3 fFaceNormal;



	TriangleFace(const int a, const int b, const int c)
	{
		fVertices.d[0] = a;
		fVertices.d[1] = b;
		fVertices.d[2] = c;
	}

	TriangleFace(const ivec3 &vertices, const ivec3 &normals, const ivec3 &uv)
		:fVertices(vertices),
		fNormals(normals),
		fUV(uv)
	{
		//vec3f v1 = p1 - p0;
		//vec3f v2 = p2 - p0;

		//fFaceNormal = cross(v1, v2);
	}
};


struct TriangleMesh
{
	std::vector<vec3> fVertices;
	std::vector<vec3> fNormals;
	std::vector<vec2> fUV;
	std::vector<TriangleFace> fFaces;

	ndt::ImageSampler fDiffuseSampler=nullptr;
	ndt::ImageSampler fNormalSampler = nullptr;
	ndt::ImageSampler fSpecularSampler = nullptr;

	vec3 fVertSmallest;
	vec3 fVertLargest;

	TriangleMesh()
	{
		resetExtent();
	}

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

	void resetExtent()
	{
		fVertSmallest.d[0] = std::numeric_limits<float>::max();
		fVertSmallest.d[1] = std::numeric_limits<float>::max();
		fVertSmallest.d[2] = std::numeric_limits<float>::max();

		fVertLargest.d[0] = std::numeric_limits<float>::min();
		fVertLargest.d[1] = std::numeric_limits<float>::min();
		fVertLargest.d[2] = std::numeric_limits<float>::min();
	}

	void extendRange(const vec3& vert)
	{
		fVertSmallest.d[0] = maths::Min(vert.d[0], fVertSmallest.d[0]);
		fVertSmallest.d[1] = maths::Min(vert.d[1], fVertSmallest.d[1]);
		fVertSmallest.d[2] = maths::Min(vert.d[2], fVertSmallest.d[2]);

		fVertLargest.d[0] = maths::Max(vert.d[0], fVertLargest.d[0]);
		fVertLargest.d[1] = maths::Max(vert.d[1], fVertLargest.d[1]);
		fVertLargest.d[2] = maths::Max(vert.d[2], fVertLargest.d[2]);
	}

	void calcExtent()
	{
		//printf("==== calcExtent ====\n");
		resetExtent();
		for (size_t i = 0; i < fVertices.size(); i++)
			extendRange(fVertices[i]);

		//printf("SMALLEST: %f, %f, %f\n", fVertSmallest.x, fVertSmallest.y, fVertSmallest.z);
		//printf("LARGEST: %f, %f, %f\n", fVertLargest.x, fVertLargest.y, fVertLargest.z);
	}

	void normalizeVertices()
	{
		//printf("==== normalizeVertices ====\n");
		calcExtent();

		vec3 diff = subv3(fVertLargest, fVertSmallest);
		float extent = lengthv3(diff);

		//printf("DIFF: %f, %f, %f\n", diff.x, diff.y, diff.z);
		//printf("EXTENT: %f\n", extent);

		for (size_t n = 0; n < nverts(); n++) 
		{
			fVertices[n] = scalev3(1.0/extent, fVertices[n]);
		}
	}

	void centerMesh()
	{
		//printf("==== centerMesh ====\n");
		calcExtent();
		vec3 center{};
		center.d[0] = maths::Lerp(0.5f, fVertSmallest.d[0], fVertLargest.d[0]);
		center.d[1] = maths::Lerp(0.5f, fVertSmallest.d[1], fVertLargest.d[1]);
		center.d[2] = maths::Lerp(0.5f, fVertSmallest.d[2], fVertLargest.d[2]);

		//printf("CENTER: %f,%f,%f\n", center.x, center.y, center.z);


		for (size_t n = 0; n < nverts(); n++) {
			fVertices[n] = subv3(fVertices[n], center);
		}
	}



	size_t addVertex(const vec3 &vert)
	{
		extendRange(vert);
		fVertices.push_back(vert);
		return fVertices.size()-1;
	}

	size_t addVertex(const float x, const float y, const float z)
	{
		return addVertex(vec3{ .d[0] = x,.d[1] = y,.d[2] = z });
	}

	// Normals
	size_t addNormal(const vec3& n)
	{
		vec3 aNormal(n);
		//fNormals.push_back(aNormal.normalize());
		fNormals.push_back(normalizev3(aNormal));

		return fNormals.size()-1;
	}

	// Texture coordinate
	size_t addUV(const vec2& uv)
	{
		fUV.push_back(uv);
		return fUV.size()-1;
	}

	size_t addFace(const size_t a, const size_t b, const size_t c)
	{
		TriangleFace face(a, b, c);
		fFaces.push_back(face);

		return fFaces.size() - 1;
	}

	// A face is indicated by 3 indices of vertices
	// that are already in the mesh
	size_t addFace(const ivec3 &verts, const ivec3 &norms, const ivec3 &uv)
	{
		TriangleFace face(verts, norms, uv);
		fFaces.push_back(face);

		return fFaces.size()-1;
	}

	size_t nverts() { return fVertices.size(); }
	size_t nfaces() { return fFaces.size(); }
	size_t nnormals() { return fNormals.size(); }
	size_t nuvs() { return fUV.size(); }

	// Getting values out of the mesh

	
	vec3 vert(int i) 
	{
		return fVertices[i];
	}

	vec3 vert(int iface, int nthvert) 
	{
		return fVertices[fFaces[iface].fVertices.d[nthvert]];
	}

	vec2 uv(int iface, int nthvert) 
	{
		if (nuvs() == 0)
			return {};

		return fUV[fFaces[iface].fUV.d[nthvert]];
	}

	vec3 normal(int iface, int nthvert)
	{
		if (nnormals() == 0)
			return {};

		int idx = fFaces[iface].fNormals.d[nthvert];
		return fNormals[idx];
	}


	// Using Samplers
	BLRgba32 diffuse(vec2 uvf) {
		if (!fDiffuseSampler.isValid())
			return BLRgba32();

		auto c = fDiffuseSampler(uvf.d[0], uvf.d[1]);
		return c;
	}

	vec3 normal(vec2 uvf) {
		if (!fNormalSampler.isValid())
			return vec3();

		auto c = fNormalSampler(uvf.d[0], uvf.d[1]);

		vec3 res{};
		res.d[2] = (float)c.b() / 255.f * 2.f - 1.f;
		res.d[1] = (float)c.g() / 255.f * 2.f - 1.f;
		res.d[0] = (float)c.r() / 255.f * 2.f - 1.f;

		return res;
	}


	float specular(vec2 uvf) {
		if (!fSpecularSampler.isValid())
			return 0.0;

		auto c = fSpecularSampler(uvf.d[0], uvf.d[1]);
		return c.b() / 1.0f;
	}

};