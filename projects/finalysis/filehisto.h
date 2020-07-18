#pragma once

#include "p5.hpp"
#include <memory>

#include "filestream.h"
#include "gview.h"
#include "gwindow.h"
#include "coloring.h"

using namespace p5;

class FileHistogram : public IDrawable
{
	size_t histogram[256];
	size_t biggest;
	size_t size;

	void createHistogram(BinStream& bs)
	{
		memset(histogram, 0, sizeof(histogram));
		biggest = 0;

		// populate the histogram
		size = 0;

		while (!bs.isEOF()) 
		{
			size++;
			uint8_t c = bs.readOctet();
			histogram[c] += 1;
		}


		// Run through the histogram to see what 
		// the biggest value is
		biggest = 0;
		for (size_t i = 0; i < 256; i++) {
			biggest = maths::Max(biggest, histogram[i]);
		}
	}



public:

	FileHistogram(BinStream& bs)
	{
		createHistogram(bs);
	}

	void draw(IGraphics* ctx)
	{
		// Display histogram
		ctx->noStroke();
		ctx->fill(0xc0);
		for (int i = 0; i < 256; i++) {
			int x = (int)map(i, 0, 256, 0, 256);
			int h = (int)map(histogram[i], 0, biggest, 0, 256);

			// Figure out the color
			// red smallest, blue highest
			double wl = map(h, 0, 256, 780, 380);
			auto c = ndt::ColorRGBAFromWavelength(wl, 1.5);
			auto p = BLRgba32(c.r * 255, c.g * 255, c.b * 255, 255);
			ctx->fill(p);

			ctx->rect(x, (double)256 - h, 2, h);
		}
	}

	static shared_ptr<FileHistogram> fromFile(std::string filename)
	{
		FileStream fStream(filename.c_str());
		if (!fStream.isValid())
			return nullptr;

		std::shared_ptr<FileHistogram> res = std::make_shared<FileHistogram>(fStream);
		return res;
	}

	static shared_ptr<FileHistogram> fromStream(BinStream &bs)
	{
		std::shared_ptr<FileHistogram> res = std::make_shared<FileHistogram>(bs);
		return res;
	}
};

class FileHistoWindow : public GWindow
{
	std::string fFilename;

public:
	FileHistoWindow(std::string filename, int x, int y, int w=256, int h=256)
		: GWindow(x,y,w,h),
		fFilename(filename)
	{
		setPage(FileHistogram::fromFile(filename));
	}

	void drawBackground(IGraphics* ctx)
	{
		ctx->push();
		ctx->background(color(255, 250, 247));
		// draw a little frame
		ctx->stroke(0);
		ctx->noFill();
		ctx->rect(0, 0, 256, 256);

		ctx->pop();

	}

	void drawForeground(IGraphics* ctx) 
	{
		ctx->fill(color(225, 220, 220, 127));
		ctx->rect(2, 2, 254, 18);

		ctx->fill(0);
		ctx->text(fFilename.c_str(), 4, 16);
	}
};