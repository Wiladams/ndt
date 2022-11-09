#pragma once

#include "texture.h"

// Use a texture as a mask for a source
class Masker : public Texture
{
	shared_ptr<Texture> fSource;
	shared_ptr<Texture> fMask;

public:
	Masker(shared_ptr<Texture> mask, shared_ptr<Texture> src)
		:fMask(mask),
		fSource(src)
	{}

	virtual rtcolor value(double u, double v, const vec3f& p) const
	{
		// get pixel from source
		auto srcC = fSource->value(u, v, p);
		auto maskC = fMask->value(u, v, p);

		// either do a blend, or a basic threshold
		if (maskC.r == 0 && maskC.g == 0 && maskC.b == 0)
			return maskC;

		return srcC;
	}
};
