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
    static void tokenizePath(std::string_view subject, std::vector<PathSegment>& commands)
    {
        ndt::charset whitespaceChars(",\t\n\f\r ");
        ndt::charset commandChars("mMlLhHvVcCqQsStTaAzZ");
        ndt::charset numberChars("0123456789.+-eE");

        // use binstream do do the parsing
        //BinStream bs(subject.data(), subject.size(), 0);
        DataCursor dc = ndt::make_cursor_size(subject.data(), subject.size());

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
                commands[commands.size() - 1].fNumbers.push_back(afloat);

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