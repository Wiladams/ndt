#pragma once

#include "binstream.hpp"
#include "blend2d.h"
#include "maths.hpp"
#include "shaper.h"

#include "elements/textscan.h"

#include <charconv>
#include <string>
#include <vector>


namespace svg
{
    /*
    struct SVGPathCommand
    {
        //SegmentKind fCommand{SegmentKind::INVALID};
        uint8_t fCommand{ 0};

        std::vector<float> fNumbers{};

        SVGPathCommand() { ; }
        SVGPathCommand(const SVGPathCommand& other)
            :fCommand(other.fCommand)
            , fNumbers(other.fNumbers)
        {}
    };
    */
    //
// Take the 'd' attribute of a SVG path command and turn it into 
// a collection of commands and their associated numbers.
// This is not very robust, and will fall apart based on simple errors
// but it will work for common conformant cases.
// 
// Much smaller and faster than using regular expressions.
// 
// The most challenging part is parsing float numbers.  We rely on
// sscanf to do that work.
// 
// Here we're just tokenizing the string, not creating any geometry.
// This token set can be consumed by something else to create path objects
//
    /*
    void tokenizeSVGPath(std::string subject, std::vector<ndt::Contour>& commands)
    {
        ndt::charset whitespaceChars(",\t\n\f\r ");
        ndt::charset commandChars("mMlLhHvVcCqQsStTaAzZ");
        ndt::charset numberChars("0123456789.+-eE");

        // use binstream do do the parsing
        BinStream bs(subject.data(), subject.size(), 0);

        // create a little buffer to be used for numbers
        int numoffset = 0;
        char numbuff[64]{ 0 };

        while (!bs.isEOF())
        {
            // ignore whitespace
            auto c = bs.readBYTE();

            while (whitespaceChars[c] && !bs.isEOF())
                c = bs.readBYTE();

            // See what we've got at this point
            // it's either in commandChars, if we in the START state
            if (commandChars[c])
            {
                switch (c) {
                default:
                    //printf("CMD: %c\n", c);
                    ndt::Contour cmd;
                    cmd.fCommand = ndt::ContourKind(c);
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
                    c = bs.readBYTE();
                    numoffset++;
                }

                // backup one, because whatever we read was not
                // a number char, so we'll let the top of the loop
                // deal with it
                bs.skip(-1);

                numbuff[numoffset] == 0;
                float afloat = 0;
                std::from_chars((const char*)numbuff, ((const char*)numbuff) + numoffset, afloat);

                //printf("  %f\n", afloat);
                commands[commands.size() - 1].fNumbers.push_back(afloat);

                //memset(numbuff, 0, sizeof(numbuff));
            }
        }
    }
    */

    //
// A path should have commands and vertices
//
    struct SVGPathParser
    {
    public:
        maths::vec2f fLastPosition{};
        BLPath fPath{};
        BLPath fWorkingPath{};


    public:
        BLPath& getPath() { return fPath; }

        // Add a moveTo command, and two numbers
        // SVG - M
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


        void parseCommands(const std::vector<ndt::Contour>& commands)
        {
            for (auto& cmd : commands)
            {
                switch (cmd.fCommand)
                {
                case ndt::ContourKind::MoveTo:
                    moveTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;
                case ndt::ContourKind::MoveBy:
                    moveBy(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;

                case ndt::ContourKind::LineTo:
                    lineTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;
                case ndt::ContourKind::LineBy:
                    lineBy(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;

                case ndt::ContourKind::HLineTo:
                    hLineTo(cmd.fNumbers[0]);
                    break;
                case ndt::ContourKind::HLineBy:
                    hLineBy(cmd.fNumbers[0]);
                    break;

                case ndt::ContourKind::VLineTo:
                    vLineTo(cmd.fNumbers[0]);
                    break;
                case ndt::ContourKind::VLineBy:
                    vLineBy(cmd.fNumbers[0]);
                    break;

                case ndt::ContourKind::CubicTo:
                    cubicTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5]);
                    break;
                case ndt::ContourKind::CubicBy:
                    break;

                case ndt::ContourKind::QuadTo:
                    quadTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3]);
                    break;
                case ndt::ContourKind::QuadBy:
                    break;

                // Elliptic arc
                case ndt::ContourKind::ArcTo:
                    arcTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5], cmd.fNumbers[6]);
                    break;
                case ndt::ContourKind::ArcBy:
                    break;


                case ndt::ContourKind::CloseTo:
                case ndt::ContourKind::CloseBy:
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

        static bool createPathFromCommands(const std::string& subject, BLPath& apath)
        {
            std::vector<ndt::Contour> commands{};

            ndt::tokenizeContour(subject, commands);

            SVGPathParser builder{};
            std::vector<BLPath> figures{};

            builder.parseCommands(commands);
            apath = builder.getPath();

            return true;
        }
    };

}