#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "geometry.h"
#include "codec_targa.hpp"
#include "imagesampler.h"


class GMesh
{
public:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
	std::vector<Vec3f> norms_;
	std::vector<Vec2f> uv_;

	BLImage *diffusemap_;
	ndt::ImageSampler fDiffuseSampler;

	BLImage *normalmap_;
	ndt::ImageSampler fNormalSampler;

	BLImage *specularmap_;
	ndt::ImageSampler fSpecularSampler;

	BLImage * load_texture(std::string filename, const char *suffix) {
		std::string texfile(filename);
		size_t dot = texfile.find_last_of(".");
		if (dot != std::string::npos) {
			texfile = texfile.substr(0, dot) + std::string(suffix);

			targa::TargaMeta meta;
			BLImage* img = targa::readFromFile(texfile.c_str(), meta);

			return img;
			//std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
			//img.flip_vertically();
		}
		return nullptr;
	}

	bool loadDiffuseMap(std::string filename, const char *suffix)
	{
		diffusemap_ = load_texture(filename, suffix);
		return fDiffuseSampler.init(diffusemap_, true);
	}
	
	bool loadNormalMap(std::string filename, const char* suffix)
	{
		normalmap_ = load_texture(filename, suffix);
		return fNormalSampler.init(normalmap_, true);
	}

	bool loadSpecularMap(std::string filename, const char* suffix)
	{
		specularmap_ = load_texture(filename, suffix);
		return fSpecularSampler.init(specularmap_, true);
	}

public:
	GMesh()
		: verts_(), 
		faces_(), 
		norms_(), 
		uv_(), 
		diffusemap_(nullptr), 
		normalmap_(nullptr), 
		specularmap_(nullptr)
	{}

	void addVert(Vec3f &v) {
		verts_.push_back(v);
	}

	void addFace(std::vector<Vec3i> &fv) {
		faces_.push_back(fv);
	}

	void addNormal(Vec3f &v) {
		norms_.push_back(v);
	}

	void addUV(Vec2f &uv) {
		uv_.push_back(uv);
	}

	size_t nverts() {return verts_.size();}
	size_t nfaces() {return faces_.size();}
	size_t nnormals() { return norms_.size(); }
	size_t nuvs() { return uv_.size(); }

	// Retrieve a face at the specified index
	std::vector<int> face(int idx) {
		std::vector<int> vface;
		for (int i = 0; i<(int)faces_[idx].size(); i++) 
			vface.push_back(faces_[idx][i][0]);

		return vface;
	}

	// Get the normal for a specific face and vertex
	Vec3f normal(int iface, int nthvert) 
	{
		if (norms_.size() == 0)
			return {};

		int idx = faces_[iface][nthvert][2];
		return norms_[idx].normalize();
	}

	Vec3f vert(int i) {
		return verts_[i];
	}

	Vec3f vert(int iface, int nthvert) {
		return verts_[faces_[iface][nthvert][0]];
	}

	Vec2f uv(int iface, int nthvert) {
		if (uv_.size() == 0)
			return {};

		return uv_[faces_[iface][nthvert][1]];
	}

	BLRgba32 diffuse(Vec2f uvf) {
		if (nullptr == diffusemap_)
			return BLRgba32();

		auto c = fDiffuseSampler(uvf[0], uvf[1]);
		return c;
	}

	Vec3f normal(Vec2f uvf) {
		if (nullptr == normalmap_)
			return Vec3f();

		auto c = fNormalSampler(uvf[0], uvf[1]);
		
		Vec3f res;
		res[2] = (float)c.b / 255.f * 2.f - 1.f;
		res[1] = (float)c.g / 255.f * 2.f - 1.f;
		res[0] = (float)c.r / 255.f * 2.f - 1.f;

		//Vec2i uv(uvf[0] * normalmap_->width(), uvf[1] * normalmap_->height());
		//BLRgba32 c = normalmap_->get(uv[0], uv[1]);
		//Vec3f res;
		//for (int i = 0; i<3; i++)
		//	res[2 - i] = (float)c[i] / 255.f*2.f - 1.f;
		
		return res;
	}


	float specular(Vec2f uvf) {
		if (nullptr == specularmap_)
			return 0.0;

		auto c = fSpecularSampler(uvf[0], uvf[1]);
		return c.b/1.0f;

		//Vec2i uv(uvf[0] * specularmap_->width(), uvf[1] * specularmap_->height());
		//return specularmap_->get(uv[0], uv[1])[0] / 1.f;
	}

};

