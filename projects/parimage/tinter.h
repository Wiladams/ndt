#pragma once

#include "texture.h"
#include "grmath.h"

class Tinter : public Texture
{
	shared_ptr<Texture> fSource;
	vec3 fTint;

public:
	Tinter(const vec3& tint, shared_ptr<Texture> src)
		:fSource(src),
		fTint(tint)
	{
	}

	virtual rtcolor value(double u, double v, const vec3& p) const
	{
		// get pixel from source
		auto srcC = fSource->value(u, v, p);

		// add tint
		return vec3(Clamp((srcC.r + fTint.r), .0f, 1.0f), Clamp((srcC.g + fTint.g), .0f, 1.0f), Clamp((srcC.b + fTint.b), .0f, 1.0f));
	}
};
