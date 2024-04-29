#pragma once

#include "blend2d.h"

struct SpriteSheet
{
	BLImage fBaseImage{};
	BLImageData fImageData{};

	bool loadImage(const char* filename)
	{
		auto err = fBaseImage.readFromFile(filename);

		if (err)
		{
			printf("SpriteSheet::loadImage, could not load image: %s (%d)\n", filename, err);
			return false;
		}

		fBaseImage.getData(&fImageData);

		return true;

	}

	// Get a sub-image of the entire image
	bool getSprite(int x, int y, int w, int h, BLImage& sprite)
	{
		size_t pixelOffset = y * fImageData.stride + (x * sizeof(BLRgba32));
		uint8_t* pixelPtr = &(((uint8_t*)fImageData.pixelData)[pixelOffset]);
		BLResult bResult = blImageInitAsFromData(&sprite, w, h, BL_FORMAT_PRGB32, pixelPtr, fImageData.stride, BLDataAccessFlags::BL_DATA_ACCESS_RW, nullptr, nullptr);

		return bResult == 0;
	}
};
