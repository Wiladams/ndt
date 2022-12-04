#pragma once

#include "coloring.h"
#include "graphic.hpp"
#include "mmap.hpp"

#include <memory>

using namespace p5;

class Histogram : public GraphicGroup
{
	size_t fDatagram[256];
	size_t fBiggest;
	size_t fSize;
};

class FileHistogram : public GraphicGroup
{
	size_t histogram[256];
	size_t biggest;
	size_t size;

	void createHistogram(ndt::DataCursor &cur)
	{
		memset(histogram, 0, sizeof(histogram));
		biggest = 0;

		// populate the histogram
		size = 0;

		while (!ndt::isEOF(cur)) 
		{
			size++;
			uint8_t c = ndt::get_u8(cur);
			histogram[c] += 1;
		}


		// Run through the histogram to see what 
		// the biggest value is
		biggest = 0;
		for (size_t i = 0; i < 256; i++) {
			biggest = maths::max(biggest, histogram[i]);
		}
	}



public:

	FileHistogram(ndt::DataCursor &cur)
		:GraphicGroup(0,0,256,256)
	{
		createHistogram(cur);
	}

	void draw(IGraphics & ctx)
	{
		// Display histogram
		ctx.noStroke();
		ctx.fill(0xc0);
		for (int i = 0; i < 256; i++) {
			int x = (int)map(i, 0, 255, 1, 254);
			int h = (int)map((double)histogram[i], 0, (double)biggest, 0, 255);

			// Figure out the color
			// red smallest, blue highest
			double wl = map((double)histogram[i], 0, (double)biggest, 780, 380);
			auto c = ndt::ColorRGBAFromWavelength(wl, 1.5);
			auto p = BLRgba32(uint32_t(c.r * 255), uint32_t(c.g * 255), uint32_t(c.b * 255), 255);
			
			ctx.fill(p);
			ctx.rect(x, (double)255 - h, 2, h);
		}
	}

	static std::shared_ptr<FileHistogram> fromFile(std::string filename)
	{
		auto fmap = ndt::mmap::create_shared(filename);
		if (fmap == nullptr || !fmap->isValid())
			return std::shared_ptr<FileHistogram>{};

		auto cur = fmap->createCursor();
		std::shared_ptr<FileHistogram> res = std::make_shared<FileHistogram>(cur);

		return res;
	}

	static std::shared_ptr<FileHistogram> fromStream(ndt::DataCursor &cur)
	{
		std::shared_ptr<FileHistogram> res = std::make_shared<FileHistogram>(cur);
		return res;
	}
};

