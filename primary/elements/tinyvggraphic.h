#pragma once

#include "blend2d.h"
#include "tvgparse.h"
#include "filestream.h"

// A combination of VG commands
// and path objects to match

using namespace tinyvg;

struct VGCommandPath
{
	int fCommand;

	BLVar fFillStyle{};
	BLVar fLineStyle{};
	
	float fLineWidth;
	BLPath fPath{};

private:
	// convert a tinyvg_style_t to a BLStyle
	bool initStyle(BLVar& s, tvg_style_t& tvgs)
	{

		switch (tvgs.kind) {
		case DrawingStyle::FlatColored:
		{
			BLRgba32 c = BLRgba32(tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
			blVarAssignRgba32(&s, c.value);
			//s = c; // does not work yet
		}
		break;

		case DrawingStyle::LinearGradient:
		case DrawingStyle::RadialGradient:
		{
			if (tvgs.kind == DrawingStyle::LinearGradient) {
				BLGradient linear(BLLinearGradientValues(tvgs.point_0.x, tvgs.point_0.y, tvgs.point_1.x, tvgs.point_1.y));
				linear.addStop(0, BLRgba32(tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a));
				linear.addStop(1, BLRgba32(tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a));

				s = linear;
			}
			else if (tvgs.kind == DrawingStyle::RadialGradient) {
				double dx = tvgs.point_1.x - tvgs.point_0.x;
				double dy = tvgs.point_1.y - tvgs.point_0.y;
				double r0 = ::sqrt((dx * dx) + (dy * dy));	// distance

				BLGradient radial(BLRadialGradientValues(tvgs.point_0.x, tvgs.point_0.y, tvgs.point_0.x, tvgs.point_0.y, r0));
				radial.addStop(0, BLRgba32(tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a));
				radial.addStop(1, BLRgba32(tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a));

				s = radial;
			}
		}
		break;

		}
		return true;
	}

	// Construct a path from contour commands
	bool addContour(BLPath& segment, tvg_contour_command_t& cmd)
	{
		switch (cmd.kind)
		{
		case ContourCommands::moveTo:
			segment.moveTo(cmd.moveTo.point_0.x, cmd.moveTo.point_0.y);
			break;

		case ContourCommands::line:
		{
			segment.addLine(BLLine(
				cmd.line.point_0.x, cmd.line.point_0.y,
				cmd.line.point_1.x, cmd.line.point_1.y));
		}
		break;

		case ContourCommands::rectangle:
		{
			segment.addRect(BLRect(cmd.rect.x, cmd.rect.y, cmd.rect.width, cmd.rect.height));
		}
		break;

		case ContourCommands::lineTo:
		{
			segment.lineTo(cmd.lineTo.point_1.x, cmd.lineTo.point_1.y);
		}
		break;
		case ContourCommands::hlineTo:
		{
			BLPoint vtxOut;
			segment.getLastVertex(&vtxOut);
			segment.lineTo(cmd.hlineTo.x, vtxOut.y);
		}
		break;
		case ContourCommands::vlineTo:
		{
			BLPoint vtxOut;
			segment.getLastVertex(&vtxOut);
			segment.lineTo(vtxOut.x, cmd.vlineTo.y);
		}
		break;

		case ContourCommands::cubicBezierTo:
		{
			segment.cubicTo(cmd.cubicBezierTo.control_0.x, cmd.cubicBezierTo.control_0.y,
				cmd.cubicBezierTo.control_1.x, cmd.cubicBezierTo.control_1.y,
				cmd.cubicBezierTo.point_1.x, cmd.cubicBezierTo.point_1.y);
		}
		break;

		case ContourCommands::quadraticBezierTo:
		{
			segment.quadTo(cmd.quadraticBezierTo.control_0.x, cmd.quadraticBezierTo.control_0.y,
				cmd.quadraticBezierTo.point_1.x, cmd.quadraticBezierTo.point_1.y);
		}
		break;

		case ContourCommands::arcCircleTo:
		{
			segment.ellipticArcTo(BLPoint(cmd.arcCircleTo.radius, cmd.arcCircleTo.radius), 0, cmd.arcCircleTo.largeArc, cmd.arcCircleTo.sweep, BLPoint(cmd.arcCircleTo.target.x, cmd.arcCircleTo.target.y));
		}
		break;

		case ContourCommands::arcEllipseTo:
		{
			segment.ellipticArcTo(BLPoint(cmd.arcEllipseTo.radiusxy.x, cmd.arcEllipseTo.radiusxy.y), cmd.arcEllipseTo.rotation, cmd.arcEllipseTo.largeArc, cmd.arcEllipseTo.sweep, BLPoint(cmd.arcEllipseTo.target.x, cmd.arcEllipseTo.target.y));
		}
		break;

		case ContourCommands::closePath:
			segment.close();
			return false;
			break;
		}

		return true;
	}

public:
	VGCommandPath(tvg_command_t cmd)
	{
		fCommand = cmd.command;
		fLineWidth = cmd.lineWidth;

		initStyle(fLineStyle, cmd.lineStyle);
		initStyle(fFillStyle, cmd.fillStyle);

		// construct a path from the sequence of contours
		for (int i = 0; i < cmd.contours.size(); i++)
		{
			// for each contour
			addContour(fPath, cmd.contours[i]);
		}
	}

	void draw(IGraphics & ctx)
	{
		switch (fCommand) {
		case Commands::DrawLinePath:
		case Commands::DrawLines:
		case Commands::DrawLineLoop:
		case Commands::DrawLineStrip:
		{
			ctx.noFill();

			ctx.stroke(fLineStyle);

			ctx.strokeWeight(fLineWidth);
			ctx.path(fPath);
		}
		break;

		case Commands::FillPath:
		case Commands::FillPolygon:
		case Commands::FillRectangles:
		{
			ctx.noStroke();
			ctx.fill(fFillStyle);
			ctx.path(fPath);
		}
		break;

		case Commands::OutlineFillPath:
		case Commands::OutlineFillRectangles:
		case Commands::OutlineFillPolygon:
		{

			//ctx.fill(fillStyle.color_0.r, fillStyle.color_0.g, fillStyle.color_0.b);
			//ctx.stroke(lineStyle.color_0.r, lineStyle.color_0.g, lineStyle.color_0.b);

			ctx.fill(fFillStyle);
			ctx.stroke(fLineStyle);
			ctx.strokeWeight(fLineWidth);
			ctx.path(fPath);
		}
		break;


		default:
		{
			printf("UNKNOWN COMMAND: %d\n", fCommand);
		}
		}
	}
};

class TinyVGGraphic : public GraphicElement
{
	BinStream& fStream;
	std::vector<VGCommandPath> fFigures;

public:
	TinyVGGraphic(BinStream& bs)
		:fStream(bs)
	{
		tvgparser parser(bs);

		// Assuming the parse was ok
		// BUGBUG - should check isValid()
		setBounds(maths::rectf{ 0, 0, (float)parser.header.width, (float)parser.header.height });
		setFrame(bounds());

		// Construct our graphic commands
		while (true)
		{
			tvg_command_t cmd;
			if (!parser.next(cmd))
				break;

			VGCommandPath fig(cmd);
			fFigures.push_back(fig);
		}
	}

	void drawSelf(IGraphics & ctx) override
	{
		for (int i = 0; i < fFigures.size(); i++)
		{
			fFigures[i].draw(ctx);
		}
	}

	static std::shared_ptr<TinyVGGraphic> createFromStream(BinStream& bs)
	{
		auto g = std::make_shared<TinyVGGraphic>(bs);

		return g;
	}

	static std::shared_ptr<TinyVGGraphic> createFromFilename(const char *filename)
	{
		ndt::FileStream fs(filename);
		auto g = std::make_shared<TinyVGGraphic>(fs);

		return g;
	}
};
