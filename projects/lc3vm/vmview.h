#pragma once

#include "lc3.h"
#include "p5.hpp"
#include "filestream.h"
#include "layout.h"

class StringLabel : public Graphic
{
	BLRect fFrame;
	std::string fContent;

public:
	StringLabel(const std::string content, const double x, const double y, const double w, const double h)
		:fContent(content),
		fFrame(x, y, w, h)
	{}

	virtual BLRect getFrame() const { return fFrame; }
	
	virtual void moveTo(const double x, const double y) 
	{
		fFrame.x = x; fFrame.y = y;
	}

	void draw(std::shared_ptr<IGraphics> ctx)
	{
		ctx->noStroke();
		ctx->fill(0);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		ctx->textSize(14);
		ctx->text(fContent.c_str(), fFrame.x, fFrame.y);
	}
};

class RegisterLabel : public Graphic
{
	BLRect fFrame;
	// Use binary layout

public:
	RegisterLabel(const std::string& str, int x, int y,int w,int h)
		: fFrame(x,y,w,h)
	{
		addChild(std::make_shared<StringLabel>(str,0,0,20,24));
	}

	BLRect getFrame() const { return fFrame; }
};

// A view of all the registers
class RegisterView : public Graphic
{

public:
	RegisterView(double x, double y)
		:Graphic({ x,y,200,240 })
	{
		double xoffset = 20;
		double yoffset = 40;
		double cellheight = 24;

		addChild(std::make_shared<RegisterLabel>("R0", xoffset, yoffset + (0* cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R1", xoffset, yoffset + (1 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R2", xoffset, yoffset + (2 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R3", xoffset, yoffset + (3 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R4", xoffset, yoffset + (4 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R5", xoffset, yoffset + (5 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R6", xoffset, yoffset + (6 * cellheight), 20, 24));
		addChild(std::make_shared<RegisterLabel>("R7", xoffset, yoffset + (7 * cellheight), 20, 24));
	}

};

class Lc3VMView : public Graphic
{
public:
	lc3::Lc3VM vm;

	Lc3VMView(BinStream& bs)
		: vm(bs)
	{
		// Add view of registers
		addChild(std::make_shared<RegisterView>(10, 32));

		// View of flags and PC
		// view of Memory mapped registers
		// View of memory
	}

	void drawBackground(std::shared_ptr<IGraphics> ctx)
	{
		ctx->push();
		ctx->noStroke();
		ctx->fill(0);
		ctx->textSize(18);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
		ctx->text("REGISTERS", 10, 24);

		ctx->pop();
	}

	static std::shared_ptr<Lc3VMView> fromFilename(std::string filename)
	{
		// create bigendian stream
		auto fs = FileStream(filename.c_str(), false);

		auto view = std::make_shared<Lc3VMView>(fs);

		return view;
	}
};
