#pragma once

#include "binstream.hpp"
#include "elements/textscan.h"

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
    // Shaper contour Commands
    // Origin from SVG path commands
    // M - moveTo       (M, m)
    // L - lineTo       (L, l, H, h, V, v)
    // C - cubicTo      (C, c, S, s)
    // Q - quadTo       (Q, q, T, t)
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
        , QuadTo = 'Q'
        , QuadBy = 'q'
        , ArcTo = 'A'
        , ArcBy = 'a'
        , CloseTo = 'Z'
        , CloseBy = 'z'
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
    static void tokenizePath(DataChunk &chunk, std::vector<PathSegment>& commands)
    {
        ndt::charset whitespaceChars(",\t\n\f\r ");
        ndt::charset commandChars("mMlLhHvVcCqQsStTaAzZ");
        ndt::charset numberChars("0123456789.+-eE");

        // use binstream do do the parsing
        //BinStream bs(subject.data(), subject.size(), 0);
        DataCursor dc = ndt::make_cursor_chunk(chunk);

        // create a little buffer to be used for numbers
        int numoffset = 0;
        char numbuff[64]{ 0 };

        while (!ndt::isEOF(dc))
        {

            auto c = ndt::get_u8(dc);

            // ignore whitespace
            while (whitespaceChars[c] && !isEOF(dc))
                c = get_u8(dc);

            // See what we've got at this point
            // it's either in commandChars, if we in the START state
            if (commandChars[c])
            {
                switch (c) {
                default:
                    //printf("CMD: %c\n", c);
                    ndt::PathSegment cmd{};
                    cmd.fCommand = ndt::SegmentKind(c);
                    commands.push_back(cmd);
                }

                continue;
            }

            // or it's something related to a number (+,-,digit,.)
            if (numberChars[c])
            {
                numoffset = 0;
                while (numberChars[c]) {
                    numbuff[numoffset] = c;
                    c = get_u8(dc);
                    numoffset++;
                }



                numbuff[numoffset] == 0;
                float afloat = 0;
                std::from_chars((const char*)numbuff, ((const char*)numbuff) + numoffset, afloat);

                //printf("  %f\n", afloat);
                //commands[commands.size() - 1].fNumbers.push_back(afloat);
				commands[commands.size() - 1].addNumber(afloat);
                
                if (isEOF(dc))
                    break;

                // backup one, because whatever we read was not
                // a number char, so we'll let the top of the loop
                // deal with it
                skip(dc, -1);
            }
        }
    }




}

namespace ndt
{
    //
    // A path should have commands and vertices
    //
    struct PathBuilder
    {
    public:
        maths::vec2f fLastPosition{};
        BLPath fPath{};
        BLPath fWorkingPath{};


    public:
        BLPath& getPath() { return fPath; }

        // Add a moveTo command, and two numbers
        // SVG - M
        void moveTo(ndt::PathSegment& cmd)
        {
            if (!fWorkingPath.empty())
            {
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }

            fLastPosition = maths::vec2f{ cmd.fNumbers[0], cmd.fNumbers[1] };
            fWorkingPath.moveTo(cmd.fNumbers[0], cmd.fNumbers[1]);
        }

        void moveTo(const float x, const float y)
        {
            if (!fWorkingPath.empty())
            {
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }

            fLastPosition = maths::vec2f{ x, y };
            fWorkingPath.moveTo(x, y);
        }

        // SVG - m
        void moveBy(const float dx, const float dy)
        {
            moveTo(fLastPosition.x + dx, fLastPosition.y + dy);
        }


        // Add a line, using current point as first endpoint
        // SVG - H
        void lineTo(const float x, const float y)
        {
            fWorkingPath.lineTo(x, y);
            fLastPosition = { x, y };
        }

        // Add a line using relative coordinates
        //SVG - l
        void lineBy(const float dx, const float dy)
        {
            lineTo(fLastPosition.x + dx, fLastPosition.y + dy);
        }

        // Add horizontal line, using only x coordinate added to existing point
        // SVG - H
        void hLineTo(const float x)
        {
            fWorkingPath.lineTo(x, fLastPosition.y);
            fLastPosition = { x, fLastPosition.y };
        }

        // SVG - h
        void hLineBy(const float dx)
        {
            hLineTo(fLastPosition.x + dx);
        }

        // SVG - V
        void vLineTo(const float y)
        {
            fWorkingPath.lineTo(fLastPosition.x, y);
            fLastPosition = { fLastPosition.x, y };
        }

        // SVG - v
        void vLineBy(const float dy)
        {
            vLineTo(fLastPosition.y + dy);
        }

        // SVG - Q
        // Quadratic Bezier curve
        void quadTo(const float c1x, const float c1y, const float p2x, const float p2y)
        {
            fWorkingPath.quadTo(BLPoint(c1x, c1y), BLPoint(p2x, p2y));
            fLastPosition = { p2x, p2y };
        }

        //void quadTo(const BLPoint& c1, const BLPoint& p2)
        //{
        //    quadTo(c1.x, c1.y, p2.x, p2.y);
        //}

        // SVG - C
        void cubicTo(const float c1x, const float c1y,
            const float c2x, const float c2y,
            const float p2x, const float p2y)
        {
            fWorkingPath.cubicTo(BLPoint(c1x, c1y), BLPoint(c2x, c2y), BLPoint(p2x, p2y));
            fLastPosition = { p2x, p2y };
        }

        //void cubicBy(const float dc1x, const float dc1y,
        //    const float dc2x, const float dc2y,
        //    const float dp2x, const float dp2y)
        //{}

        void arcTo(float rx, float ry, float rotx, float largeArc, float sweep, float x, float y)
        {
            bool larc = largeArc != 0;
            bool swp = sweep != 0;
            float rotation = maths::radians(rotx);

            fWorkingPath.ellipticArcTo(BLPoint(rx, ry), rotation, larc, swp, BLPoint(x, y));
            fLastPosition = { x,y };
        }

        // SVG - S, smooth cubicTo
        // SVG - s, smooth cubicBy

        // SVG - Z,z    close path
        void close()
        {
            if (!fWorkingPath.empty())
            {
                fWorkingPath.close();
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }
        }

        // The case where the path did not end
        // with a 'Z', but we're done parsing
        void finish()
        {
            if (!fWorkingPath.empty())
            {
                fPath.addPath(fWorkingPath);

                fWorkingPath.reset();
            }
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
                    moveTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    //moveTo(cmd);
                    break;
                case ndt::SegmentKind::MoveBy:
                    moveBy(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;

                case ndt::SegmentKind::LineTo:
                    lineTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;
                case ndt::SegmentKind::LineBy:
                    lineBy(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;

                case ndt::SegmentKind::HLineTo:
                    hLineTo(cmd.fNumbers[0]);
                    break;
                case ndt::SegmentKind::HLineBy:
                    hLineBy(cmd.fNumbers[0]);
                    break;

                case ndt::SegmentKind::VLineTo:
                    vLineTo(cmd.fNumbers[0]);
                    break;
                case ndt::SegmentKind::VLineBy:
                    vLineBy(cmd.fNumbers[0]);
                    break;

                case ndt::SegmentKind::CubicTo:
                    cubicTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5]);
                    break;
                case ndt::SegmentKind::CubicBy:
                    break;

                case ndt::SegmentKind::QuadTo:
                    quadTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3]);
                    break;
                case ndt::SegmentKind::QuadBy:
                    break;

                    // Elliptic arc
                case ndt::SegmentKind::ArcTo:
                    arcTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5], cmd.fNumbers[6]);
                    break;
                case ndt::SegmentKind::ArcBy:
                    break;


                case ndt::SegmentKind::CloseTo:
                case ndt::SegmentKind::CloseBy:
                    close();
                    break;

                default:
                    // do nothing
                    break;
                }

            }

            finish();

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

