#pragma once

//
// https://tinyvg.tech/download/specification.pdf
//
//
// Note: File format is little-endian
//
// Description: The general idea with the code here is to read the 
// tinyvg file format, and output a different data structure
// that is ready for rendering.
//

#include <cstdint>
#include <memory>

#include "binstream.hpp"
#include "bitbang.h"
#include "blend2d.h"

namespace tinyvg
{
	// Useful enumerations
	struct Commands {
		static constexpr int EndOfDocument = 0;		// end of the file
		static constexpr int FillPolygon = 1;		// fills an N-gon
		static constexpr int FillRectangles = 2;		// fills a set of rectangles
		static constexpr int FillPath = 3;		// fill a free-form path
		static constexpr int DrawLines = 4;		// draw a set of independent lines
		static constexpr int DrawLineLoop = 5;		// draw the outline of a polygon
		static constexpr int DrawLineStrip = 6;		// draw a list of connected lines
		static constexpr int DrawLinePath = 7;		// draw a free-form path outline
		static constexpr int OutlineFillPolygon = 8;		// combine fill and draw polygon
		static constexpr int OutlineFillRectangles = 9;	// combine fill and draw rectangles
		static constexpr int OutlineFillPath = 10;	// Combine fill and draw path
	};

	struct PathCommands {
		static constexpr int line = 0;
		static constexpr int horizontalLine = 1;
		static constexpr int verticalLine = 2;
		static constexpr int cubicBezier = 3;
		static constexpr int arcCircle = 4;
		static constexpr int arcEllipse = 5;
		static constexpr int closePath = 6;
		static constexpr int quadraticBezier = 7;
	};

	struct ColorEncoding {
		static constexpr int RGBA8888 = 0;
		static constexpr int RGB565 = 1;
		static constexpr int RGBAF32 = 2;
		static constexpr int Custom = 3;
	};

	struct CoordinateRange {
		static constexpr int Default = 0;	// 16-bit
		static constexpr int Reduced = 1;	// 8-bit
		static constexpr int Enhanced = 2;	// 32-bit
	};

	struct DrawingStyle {
		static constexpr int FlatColored = 0;
		static constexpr int LinearGradient = 1;
		static constexpr int RadialGradient = 2;
	};

	// Structures
	union tinyvg_rgba8888_t {
		struct {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		uint32_t v;
	};

	// Each color encoded as a sequence of two bytes
	union tinyvg_rgb565_t {
		struct {
			uint16_t r : 5;
			uint16_t g : 6;
			uint16_t b : 5;
		};
		uint16_t v;
	};

	// each color channel between 0.0 and 1.0
	union tinyvg_rgbaf32_t {
		float r;
		float g;
		float b;
		float a;
	};


	using tvg_point = BLPoint;

	struct tvg_style_t
	{
		//int kind;	// flat, linear, radial
		BLStyle style;
		//tvg_point point_0;
		//tvg_point point_1;
		//uint32_t		color_index_0;
		//uint32_t		color_index_1;

		//BLRgba32 color_0;
		//BLRgba32 color_1;

		void reset()
		{
			memset(this, 0, sizeof(*this));
		}
	};

	struct tvg_command_t
	{
		uint8_t command=0;
		int segment_count=0;
		tvg_style_t fillStyle;
		tvg_style_t lineStyle;
		float lineWidth=0.0;
		BLPath segment;

		void reset()
		{
			command = Commands::EndOfDocument;
			segment_count = 0;
			fillStyle.reset();
			lineStyle.reset();
			lineWidth = 0.0f;
			segment.reset();
		}
	};

	// simple structure to contain parsed header information
	struct tvg_header_t {
		char magic[2];
		uint8_t version;

		uint8_t scale;		// Used for creating Unit fractions
		uint8_t colorEncoding;	// How are colors represented in color table
		uint8_t coordinateRange;	// how big can coordinates be

		uint32_t width;			// Width of image
		uint32_t height;		// Height of image
		uint32_t colorCount;	// Number of entries in color table
	};


	// Primary structure is this parser
	struct tvgparser
	{
		tvg_header_t header;
		std::vector<BLRgba32> colorTable;
		BinStream& bs;
		bool isValid=false;
		bool endOfDocument = false;

		tvgparser(BinStream& bs)
			:bs(bs)
		{
			init();
		}

		bool init()
		{
			// set everything to an initial state

			endOfDocument = false;
			isValid= false;
			colorTable.clear();
			memset(&header, 0, sizeof(header));

			// read header
			auto success = readHeader();
			if (!success)
				return false;

			// read color table
			success = readColorTable();

			return success;
		}

		// read the next command
		// if we've reached End Of Document
		// or End of File, return false
		// otherwise, try to read the next command
		bool next(tvg_command_t& cmd)
		{
			if (bs.isEOF() || endOfDocument)
				return false;

			bool success = readCommand(cmd);

			return success;
		}

		// Parsing routines
		// Read a variable sized integer from the stream
		// limited to unsigned 32-bit maximum
		// calculatring binary to decimal:
		// https://www.rapidtables.com/convert/number/binary-to-decimal.html?x=0001.0011
		//
		uint32_t readUInt()
		{
			uint32_t result = 0;

			for (int count = 0; count < 5; count++)	// at most 5 iterations
			{
				auto abyte = bs.readOctet();
				auto val = (abyte & 0x7f) << (7 * count);
				result |= val;

				if ((abyte & 0x80) == 0)
					break;
			}

			return result;
		}

		// A unit is encoded as a signed integer, with a configurable
		// amount of bits (CoordinateRange)
		uint32_t readRangeInt()
		{
			uint32_t value = 0;

			switch (header.coordinateRange)
			{
			case CoordinateRange::Default:
				// 16-bit
				value = bs.readUInt16();

				break;
			case CoordinateRange::Reduced:
				// 8-bit
				value = bs.readOctet();
				break;
			case CoordinateRange::Enhanced:
				// 32-bit
				value = bs.readUInt32();
				break;
			}

			return value;
		}

		// Read a fixed value and turn it into floating point
		float readUnit()
		{
			uint32_t value = readRangeInt();
			return (float)value / (float)(1 << header.scale);
		}

		tvg_point readPoint()
		{
			tvg_point pt;
			pt.x = readUnit();
			pt.y = readUnit();

			return pt;
		}

		tvg_style_t  readStyle(int kind)
		{
			tvg_style_t s;
			//s.kind = kind;

			switch (kind) {
			case DrawingStyle::FlatColored:
			{
				int color_index_0 = readUInt();
				auto color_0 = colorTable[color_index_0];
				s.style = BLStyle(color_0);
			}
				break;

			case DrawingStyle::LinearGradient: 
			case DrawingStyle::RadialGradient: 
			{
				//printf("LinearGradient\n");
				auto point_0 = readPoint();
				auto point_1 = readPoint();
				auto color_index_0 = readUInt();
				auto color_index_1 = readUInt();

				auto color_0 = colorTable[color_index_0];
				auto color_1 = colorTable[color_index_1];

				if (kind == DrawingStyle::LinearGradient) {
					BLGradient linear(BLLinearGradientValues(point_0.x, point_0.y, point_1.x, point_1.y));
					linear.addStop(0, color_0);
					linear.addStop(1, color_1);
					s.style = BLStyle(linear);
				}
				else if (kind == DrawingStyle::RadialGradient) {
					double dx = point_1.x - point_0.x;
					double dy = point_1.y - point_0.y;
					double r0 = ::sqrt((dx * dx) + (dy * dy));	// distance

					BLGradient radial(BLRadialGradientValues(point_0.x, point_0.y, point_0.x, point_0.y, r0));
					radial.addStop(0, color_0);
					radial.addStop(1, color_1);
					s.style = BLStyle(radial);
				}
			}
			break;

			}
			return s;
		}



		bool readHeader()
		{
			// read magic
			bs.readBytes((uint8_t*)&header.magic, 2);

			// read version
			header.version = bs.readOctet();

			// read scale : u4
			// read color encoding : u2
			// read coordinate range : u2
			uint8_t bits = bs.readOctet();
			header.scale = ndt::BITSVALUE(bits, 0, 3);
			header.colorEncoding = ndt::BITSVALUE(bits, 4, 5);
			header.coordinateRange = ndt::BITSVALUE(bits, 6, 7);

			// read width
			// read height
			header.width = readRangeInt();
			header.height = readRangeInt();

			// read color count
			header.colorCount = readUInt();

			return true;
		}

		bool readColorTable()
		{
			for (int i = 0; i < header.colorCount; i++) {
				// read a color and convert to BLRgba32
				switch (header.colorEncoding) {
				case ColorEncoding::RGB565:
				{
					tinyvg_rgb565_t c;
					c.v = bs.readUInt16();
					colorTable.push_back(BLRgba32(c.r, c.g, c.b));
				}
				break;

				case ColorEncoding::RGBA8888:
				{
					tinyvg_rgba8888_t c;
					c.v = bs.readUInt32();
					colorTable.push_back(BLRgba32(c.r, c.g, c.b, c.a));
				}

				break;

				case ColorEncoding::RGBAF32:
				{
					tinyvg_rgbaf32_t c;
					c.r = bs.readFloat();
					c.g = bs.readFloat();
					c.b = bs.readFloat();
					c.a = bs.readFloat();
					colorTable.push_back(BLRgba32((int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255), (int)(c.a * 255)));
				}
				break;

				default:
					break;
				}
			}

			return true;
		}

		// Read commands, filling in the blend2d path as we go along
		// Command has...
		//  command_index
		//  primary_style
		// Then, depending on the command, a collection 
		// of various paths, and styles
		//
		bool readCommand(tvg_command_t& cmd)
		{
			uint8_t cmdBits = bs.readOctet();
			cmd.command = ndt::BITSVALUE(cmdBits, 0, 5);
			uint8_t primary_style_kind = ndt::BITSVALUE(cmdBits, 6, 7);

			switch (cmd.command)
			{
			case Commands::EndOfDocument:
			{
				//printf("EndOfDocument\n");

				endOfDocument = true;
				return false;
			}
			break;

			case Commands::DrawLineLoop:
			{
				//printf("DrawLineLoop\n");
				readLineHeader(cmd, primary_style_kind);

				auto pt1 = readPoint();
				cmd.segment.moveTo(pt1);
				for (int i = 1; i < cmd.segment_count; i++)
				{
					auto pt = readPoint();
					cmd.segment.lineTo(pt);
				}
				cmd.segment.close();
			}
			break;

			case Commands::DrawLinePath:
			{
				//printf("DrawLinePath\n");
				readLineHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::DrawLines:
			{
				//printf("DrawLines\n");
				readLineHeader(cmd, primary_style_kind);

				for (int i = 0; i < cmd.segment_count; i++) {
					auto p1 = readPoint();
					auto p2 = readPoint();
					cmd.segment.addLine(BLLine(p1.x, p1.y, p2.x, p2.y));
				}
			}
			break;

			case Commands::DrawLineStrip:
			{
				//printf("DrawLineStrip\n");
				readLineHeader(cmd, primary_style_kind);
				auto pt1 = readPoint();
				cmd.segment.moveTo(pt1);
				for (int i = 1; i < cmd.segment_count; i++)
				{
					auto pt = readPoint();
					cmd.segment.lineTo(pt);
				}
			}
			break;

			case Commands::FillPath:
			{
				//printf("FillPath\n");

				readFillHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::FillPolygon:
			{
				//printf("FillPolygon\n");

				readFillHeader(cmd, primary_style_kind);

				BLPoint* points = new BLPoint[cmd.segment_count];


				for (int i = 1; i <= cmd.segment_count; i++)
				{
					points[i - 1] = readPoint();
				}
				cmd.segment.addPolygon(points, cmd.segment_count);

				delete [] points;

			}
			break;

			case Commands::FillRectangles:
			{
				//printf("FillRectangles\n");
				readFillHeader(cmd, primary_style_kind);
				BLRect* rects = new BLRect[cmd.segment_count];

				for (int i = 0; i < cmd.segment_count; i++)
				{
					auto x = readUnit();
					auto y = readUnit();
					auto w = readUnit();
					auto h = readUnit();
					rects[i] = BLRect(x, y, w, h);
				}
				cmd.segment.addRectArray(rects, cmd.segment_count);

				delete [] rects;
			}
			break;

			case Commands::OutlineFillPath:
			{
				//printf("OutlineFillPath\n");

				readOutlineFillHeader(cmd, primary_style_kind);
				readPath(cmd);
			}
			break;

			case Commands::OutlineFillPolygon:
			{
				//printf("OutlineFillPolygon\n");

				readOutlineFillHeader(cmd, primary_style_kind);

				BLPoint* points = new BLPoint[cmd.segment_count];


				for (int i = 1; i <= cmd.segment_count; i++)
				{
					points[i - 1] = readPoint();
				}
				cmd.segment.addPolygon(points, cmd.segment_count);

				delete [] points;
			}
			break;

			case Commands::OutlineFillRectangles:
			{
				//printf("OutlineFillRectangles\n");
				readOutlineFillHeader(cmd, primary_style_kind);
				BLRect* rects = new BLRect[cmd.segment_count];

				for (int i = 0; i < cmd.segment_count; i++)
				{
					auto x = readUnit();
					auto y = readUnit();
					auto w = readUnit();
					auto h = readUnit();
					rects[i] = BLRect(x, y, w, h);
				}
				cmd.segment.addRectArray(rects, cmd.segment_count);

				delete [] rects;
			}
			break;

			default: {
				printf("Unknown Command: %d\n", cmd.command);
				// Unknown command, simply return false
				return false;
			}
				   break;
			}

			return true;
		}

		bool readLineHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			cmd.segment_count = readUInt() + 1;
			cmd.lineStyle = readStyle(primary_style_kind);
			cmd.lineWidth = readUnit();

			return true;
		}

		bool readFillHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			cmd.segment_count = readUInt()+1;
			cmd.fillStyle = readStyle(primary_style_kind);

			return true;
		}

		bool readOutlineFillHeader(tvg_command_t& cmd, int primary_style_kind)
		{
			// segment count and style
			uint32_t bits = bs.readOctet();
			cmd.segment_count = ndt::BITSVALUE(bits, 0, 5) + 1;
			int scondary_style_kind = ndt::BITSVALUE(bits, 6, 7);


			cmd.fillStyle = readStyle(primary_style_kind);
			cmd.lineStyle = readStyle(scondary_style_kind);

			// line_width unit
			cmd.lineWidth = readUnit();

			return true;
		}

		bool readPath(tvg_command_t &cmd)
		{
			// Get the segment lengths
			int total_node_count = 0;
			std::vector<int> segment_lengths;

			int i = 0;

			while (i < cmd.segment_count)
			{
				segment_lengths.push_back(readUInt() + 1) ;
				total_node_count += segment_lengths[i];

				i += 1;
			}

			for (int i = 0; i < cmd.segment_count; i++) {
				BLPath segment;
				readSegment(segment, segment_lengths[i]);

				// BUGBUG - do segments need to be explicitly closed?
				//segment.close();

				cmd.segment.addPath(segment);
			}

			// BUGBUG - does a path need to be explicitly closed?
			//cmd.segment.close();

			return true;
		}

		//
		// A segment is a piece of geometry with a maximum of 64 points
		// other than the line width, there is no other drawing info
		// here.
		// Need to capture the line width if it's specified and
		// convey it back.
		bool readSegment(BLPath& segment, int command_count)
		{
			// Starting point
			tvg_point startPoint = readPoint();
			tvg_point lastPoint = startPoint;

			segment.moveTo(startPoint.x, startPoint.y);
			//printf("moveTo(%3.2f,%3.2f);\n", startPoint.x, startPoint.y);

			// For each instruction
			for (int insCtr = 1; insCtr <= command_count; insCtr++)
			{
				// First byte is packed with goodness
				// decode it
				uint8_t insBits = bs.readOctet();
				uint8_t cmd = ndt::BITSVALUE(insBits, 0, 2);
				uint8_t padding = ndt::BITSVALUE(insBits, 3, 3);	// always 0
				uint8_t hasLineWidth = ndt::BITSVALUE(insBits, 4, 4);
				uint8_t morepadding = ndt::BITSVALUE(insBits, 5, 7);	// always 0

				float lineWidth = 0.0f;
				if (hasLineWidth)
					lineWidth = readUnit();

				// Read command specifics
				switch (cmd)
				{
				case PathCommands::line:
				{
					auto nextPoint = readPoint();

					//printf("lineTo(%3.2f,%3.2f);\n", nextPoint.x, nextPoint.y);
					segment.lineTo(nextPoint.x, nextPoint.y);
					lastPoint = nextPoint;
				}
				break;
				case PathCommands::horizontalLine: {
					double x = readUnit();
					
					//printf("hline\n");
					segment.lineTo(x, lastPoint.y);
					lastPoint.x = x;
				}
												 break;
				case PathCommands::verticalLine: {
					//printf("vline\n");
					double y = readUnit();
					segment.lineTo(lastPoint.x, y);

					lastPoint.y = y;
				}
				break;
				
				case PathCommands::cubicBezier: {
					//printf("cubicBezier\n");
					tvg_point control_0 = readPoint();
					tvg_point control_1 = readPoint();
					tvg_point point_1 = readPoint();

					// draw cubic bezier using lastPoint as first point
					// and point_1 as last point
					segment.cubicTo(control_0.x, control_0.y, control_1.x, control_1.y, point_1.x, point_1.y);

					lastPoint = point_1;
				}
					break;

				case PathCommands::quadraticBezier: {
					//printf("quadraticBezier\n");
					tvg_point control = readPoint();
					tvg_point point_1 = readPoint();

					segment.quadTo(control.x, control.y, point_1.x, point_1.y);
					lastPoint = point_1;
				}
					break;

				case PathCommands::arcCircle: {
					//printf("arcCircle\n");
					uint8_t flags = bs.readOctet();
					int largeArc = ndt::BITSVALUE(flags, 0, 0);
					int sweep = !ndt::BITSVALUE(flags, 1, 1);
					int padding = ndt::BITSVALUE(flags, 2, 7);	// always 0

					float radius = readUnit();
					tvg_point target = readPoint();

					segment.ellipticArcTo(BLPoint(radius,radius), 0, largeArc, sweep, target);

					lastPoint = target;
				}
					break;

				case PathCommands::arcEllipse: {
					//printf("arcEllipse\n");
					uint8_t flags = bs.readOctet();
					int sweep = !ndt::BITSVALUE(flags, 1, 1);	// 0 == anti-clockwise, 1 == clockwise
					bool largeArc = ((flags&1) != 0);
					int padding = ndt::BITSVALUE(flags, 2, 7);

					auto radius = readPoint();
					float rotation = readUnit();
					tvg_point target = readPoint();

					//printf("ellipticArcTo(BLPoint(%3.2f,%3.2f), %3.2f, %d, %d, BLPoint(%3.2f,%3.2f));\n",
					//	radius.x, radius.y, rotation, largeArc, sweep, target.x, target.y);

					segment.ellipticArcTo(radius, rotation, largeArc, sweep, target);

					lastPoint = target;
				}
					break;

				case PathCommands::closePath:
					//printf("closePath\n");
					segment.close();
					break;
				}

			}

			return true;
		}

	};

}