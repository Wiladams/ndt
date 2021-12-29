// 
// generate fiddle output for tiny vector graphic
//
#include "p5.hpp"
#include "filestream.h"
#include "tvgparse.h"

using namespace p5;
using namespace tinyvg;

// Get a stream on the intended image
FileStream fs_tiger("tiger.tvg");
FileStream fs_shield("shield.tvg");
//FileStream fs_comic("comic.tvg");
FileStream fs_chart("chart.tvg");
FileStream fs_icon("app-icon.tvg");
FileStream fs_flowchart("flowchart.tvg");
FileStream fs_everything("everything.tvg");
FileStream fs_logo("logo.tvg");

BinStream* mainStream = nullptr;
tvgparser* mainParser = nullptr;

void writeStyle(const char *name, tvg_style_t& tvgs)
{
	switch (tvgs.kind)
	{
	case DrawingStyle::FlatColored:
	{
		printf("ctx.set%sStyle(",name);
		printf("BLRgba32(%d,%d,%d,%d));\n",
			tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
	}
	break;

	case DrawingStyle::LinearGradient:
	{
		printf("  BLGradient %slinear(BLLinearGradientValues(%3.2f,%3.2f,%3.2f,%3.2f));\n", name, tvgs.point_0.x, tvgs.point_0.y, tvgs.point_1.x, tvgs.point_1.y);
		printf("  %slinear.addStop(0, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		printf("  %slinear.addStop(1, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		printf("  ctx.set%sStyle(%slinear);\n", name, name);
	}
	break;

	case DrawingStyle::RadialGradient:
	{
		double dx = tvgs.point_1.x - tvgs.point_0.x;
		double dy = tvgs.point_1.y - tvgs.point_0.y;
		double r0 = ::sqrt((dx * dx) + (dy * dy));	// distance

		printf("  BLGradient %sradial(BLRadialGradientValues(%3.2f,%3.2f,%3.2f,%3.2f,%3.2f));\n", name, tvgs.point_0.x, tvgs.point_0.y, tvgs.point_0.x, tvgs.point_0.y, r0);
		printf("  %sradial.addStop(0, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_0.r, tvgs.color_0.g, tvgs.color_0.b, tvgs.color_0.a);
		printf("  %sradial.addStop(1, BLRgba32(%d,%d,%d,%d));\n", name, tvgs.color_1.r, tvgs.color_1.g, tvgs.color_1.b, tvgs.color_1.a);
		printf("  ctx.set%sStyle(%sradial);\n", name, name);
	}
	break;

	}

}

// Construct a path from contour commands
void writeContour(tvg_contour_command_t& cmd)
{
	switch (cmd.kind)
	{
	case ContourCommands::moveTo:
		printf("segment.moveTo(%3.2f, %3.2f);\n", cmd.moveTo.point_0.x, cmd.moveTo.point_0.y);
		break;

	case ContourCommands::line:
	{
		printf("segment.addLine(BLLine(%3.2f,%3.2f,%3.2f,%3.2f));\n",
			cmd.line.point_0.x,
			cmd.line.point_0.y,
			cmd.line.point_1.x,
			cmd.line.point_1.y);
	}
	break;
	
	case ContourCommands::rectangle:
	{
		printf("segment.addRect(BLRect(%3.2f,%3.2f,%3.2f,%3.2f));\n", 
			cmd.rect.x, cmd.rect.y, cmd.rect.width, cmd.rect.height);
	}
	break;
	

	case ContourCommands::lineTo:
	{
		printf("segment.lineTo(%3.2f,%3.2f);\n", cmd.lineTo.point_1.x, cmd.lineTo.point_1.y);
	}
	break;
	
	case ContourCommands::hlineTo:
	{
		printf("// hlineto\n");
		//printf("BLPoint vtxOut;\n");
		printf("segment.getLastVertex(&vtxOut);\n");
		printf("segment.lineTo(%3.2f, vtxOut.y);\n", cmd.hlineTo.x);
	}
	break;
	case ContourCommands::vlineTo:
	{
		printf("// vlineto\n");
		printf("segment.getLastVertex(&vtxOut);\n");
		printf("segment.lineTo(vtxOut.x, %3.2f);\n", cmd.vlineTo.y);
	}
	break;
	
	case ContourCommands::cubicBezierTo:
	{
		printf("segment.cubicTo(%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f);\n",
			cmd.cubicBezierTo.control_0.x, cmd.cubicBezierTo.control_0.y,
			cmd.cubicBezierTo.control_1.x, cmd.cubicBezierTo.control_1.y,
			cmd.cubicBezierTo.point_1.x, cmd.cubicBezierTo.point_1.y);
	}
	break;

	case ContourCommands::quadraticBezierTo:
	{
		printf("  segment.quadTo(%3.2f,%3.2f,%3.2f,%3.2f);\n",
			cmd.quadraticBezierTo.control_0.x, cmd.quadraticBezierTo.control_0.y,
			cmd.quadraticBezierTo.point_1.x, cmd.quadraticBezierTo.point_1.y);
	}
	break;
	
	case ContourCommands::arcCircleTo:
	{
		printf("  segment.ellipticArcTo(BLPoint(%3.2f, %3.2f), 0, %d, %d, BLPoint(%3.2f,%3.2f));\n",
			cmd.arcCircleTo.radius, cmd.arcCircleTo.radius,
			cmd.arcCircleTo.largeArc, cmd.arcCircleTo.sweep,
			cmd.arcCircleTo.target.x, cmd.arcCircleTo.target.y);
	}
	break;

	case ContourCommands::arcEllipseTo:
	{
		printf("  segment.ellipticArcTo(BLPoint(%3.2f, %3.2f), %3.2f, %d, %d, BLPoint(%3.2f,%3.2f));\n",
			cmd.arcEllipseTo.radiusxy.x, cmd.arcEllipseTo.radiusxy.y,
			cmd.arcEllipseTo.rotation,
			cmd.arcEllipseTo.largeArc, cmd.arcEllipseTo.sweep,
			cmd.arcEllipseTo.target.x, cmd.arcEllipseTo.target.y);

	}
	break;
			
	case ContourCommands::closePath:
		printf("segment.close();\n");
		break;

	}


}

void writeCommand(tvg_command_t& cmd)
{
	switch (cmd.command) {
		case Commands::DrawLinePath:
		case Commands::DrawLines:
		case Commands::DrawLineLoop:
		case Commands::DrawLineStrip:
		{
			printf("\n  do { // Draw \n");
			printf("BLPath segment;\n");

			for (int i = 0; i<cmd.contours.size(); i++)
				writeContour(cmd.contours[i]);

			writeStyle("Stroke", cmd.lineStyle);
			printf("ctx.setStrokeWidth(%3.2f);\n", cmd.lineWidth);
			printf("ctx.strokePath(segment);\n");

			printf("} while(false);\n");
		}
		break;

		case Commands::FillPath:
		case Commands::FillPolygon:
		case Commands::FillRectangles:
		{
			printf("\n  do { // Fill \n");
			printf("    BLPath segment;\n");

			for (int i = 0; i<cmd.contours.size(); i++)
				writeContour(cmd.contours[i]);

			writeStyle("Fill", cmd.fillStyle);
			printf("    ctx.fillPath(segment);\n");

			printf("  } while(false);\n");

		}

		case Commands::OutlineFillPath:
		case Commands::OutlineFillRectangles:
		case Commands::OutlineFillPolygon:
		{
			printf("\n  do { // OutlineFill\n");
			printf("  BLPath segment;\n");

			for (int i = 0; i<cmd.contours.size(); i++)
				writeContour(cmd.contours[i]);

			writeStyle("Fill", cmd.fillStyle);
			printf("  ctx.fillPath(segment);\n");

			printf("ctx.setStrokeWidth(%3.2f);\n", cmd.lineWidth);
			writeStyle("Stroke", cmd.lineStyle);
			printf("  ctx.strokePath(segment);\n");

			printf("} while(false);\n");
		}
		break;

		default:
		{
			printf("UNKNOWN COMMAND: %d\n", cmd.command);
		}
	}
}

// Draw the entire thing in one shot
void writeStream(BinStream& bs)
{
	tvgparser p(bs);

	printf("BLImage render(const BLContextCreateInfo & cci) {\n");
	printf("  BLImage img(%d, %d, BL_FORMAT_PRGB32);\n", p.header.width, p.header.height);
	printf("  BLContext ctx(img, cci);\n");
	printf("  ctx.clearAll();\n");
	printf("  ctx.setFillStyle(BLRgba32(0xFFffffff));\n");
	printf("  ctx.fillAll();\n");

	printf("\n  BLPoint vtxOut;\n");

	while (true)
	{
		tvg_command_t cmd;
		if (!p.next(cmd))
			break;

		//printf("Another path\n");
		writeCommand(cmd);
	}

	printf("  return img;\n");
	printf("}\n");
}

void setup()
{
	//mainStream = &fs_tiger;
	//mainStream = &fs_shield;
	//mainStream = &fs_chart;
	//mainStream = &fs_everything;
	mainStream = &fs_logo;

	writeStream(*mainStream);

	halt();
}
