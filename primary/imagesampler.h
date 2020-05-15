#pragma once
#include "blend2d.h"
#include "sampler.hpp"
#include "maths.hpp"

namespace ndt 
{
	// This assumes the BLImage is in the PRGBA32 format
	class ImageSampler : public virtual ISampler2D<BLRgba32>
	{
		const BLImage& fImage;
		BLImageData fImageData;

	public:
		ImageSampler(const BLImage &img)
			:fImage(img)
		{
			fImage.getData(&fImageData);
		}

		BLRgba32 operator()(double u, double v)  const
		{
			int x = map(u, 0, 1, 0, fImage.width() - 1, true);
			int y = map(v, 0, 1, 0, fImage.height() - 1, true);

			int offset = y * fImage.width() + x;
			return ((BLRgba32*)fImageData.pixelData)[offset];
		}
	};
}	// end of ndt namespace