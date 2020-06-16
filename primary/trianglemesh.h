#pragma once

#include <vector>
#include <algorithm>
#include <limits>


#include "grmath.h"
#include "imagesampler.h"

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
	vec3i fVertices;
	vec3i fNormals;
	vec3i fUV;

	vec3f fFaceNormal;



	TriangleFace(const size_t a, const size_t b, const size_t c)
		:fVertices(a,b,c)
	{

	}

	TriangleFace(const vec3i &vertices, const vec3i &normals, const vec3i &uv)
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
	std::vector<vec3f> fVertices;
	std::vector<vec3f> fNormals;
	std::vector<vec2f> fUV;
	std::vector<TriangleFace> fFaces;

	ndt::ImageSampler fDiffuseSampler=nullptr;
	ndt::ImageSampler fNormalSampler = nullptr;
	ndt::ImageSampler fSpecularSampler = nullptr;

	vec3f fVertSmallest;
	vec3f fVertLargest;

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
		fVertSmallest.x = std::numeric_limits<float>::max();
		fVertSmallest.y = std::numeric_limits<float>::max();
		fVertSmallest.z = std::numeric_limits<float>::max();

		fVertLargest.x = std::numeric_limits<float>::min();
		fVertLargest.y = std::numeric_limits<float>::min();
		fVertLargest.z = std::numeric_limits<float>::min();
	}

	void extendRange(const vec3f& vert)
	{
		fVertSmallest.x = MIN(vert.x, fVertSmallest.x);
		fVertSmallest.y = MIN(vert.y, fVertSmallest.y);
		fVertSmallest.z = MIN(vert.z, fVertSmallest.z);

		fVertLargest.x = MAX(vert.x, fVertLargest.x);
		fVertLargest.y = MAX(vert.y, fVertLargest.y);
		fVertLargest.z = MAX(vert.z, fVertLargest.z);
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

		vec3f diff = fVertLargest - fVertSmallest;
		float extent = diff.length();

		//printf("DIFF: %f, %f, %f\n", diff.x, diff.y, diff.z);
		//printf("EXTENT: %f\n", extent);

		for (size_t n = 0; n < nverts(); n++) {
			fVertices[n] = fVertices[n] / extent;
		}
	}

	void centerMesh()
	{
		//printf("==== centerMesh ====\n");
		calcExtent();
		vec3f center;
		center.x = lerp(fVertSmallest.x, fVertLargest.x, 0.5f);
		center.y = lerp(fVertSmallest.y, fVertLargest.y, 0.5f);
		center.z = lerp(fVertSmallest.z, fVertLargest.z, 0.5f);

		//printf("CENTER: %f,%f,%f\n", center.x, center.y, center.z);


		for (size_t n = 0; n < nverts(); n++) {
			fVertices[n] = fVertices[n] - center;
		}
	}



	size_t addVertex(const vec3f &vert)
	{
		extendRange(vert);
		fVertices.push_back(vert);
		return fVertices.size()-1;
	}

	size_t addVertex(const float x, const float y, const float z)
	{
		return addVertex(vec3f(x, y, z));
	}

	// Normals
	size_t addNormal(const vec3f& n)
	{
		vec3f aNormal(n);
		fNormals.push_back(aNormal.normalize());
		return fNormals.size()-1;
	}

	// Texture coordinate
	size_t addUV(const vec2f& uv)
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
	size_t addFace(const vec3i &verts, const vec3i &norms, const vec3i &uv)
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

	
	vec3f vert(int i) 
	{
		return fVertices[i];
	}

	vec3f vert(int iface, int nthvert) 
	{
		return fVertices[fFaces[iface].fVertices[nthvert]];
	}

	vec2f uv(int iface, int nthvert) 
	{
		if (nuvs() == 0)
			return {};

		return fUV[fFaces[iface].fUV[nthvert]];
	}

	vec3f normal(int iface, int nthvert)
	{
		if (nnormals() == 0)
			return {};

		int idx = fFaces[iface].fNormals[nthvert];
		return fNormals[idx];
	}


	// Using Samplers
	BLRgba32 diffuse(vec2f uvf) {
		if (!fDiffuseSampler.isValid())
			return BLRgba32();

		auto c = fDiffuseSampler(uvf[0], uvf[1]);
		return c;
	}

	vec3f normal(vec2f uvf) {
		if (!fNormalSampler.isValid())
			return vec3f();

		auto c = fNormalSampler(uvf[0], uvf[1]);

		vec3f res;
		res[2] = (float)c.b / 255.f * 2.f - 1.f;
		res[1] = (float)c.g / 255.f * 2.f - 1.f;
		res[0] = (float)c.r / 255.f * 2.f - 1.f;

		return res;
	}


	float specular(vec2f uvf) {
		if (!fSpecularSampler.isValid())
			return 0.0;

		auto c = fSpecularSampler(uvf[0], uvf[1]);
		return c.b / 1.0f;
	}

};