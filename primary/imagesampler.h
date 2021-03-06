#pragma once
#include "blend2d.h"
#include "sampler.hpp"
#include "maths.hpp"

namespace ndt 
{
	// This assumes the BLImage is in the PRGBA32 format
	class ImageSampler : public ISampler2D<BLRgba32>
	{
		BLImage * fImage;
		BLImageData fImageData;
		bool fFlipVertical;

	public:
		ImageSampler()
			:fImage(nullptr),
			fFlipVertical(false)
		{}

		ImageSampler(BLImage *img, const bool flipVertical = false)
			: fImage(nullptr),
			fFlipVertical(false)
		{
			init(img, flipVertical);
		}

		bool init(BLImage* img, const bool flipVertical = false)
		{
			if (nullptr == img)
				return false;

			fImage = img;
			fFlipVertical = flipVertical;
			fImage->getData(&fImageData);
		
			return true;
		}

		bool isValid() const { return fImage != nullptr; }

		BLRgba32 operator()(double u, double v)  const
		{
			if (nullptr == fImage)
				return {};

			int x = int(maths::Map(maths::Clamp(u,0.0,1.0), 0, 1, 0, fImage->width() - 1)+0.5);
			int y;
			if (fFlipVertical)
				y = int(maths::Map(maths::Clamp(v,0.0,1.0), 0, 1, fImage->height() - 1, 0)+0.5);	// flip vertically
			else
				y = int(maths::Map(maths::Clamp(v,0.0,1.0), 0, 1, 0, fImage->height() - 1)+0.5);
			
			int offset = y * fImage->width() + x;
			
			return ((BLRgba32*)fImageData.pixelData)[offset];
		}
	};
}	// end of ndt namespace