#pragma once

#include "binstream.hpp"
#include "blend2d.h"
#include "maths.hpp"

#include "elements/textscan.h"

#include <charconv>
#include <string>
#include <vector>

namespace svg
{
    struct SVGPathCommand
    {
        uint8_t fCommand{ 0 };
        std::vector<float> fNumbers{};

        SVGPathCommand() { ; }
        SVGPathCommand(const SVGPathCommand& other)
            :fCommand(other.fCommand)
            , fNumbers(other.fNumbers)
        {}
    };

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
    void tokenizeSVGPath(std::string subject, std::vector<SVGPathCommand>& commands)
    {
        ndt::charset whitespaceChars(",\t\n\f\r ");
        ndt::charset commandChars("aAcChHlLmMqQsStTvVzZ");
        ndt::charset numberChars("0123456789.+-");

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
                    printf("CMD: %c\n", c);
                    SVGPathCommand cmd;
                    cmd.fCommand = c;
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

                numbuff[numoffset] == 0;
                float afloat = 0;
                std::from_chars((const char*)numbuff, ((const char*)numbuff) + numoffset, afloat);

                printf("  %f\n", afloat);
                commands[commands.size() - 1].fNumbers.push_back(afloat);
                //numoffset = 0;
                memset(numbuff, 0, sizeof(numbuff));
            }
        }
    }


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
    // SVG Path Commands
    // M - moveTo       (M, m)
    // L - lineTo       (L, l, H, h, V, v)
    // C - cubicTo      (C, c, S, s)
    // Q - quadTo       (Q, q, T, t)
    // A - ellipticArc  (A, a)
    // Z - close        (Z, z)

        void parseCommands(const std::vector<SVGPathCommand>& commands)
        {
            for (auto& cmd : commands)
            {
                switch (cmd.fCommand)
                {
                case 'M':
                    moveTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;
                case 'm':
                    moveBy(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;

                case 'L':
                    lineTo(cmd.fNumbers[0], cmd.fNumbers[1]);
                    break;
                case 'l':
                    lineBy(cmd.fNumbers[0], cmd.fNumbers[2]);
                    break;

                case 'H':
                    hLineTo(cmd.fNumbers[0]);
                    break;
                case 'h':
                    hLineBy(cmd.fNumbers[0]);
                    break;

                case 'V':
                    vLineTo(cmd.fNumbers[0]);
                    break;
                case 'v':
                    vLineBy(cmd.fNumbers[0]);
                    break;

                case 'C':
                    cubicTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5]);
                    break;

                case 'Q':
                    quadTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3]);
                    break;

                // Elliptic arc
                case 'A':
                    arcTo(cmd.fNumbers[0], cmd.fNumbers[1], cmd.fNumbers[2], cmd.fNumbers[3], cmd.fNumbers[4], cmd.fNumbers[5], cmd.fNumbers[6]);
                    break;

                case 'Z':
                case 'z':
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
            std::vector<SVGPathCommand> commands{};

            tokenizeSVGPath(subject, commands);

            SVGPathParser builder{};
            std::vector<BLPath> figures{};

            builder.parseCommands(commands);
            apath = builder.getPath();

            return true;
        }
    };

}