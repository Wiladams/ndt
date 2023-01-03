#pragma once


#include "chunkutil.h"
#include "textscan.h"
#include "blend2d.h"

#include <cstdint>
#include <vector>
#include <string_view>

//
// PathContour - smallest addressable geometry
// PathSegment - a collection of contours
// Path        - a complete set of geometry
// Shape        - Geometry + drawing attributes
//

namespace ndt
{
    enum SVGlineJoin {
        SVG_JOIN_MITER = 0,
        SVG_JOIN_BEVEL = 1,
        SVG_JOIN_ROUND = 2

    };

    enum SVGlineCap {
        SVG_CAP_BUTT = 0,
        SVG_CAP_SQUARE = 1,
        SVG_CAP_ROUND = 2,

    };
    
    enum SVGfillRule {
        SVG_FILLRULE_NONZERO = 0,
        SVG_FILLRULE_EVENODD = 1
    };
    
    enum SVGpaintType {
        SVG_PAINT_NONE = 0,
        SVG_PAINT_COLOR = 1,
        SVG_PAINT_LINEAR_GRADIENT = 2,
        SVG_PAINT_RADIAL_GRADIENT = 3
    };
    
    // Shaper contour Commands
    // Origin from SVG path commands
    // M - move       (M, m)
    // L - line       (L, l, H, h, V, v)
    // C - cubic      (C, c, S, s)
    // Q - quad       (Q, q, T, t)
    // A - ellipticArc  (A, a)
    // Z - close        (Z, z)
    enum class SegmentKind : uint8_t
    {
        INVALID = 0
        , MoveTo = 'M'
        , MoveBy = 'm'
        , LineTo = 'L'
        , LineBy = 'l'
        , HLineTo = 'H'
        , HLineBy = 'h'
        , VLineTo = 'V'
        , VLineBy = 'v'
        , CubicTo = 'C'
        , CubicBy = 'c'
		, SCubicTo = 'S'
		, SCubicBy = 's'
        , QuadTo = 'Q'
        , QuadBy = 'q'
		, SQuadTo = 'T'
		, SQuadBy = 't'
        , ArcTo = 'A'
        , ArcBy = 'a'
        , CloseTo = 'Z'
        , CloseBy = 'z'
    };

    enum class ShapeKind : uint8_t
	{
		INVALID = 0
	    , Line = 1
        , Rect = 2
        , Circle = 3
        , Ellipse = 4
        , Polyline = 5
        , Polygon = 6
        , Path = 7
	};
    


    //
    // A Path is comprised of a number of PathSegment structures
    // Each PathSegment begins with a SegmentKind, followed by a series
    // of numbers appropriate for that segment kind. 
    //
    struct PathSegment
    {
        SegmentKind fCommand{ SegmentKind::INVALID };
        std::vector<float> fNumbers{};

        PathSegment() { ; }
        PathSegment(SegmentKind aKind) :fCommand(aKind) { ; }
        PathSegment(const PathSegment& other)
            :fCommand(other.fCommand)
            , fNumbers(other.fNumbers)
        {}

		void addNumber(float aNumber) { fNumbers.push_back(aNumber); }
		void addPoint(float x, float y) { fNumbers.push_back(x); fNumbers.push_back(y); }
    };


    
// Parse a number which may have units after it
//   1.2em
// -1.0E2em
// 2.34ex
// -2.34e3M10,20
// 
// By the end of this routine, the numchunk represents the range of the 
// captured number.
// 
// The returned chunk represents what comes next, and can be used
// to continue scanning the original inChunk
//
// Note:  We assume here that the inChunk is already positioned at the start
// of a number (including +/- sign), with no leading whitespace

    static DataChunk scanNumber(const DataChunk& inChunk, DataChunk& numchunk)
    {
        static ndt::charset digitChars("0123456789");                   // only digits
        
        DataChunk s = inChunk;
        numchunk = inChunk;
        numchunk.fEnd = inChunk.fStart;


        // sign
        if (*s == '-' || *s == '+') {
            s++;
            numchunk.fEnd = s.fStart;
        }

        // integer part
        while (s && digitChars[*s]) {
            s++;
            numchunk.fEnd = s.fStart;
        }

        if (*s == '.') {
            // decimal point
            s++;
            numchunk.fEnd = s.fStart;
            
            // fraction part
            while (s && digitChars[*s]) {
                s++;
                numchunk.fEnd = s.fStart;
            }
        }

        // exponent
        // but it could be units (em, ex)
        if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) 
        {
            s++;
            numchunk.fEnd = s.fStart;
            
			// Might be a sign
            if (*s == '-' || *s == '+') {
                s++;
                numchunk.fEnd = s.fStart;
            }
            
            // Get any remaining digits
            while (s && digitChars[*s]) {
                s++;
                numchunk.fEnd = s.fStart;
            }
        }

        return s;
    }
    
    
    //
    // collectNumbers
	// Take a list of numbers and tokenize them into a vector of floats
	// Numbers are separated by whitespace, commas, or semicolons
    //
	static void collectNumbers(const DataChunk& chunk, std::vector<float>& numbers)
	{
        static ndt::charset whitespaceChars(",;\t\n\f\r ");          // whitespace found in paths
        static ndt::charset numberChars("0123456789.+-eE");         // digits, symbols, and letters found in numbers

        
        DataChunk s = chunk;

		while (s)
		{
            // ignore whitespace
            while (s && whitespaceChars[*s])
                s++;
            
            if (!s)
                break;
            
            if (numberChars[*s])
            {
                DataChunk numChunk = s;

                while (numberChars[*s])
                {
                    s++;
                    numChunk.fEnd = s.fStart;
                }

                float afloat = 0;
                std::from_chars((const char*)numChunk.fStart, (const char*)numChunk.fEnd, afloat);

                numbers.push_back(afloat);
            }
		}
	}
    
//
// tokenizePath
// Given a string, representing a series of path segments, turn the string
// into a vector of those path segments.
// 
// This gives us a structure that can then be turned into other forms, such 
// as graphic objects.
// 
// The syntax of the commands is that of the SVG path object 'd' attribute
//

    
    static void tokenizePath(const DataChunk &chunk, std::vector<PathSegment>& commands)
    {
        static ndt::charset whitespaceChars(",\t\n\f\r ");          // whitespace found in paths
        static ndt::charset commandChars("mMlLhHvVcCqQsStTaAzZ");   // set of characters used for commands
        static ndt::charset numberChars("0123456789.+-eE");         // digits, symbols, and letters found in numbers
		static ndt::charset leadingChars("0123456789.+-");          // digits, symbols, and letters found in numbers
        static ndt::charset digitChars("0123456789");                   // only digits
        
        // Use a DataChunk as a cursor on the input
        DataChunk s = chunk;

        while (s)
        {
            // ignore leading whitespace
            while (s && whitespaceChars[*s])
                s++;

            // See what we've got at this point
            // it's either in commandChars, if we in the START state
            if (commandChars[*s])
            {
                switch (*s) {
                default:
                    //printf("CMD: %c\n", *s);
                    ndt::PathSegment cmd{};
                    cmd.fCommand = ndt::SegmentKind(*s);
                    commands.push_back(cmd);
                    s++;
                }

                continue;
            }

            // or it's something related to a number (+,-,digit,.)
            if (leadingChars[*s])
            {   
                // Start with the number chunk being empty
                // expand only if we have a valid number
                DataChunk numChunk{};

                s = scanNumber(s, numChunk);
                
				// If we have a number, add it to the last command
				if (numChunk)
				{
					float afloat = 0;
					std::from_chars((const char*)numChunk.fStart, (const char*)numChunk.fEnd, afloat);
					commands.back().addNumber(afloat);
                    //printf("  %3.3f\n", afloat);
				}
                
            }
        }
    }

    

}

namespace ndt
{
    //
    // PathBuilder
    // This object takes a series of path segments and turns them into a BLPath object
    //
    struct PathBuilder
    {
    public:
        //maths::vec2f fLastControl{};
        maths::vec2f fLastPosition{};
		maths::vec2f fLastStart{};
		SegmentKind fLastCommand{};
        
        BLPath fPath{};
        BLPath fWorkingPath{};


    public:
        BLPath& getPath() { return fPath; }

        const maths::vec2f & lastPosition() noexcept
        {
            BLPoint apoint{};
			fWorkingPath.getLastVertex(&apoint);
            
            return { (float)apoint.x, (float)apoint.y };
        }
        
        // The case where the path did not end
// with a 'Z', but we're done parsing
        void finishWorking()
        {
            if (!fWorkingPath.empty())
            {
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }

            fLastPosition = lastPosition();
        }
        
        // SVG - M
        // add working path to main path
        // reset the working path to empty
        // establish a new initial position
        void moveTo(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 2) {
                printf("moveTo - Rejected: %zd\n", cmd.fNumbers.size());
                return;
            }
            
            finishWorking();

            fWorkingPath.moveTo(cmd.fNumbers[0], cmd.fNumbers[1]);
            fLastStart = { cmd.fNumbers[0], cmd.fNumbers[1] };
            //lastPosition();
            
            // perform absolute lineTo on working path
            // if there are more nunbers
            if (cmd.fNumbers.size() > 2)
            {
				//printf("EXTENDED moveTo\n");
                
                for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
                {
                    fWorkingPath.lineTo(cmd.fNumbers[i], cmd.fNumbers[i + 1]);
                    //fLastPosition = maths::vec2f{ cmd.fNumbers[i], cmd.fNumbers[i + 1] };
                }
            }

        }

        
        // SVG - m
        void moveBy(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 2) {
                printf("moveBy - Rejected: %zd\n", cmd.fNumbers.size());
                return;
            }
            
            maths::vec2f lastPoint{};

            lastPoint = lastPosition();
            
            finishWorking();


            fWorkingPath.moveTo(lastPoint.x + cmd.fNumbers[0], lastPoint.y+cmd.fNumbers[1]);

            
            // perform relative lineBy on working path
            // if there are more nunbers
			if (cmd.fNumbers.size() > 2)
			{
				//printf("EXTENDED MOVEBY\n");
                
				for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
				{
					fWorkingPath.lineTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1]);
                    
					//fLastPosition = maths::vec2f{ lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1] };
				}
			}
        }

        // Add a line, using current point as first endpoint
        // SVG - L
		void lineTo(const PathSegment& cmd)
		{
            if (cmd.fNumbers.size() < 2)
            {
                printf("lineTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
            
			fWorkingPath.lineTo(cmd.fNumbers[0], cmd.fNumbers[1]);

            
			if (cmd.fNumbers.size() > 2)
			{
				//printf("EXTENDED lineTo\n");

				for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
				{
					fWorkingPath.lineTo(cmd.fNumbers[i], cmd.fNumbers[i + 1]);
				}
			}

		}


        // Add a line using relative coordinates
        //SVG - l
        void lineBy(const PathSegment &cmd)
		{
            if (cmd.fNumbers.size() < 2)
            {
                printf("lineBy - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.lineTo(lastPosition().x + cmd.fNumbers[0], lastPosition().y + cmd.fNumbers[1]);

            if (cmd.fNumbers.size() > 2)
			{
				//printf("EXTENDED lineBy\n");

				for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
				{
					fWorkingPath.lineTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1]);
				}
			}
		}


        // Add horizontal line, using only x coordinate added to existing point
        // SVG - H
        void hLineTo(const PathSegment &cmd)
        {
            if (cmd.fNumbers.size() < 1)
            {
                printf("hLineTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
            fWorkingPath.lineTo(cmd.fNumbers[0], lastPosition().y);
            
			if (cmd.fNumbers.size() > 1)
			{
				//printf("EXTENDED hLineTo\n");

				for (size_t i = 1; i < cmd.fNumbers.size(); i++)
				{
					fWorkingPath.lineTo(cmd.fNumbers[i], lastPosition().y);
				}
			}
        }

        // SVG - h
		void hLineBy(const PathSegment& cmd)
		{
            if (cmd.fNumbers.size() < 1) {
                printf("hLineBy - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.lineTo(lastPosition().x + cmd.fNumbers[0], lastPosition().y);

			if (cmd.fNumbers.size() > 1)
			{
				//printf("EXTENDED hLineBy\n");

				for (size_t i = 1; i < cmd.fNumbers.size(); i++)
				{
					fWorkingPath.lineTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y);
				}
			}
		}

        // SVG - V
        void vLineTo(const PathSegment &cmd)
        {
            if (cmd.fNumbers.size() < 1) {
                printf("vLineTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
            fWorkingPath.lineTo(lastPosition().x, cmd.fNumbers[0]);
            
            if (cmd.fNumbers.size() > 1)
            {
				//printf("EXTENDED vLineTo\n");

				for (size_t i = 1; i < cmd.fNumbers.size(); i++)
				{
					fWorkingPath.lineTo(lastPosition().x, cmd.fNumbers[i]);
				}
            }
        }

        // SVG - v
        void vLineBy(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 1) {
                printf("vLineBy - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.lineTo(lastPosition().x, lastPosition().y + cmd.fNumbers[0]);

            if (cmd.fNumbers.size() > 1)
            {
                //printf("EXTENDED vLineBy\n");
                for (size_t i = 1; i < cmd.fNumbers.size(); i++)
                {
                    fWorkingPath.lineTo(lastPosition().x, lastPosition().y + cmd.fNumbers[i]);
                }
            }
        }


        // SVG - Q
        // Quadratic Bezier curve
        // consumes 2 points (4 numbers)
		void quadTo(const PathSegment& cmd)
		{
            if (cmd.fNumbers.size() < 4) {
                printf("quadTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.quadTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3]);

			if (cmd.fNumbers.size() > 4)
			{
				//printf("EXTENDED quadTo\n");
				for (size_t i = 4; i < cmd.fNumbers.size(); i += 4)
				{
					fWorkingPath.quadTo(cmd.fNumbers[i], cmd.fNumbers[i + 1], cmd.fNumbers[i + 2], cmd.fNumbers[i + 3]);

				}
			}
		}

		// SVG - q
		// Quadratic Bezier curve, relative coordinates
		void quadBy(const PathSegment& cmd)
		{
            if (cmd.fNumbers.size() < 4) {
                printf("quadBy - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.quadTo(lastPosition().x + cmd.fNumbers[0], lastPosition().y + cmd.fNumbers[1], lastPosition().x + cmd.fNumbers[2], lastPosition().y + cmd.fNumbers[3]);

			if (cmd.fNumbers.size() > 4)
			{
				//printf("EXTENDED quadBy\n");
				for (size_t i = 4; i < cmd.fNumbers.size(); i += 4)
				{
					fWorkingPath.quadTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1], lastPosition().x + cmd.fNumbers[i + 2], lastPosition().y + cmd.fNumbers[i + 3]);
				}
			}
		}
        
        // SVG - T
		// Smooth quadratic Bezier curve
        void smoothQuadTo(const PathSegment& cmd)
        {
            //printf("== NYI - smoothQuadTo ==\n");
            
            if (cmd.fNumbers.size() < 2) {
                printf("smoothQuadTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }

			fWorkingPath.smoothQuadTo(cmd.fNumbers[0], cmd.fNumbers[1]);

            if (cmd.fNumbers.size() > 2)
			{
				//printf("EXTENDED smoothQuadTo\n");
				for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
				{
					fWorkingPath.smoothQuadTo(cmd.fNumbers[i], cmd.fNumbers[i + 1]);
				}
			}
        }
        
		// SVG - t
		// Smooth quadratic Bezier curve, relative coordinates
        void smoothQuadBy(const PathSegment& cmd)
        {
            
			if (cmd.fNumbers.size() < 2) {
				printf("smoothQuadBy - Rejected: %zd\n", cmd.fNumbers.size());

				return;
			}

			fWorkingPath.smoothQuadTo(lastPosition().x + cmd.fNumbers[0], lastPosition().y + cmd.fNumbers[1]);

            if (cmd.fNumbers.size() > 2)
            {
				//printf("EXTENDED smoothQuadBy\n");
				for (size_t i = 2; i < cmd.fNumbers.size(); i += 2)
				{
					fWorkingPath.smoothQuadTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1]);
				}
            }
        }
        
        // SVG - C
		// Cubic Bezier curve
        void cubicTo(const PathSegment &cmd)
		{
            if (cmd.fNumbers.size() < 6) {
                printf("cubicTo - Rejected: %zd\n", cmd.fNumbers.size());

                return;
            }
            
			fWorkingPath.cubicTo(BLPoint(cmd.fNumbers[0], cmd.fNumbers[1]), BLPoint(cmd.fNumbers[2], cmd.fNumbers[3]), BLPoint(cmd.fNumbers[4], cmd.fNumbers[5]));

			if (cmd.fNumbers.size() > 6)
			{
				//printf("EXTENDED cubicTo\n");
				for (size_t i = 6; i < cmd.fNumbers.size(); i += 6)
				{
					fWorkingPath.cubicTo(BLPoint(cmd.fNumbers[i], cmd.fNumbers[i + 1]), BLPoint(cmd.fNumbers[i + 2], cmd.fNumbers[i + 3]), BLPoint(cmd.fNumbers[i + 4], cmd.fNumbers[i + 5]));
				}
			}
		}

        // SVG - c
		// Cubic Bezier curve, relative coordinates
        void cubicBy(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 6) {
				printf("cubicBy - Rejected: %zd\n", cmd.fNumbers.size());
                return;
            }
            
			fWorkingPath.cubicTo(BLPoint(lastPosition().x + cmd.fNumbers[0], lastPosition().y + cmd.fNumbers[1]), BLPoint(lastPosition().x + cmd.fNumbers[2], lastPosition().y + cmd.fNumbers[3]), BLPoint(lastPosition().x + cmd.fNumbers[4], lastPosition().y + cmd.fNumbers[5]));

            if (cmd.fNumbers.size() > 6)
            {
                //printf("EXTENDED cubicBy\n");
                for (size_t i = 6; i < cmd.fNumbers.size(); i += 6)
                {
                    fWorkingPath.cubicTo(BLPoint(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1]), BLPoint(lastPosition().x + cmd.fNumbers[i + 2], lastPosition().y + cmd.fNumbers[i + 3]), BLPoint(lastPosition().x + cmd.fNumbers[i + 4], lastPosition().y + cmd.fNumbers[i + 5]));
                }
            }
        }

        // SVG - S, smooth cubicTo

        void smoothCubicTo(const PathSegment& cmd)
        {
            
			if (cmd.fNumbers.size() < 4) {
                printf("smoothCubicTo - Rejected: %zd\n", cmd.fNumbers.size());
				return;
			}

            fWorkingPath.smoothCubicTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3]);   

			if (cmd.fNumbers.size() > 4)
			{
				//printf("EXTENDED smoothCubicTo\n");
				for (size_t i = 4; i < cmd.fNumbers.size(); i += 4)
				{
					fWorkingPath.smoothCubicTo(cmd.fNumbers[i], cmd.fNumbers[i + 1], cmd.fNumbers[i + 2], cmd.fNumbers[i + 3]);
				}
			}

        }
        
        // SVG - s, smooth cubicBy
        void smoothCubicBy(const PathSegment& cmd)
        {
            //printf("== NYI - smoothCubicBy ==\n");
			if (cmd.fNumbers.size() < 4) {
                printf("smoothCubicBy - Rejected: %zd\n", cmd.fNumbers.size());
				return;
			}

			fWorkingPath.smoothCubicTo(lastPosition().x + cmd.fNumbers[0], lastPosition().y + cmd.fNumbers[1], lastPosition().x + cmd.fNumbers[2], lastPosition().y + cmd.fNumbers[3]);

			if (cmd.fNumbers.size() > 4)
			{
				//printf("EXTENDED smoothCubicBy\n");
				for (size_t i = 4; i < cmd.fNumbers.size(); i += 4)
				{
					fWorkingPath.smoothCubicTo(lastPosition().x + cmd.fNumbers[i], lastPosition().y + cmd.fNumbers[i + 1], lastPosition().x + cmd.fNumbers[i + 2], lastPosition().y + cmd.fNumbers[i + 3]);
				}
			}
        }
        
        // SVG - A
        void arcTo(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 7) {
                printf("arcTo - Rejected: %zd\n", cmd.fNumbers.size());
                return;
            }
            
            float rx = cmd.fNumbers[0];
			float ry = cmd.fNumbers[1];
			float xRotation = cmd.fNumbers[2];
			float largeArcFlag = cmd.fNumbers[3];
			float sweepFlag = cmd.fNumbers[4];
			float x = cmd.fNumbers[5];
			float y = cmd.fNumbers[6];
            
			bool larc = largeArcFlag > 0.5f;
			bool swp = sweepFlag > 0.5f;
			float rotation = maths::radians(xRotation);

            fWorkingPath.ellipticArcTo(BLPoint(rx, ry), rotation, larc, swp, BLPoint(x, y));

            if (cmd.fNumbers.size() > 7)
            {
				//printf("EXTENDED arcTo\n");
				for (size_t i = 7; i < cmd.fNumbers.size(); i += 7)
				{
					rx = cmd.fNumbers[i];
					ry = cmd.fNumbers[i + 1];
					xRotation = cmd.fNumbers[i + 2];
					largeArcFlag = cmd.fNumbers[i + 3];
					sweepFlag = cmd.fNumbers[i + 4];
					x = cmd.fNumbers[i + 5];
					y = cmd.fNumbers[i + 6];

					larc = largeArcFlag > 0.5f;
					swp = sweepFlag > 0.5f;
					rotation = maths::radians(xRotation);

					fWorkingPath.ellipticArcTo(BLPoint(rx, ry), rotation, larc, swp, BLPoint(x, y));
				}
            }
        }


		// SVG - a
        void arcBy(const PathSegment& cmd)
        {
            if (cmd.fNumbers.size() < 7) {
                printf("arcBy - Rejected: %zd\n", cmd.fNumbers.size());
                return;
            }
            
            float rx = cmd.fNumbers[0];
            float ry = cmd.fNumbers[1];
            float xRotation = cmd.fNumbers[2];
            float largeArcFlag = cmd.fNumbers[3];
            float sweepFlag = cmd.fNumbers[4];
            float x = lastPosition().x + cmd.fNumbers[5];
            float y = lastPosition().y + cmd.fNumbers[6];

            bool larc = largeArcFlag > 0.5f;
            bool swp = sweepFlag > 0.5f;
            float rotation = maths::radians(xRotation);

            fWorkingPath.ellipticArcTo(BLPoint(rx, ry), rotation, larc, swp, BLPoint(x, y));

			if (cmd.fNumbers.size() > 7)
			{
				//printf("EXTENDED arcBy\n");
				for (size_t i = 7; i < cmd.fNumbers.size(); i += 7)
				{
					rx = cmd.fNumbers[i];
					ry = cmd.fNumbers[i + 1];
					xRotation = cmd.fNumbers[i + 2];
					largeArcFlag = cmd.fNumbers[i + 3];
					sweepFlag = cmd.fNumbers[i + 4];
					x = lastPosition().x + cmd.fNumbers[i + 5];
					y = lastPosition().y + cmd.fNumbers[i + 6];

					larc = largeArcFlag > 0.5f;
					swp = sweepFlag > 0.5f;
					rotation = maths::radians(xRotation);

					fWorkingPath.ellipticArcTo(BLPoint(rx, ry), rotation, larc, swp, BLPoint(x, y));
				}
			}
        }

        // SVG - Z,z    close path
		void close(const PathSegment& cmd)
        {
            if (!fWorkingPath.empty())
            {
                fWorkingPath.close();
            }
            finishWorking();
        }

        // Turn a set of commands and numbers
        // into a blPath
        //
        void parseCommands(const std::vector<ndt::PathSegment>& segments)
        {
            for (auto& cmd : segments)
            {
                switch (cmd.fCommand)
                {
                case ndt::SegmentKind::MoveTo:
                    moveTo(cmd);
                    break;
                case ndt::SegmentKind::MoveBy:
                    moveBy(cmd);
                    break;

                case ndt::SegmentKind::LineTo:
                    lineTo(cmd);
                    break;
                case ndt::SegmentKind::LineBy:
                    lineBy(cmd);
                    break;

                case ndt::SegmentKind::HLineTo:
                    hLineTo(cmd);
                    break;
                case ndt::SegmentKind::HLineBy:
                    hLineBy(cmd);
                    break;

                case ndt::SegmentKind::VLineTo:
                    vLineTo(cmd);
                    break;
                case ndt::SegmentKind::VLineBy:
                    vLineBy(cmd);
                    break;

                case ndt::SegmentKind::CubicTo:
                    cubicTo(cmd);
                    break;
                case ndt::SegmentKind::CubicBy:
                    cubicBy(cmd);
                    break;

                case ndt::SegmentKind::SCubicTo:
                    smoothCubicTo(cmd);
                    break;
                case ndt::SegmentKind::SCubicBy:
                    smoothCubicBy(cmd);
                    break;
                    
                case ndt::SegmentKind::QuadTo:
                    quadTo(cmd);
                    break;
                case ndt::SegmentKind::QuadBy:
					quadBy(cmd);
                    break;

                case SegmentKind::SQuadTo:
                    smoothQuadTo(cmd);
                    break;
                case SegmentKind::SQuadBy:
                    smoothQuadBy(cmd);
                    break;
                    

                case ndt::SegmentKind::ArcTo:
                    arcTo(cmd);
                    break;
                case ndt::SegmentKind::ArcBy:
                    arcBy(cmd);
                    break;


                case ndt::SegmentKind::CloseTo:
                case ndt::SegmentKind::CloseBy:
                    close(cmd);
                    break;

                default:
                    // do nothing
                    break;
                }

                //lastPosition();
				fLastCommand = cmd.fCommand;
            }

            finishWorking();

            return;
        }


    };

    static bool blPathFromSegments(std::vector<ndt::PathSegment>& segments, BLPath& apath)
    {
        PathBuilder builder{};

        builder.parseCommands(segments);
        apath = builder.getPath();

        return true;
    }

    static bool blPathFromCommands(DataChunk& chunk, BLPath& apath)
    {
        std::vector<ndt::PathSegment> segments{};

        ndt::tokenizePath(chunk, segments);

        return blPathFromSegments(segments, apath);
    }
}

