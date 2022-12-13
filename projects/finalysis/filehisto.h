#pragma once

#include "coloring.h"
#include "graphic.hpp"
#include "mmap.hpp"

#include <memory>

using namespace p5;

struct Histogram
{
	size_t fAccumulation[256]{ 0 };
	size_t fBiggest{ 0 };
	size_t fSize{ 0 };

	Histogram() { ; }

	void reset(ndt::DataCursor& cur)
	{
		while (!ndt::isEOF(cur))
		{
			fSize++;
			uint8_t c = ndt::get_u8(cur);
			fAccumulation[c] += 1;
		}

		// Run through the histogram to see what 
		// the biggest value is

		for (size_t i = 0; i < 256; i++) {
			fBiggest = maths::max(fBiggest, fAccumulation[i]);
		}
	}

	constexpr size_t biggest() const { return fBiggest; }
	uint8_t operator[](size_t idx) { return fAccumulation[idx]; }
};

class FileHistogram : public GraphicGroup
{
	Histogram fHistory{};

public:

	FileHistogram(ndt::DataCursor &cur)
		:GraphicGroup(0,0,256,256)
	{
		fHistory.reset(cur);
	}

	void draw(IGraphics & ctx)
	{
		// Display histogram
		ctx.noStroke();
		ctx.fill(0xc0);
		for (int i = 0; i < 256; i++) {
			int x = (int)map(i, 0, 255, 1, 254);
			int h = (int)map((double)fHistory[i], 0, (double)fHistory.biggest(), 0, 255);

			// Figure out the color
			// red smallest, blue highest
			double wl = map((double)fHistory[i], 0, (double)fHistory.biggest(), 780, 380);
			auto c = maths::float_to_byte(ndt::ColorRGBAFromWavelength(wl, 1.5));
			
			auto p = BLRgba32(uint32_t(c.r), uint32_t(c.g), uint32_t(c.b), 255);
			
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

