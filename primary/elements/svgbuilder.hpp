#pragma once


#include "blend2d.h"
#include "elements/textscan.h"

#include <vector>

//
// SVGPathBuilder
// 
// This builder creates polygons using standard vector
// commands (moveTo, lineTo, cubicTo, etc)
// Working in conjuction with some context, you can build
// figures, with stroke and fill commands
//
namespace svg
{
    //
     // A path should have commands and vertices
     //
    struct SVGPathBuilder
    {
    public:
        //
        // SVG Path Commands
        // M - moveTo       (M, m)
        // L - lineTo       (L, l, H, h, V, v)
        // C - cubicTo      (C, c, S, s)
        // Q - quadTo       (Q, q, T, t)
        // A - ellipticArc  (A, a)
        // Z - close        (Z, z)

        enum class ContourCommand
        {
            MoveTo = 0,

            LineTo,
            HLineTo,
            VLineTo,

            CubicBezierTo,
            QuadraticBezierTo,

            ArcCircleTo,
            ArcEllipseTo,

            Line,

            Close = 255,
        };

        std::vector<ContourCommand> fCommands;
        std::vector<double> fNumbers;  // vertices
        BLPoint fLastPosition;
        BLPath fPath;
        BLPath fWorkingPath;

    public:
        void addNumber(const double& value)
        {
            fNumbers.push_back(value);
        }

        void addPoint(const BLPoint& pt)
        {
            addNumber(pt.x);
            addNumber(pt.y);
        }

        void addCommand(const ContourCommand cmd)
        {
            fCommands.push_back(cmd);
        }

    public:

        SVGPathBuilder()
            :fLastPosition(0, 0)
        {}

        BLPath& getPath() { return fPath; }


        // retrieve a number from the vector of numbers
        // and increment the index
        inline double getNumber(size_t& idx) const { idx++; return fNumbers[idx - 1]; }

        // Retrieve two numbers, and increment index
        inline BLPoint getPoint(size_t& idx) const
        {
            double x = getNumber(idx);
            double y = getNumber(idx);

            return BLPoint(x, y);
        }

        // Add a moveTo command, and two numbers
        void moveTo(const double& x, const double& y)
        {
            addCommand(ContourCommand::MoveTo);
            addNumber(x);
            addNumber(y);

            if (!fWorkingPath.empty())
            {
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }

            fLastPosition = BLPoint(x, y);
            fWorkingPath.moveTo(x, y);
        }

        void moveTo(const BLPoint& pt)
        {
            moveTo(pt.x, pt.y);
        }

        void moveBy(const double& dx, const double& dy)
        {
            moveTo(fLastPosition.x + dx, fLastPosition.y + dy);
        }


        // Add a line, using current point as first endpoint
        // SVG - H
        void lineTo(const double& x, const double& y)
        {
            addCommand(ContourCommand::LineTo);
            addNumber(x);
            addNumber(y);

            fWorkingPath.lineTo(x, y);
            fLastPosition = BLPoint(x, y);
        }

        void lineTo(const BLPoint& pt)
        {
            lineTo(pt.x, pt.y);
        }

        // Add a line using relative coordinates
        //SVG - l
        void lineBy(const double& dx, const double& dy)
        {
            lineTo(fLastPosition.x + dx, fLastPosition.y + dy);
        }

        // Add horizontal line, using only x coordinate added to existing point
        // SVG - H
        void hLineTo(const double& x)
        {
            addCommand(ContourCommand::HLineTo);
            addNumber(x);

            fWorkingPath.lineTo(x, fLastPosition.y);
            fLastPosition = BLPoint(x, fLastPosition.y);
        }

        // SVG - h
        void hLineBy(const double& dx)
        {
            hLineTo(fLastPosition.x + dx);
        }

        // SVG - V
        void vLineTo(const double& y)
        {
            addCommand(ContourCommand::VLineTo);
            addNumber(y);

            fWorkingPath.lineTo(fLastPosition.x, y);
            fLastPosition = BLPoint(fLastPosition.x, y);
        }

        // SVG - v
        void vLineBy(const double& dy)
        {
            vLineTo(fLastPosition.y + dy);
        }

        // SVG - Q
        // Quadratic Bezier curve
        void quadTo(const double& c1x, const double& c1y, const double& p2x, const double& p2y)
        {
            addCommand(ContourCommand::QuadraticBezierTo);
            addNumber(c1x);
            addNumber(c1y);
            addNumber(p2x);
            addNumber(p2y);

            fWorkingPath.quadTo(BLPoint(c1x, c1y), BLPoint(p2x, p2y));
            fLastPosition = BLPoint(p2x, p2y);
        }

        void quadTo(const BLPoint& c1, const BLPoint& p2)
        {
            quadTo(c1.x, c1.y, p2.x, p2.y);
        }

        // SVG - C
        void cubicTo(const double& c1x, const double& c1y,
            const double& c2x, const double& c2y,
            const double& p2x, const double& p2y)
        {
            addCommand(ContourCommand::CubicBezierTo);
            addNumber(c1x);
            addNumber(c1y);
            addNumber(c2x);
            addNumber(c2y);
            addNumber(p2x);
            addNumber(p2y);

            fWorkingPath.cubicTo(BLPoint(c1x, c1y), BLPoint(c2x, c2y), BLPoint(p2x, p2y));
            fLastPosition = BLPoint(p2x, p2y);
        }


        void cubicTo(const BLPoint& c1, const BLPoint& c2, const BLPoint& p2)
        {
            cubicTo(c1.x, c1.y, c2.x, c2.y, p2.x, p2.y);
        }

        //void cubicBy(const T& dc1x, const T& dc1y,
        //    const T& dc2x, const T& dc2y,
        //    const T& dp2x, const T& dp2y)
        //{}
        // SVG - S, smooth cubicTo
        // SVG - s, smooth cubicBy

        // SVG - Z,z    close path
        void close()
        {
            addCommand(ContourCommand::Close);

            if (!fWorkingPath.empty())
            {
                fWorkingPath.close();
                fPath.addPath(fWorkingPath);
                fWorkingPath.reset();
            }
        }



        // Turn our current list of points and commands
        // into a vector of BLPath objects
        //
        // This may not be necessary, can be built
        // directly at construction time
        inline void getPaths(std::vector<BLPath> &figures)
        {
            typedef BLPoint Point;

            size_t ptidx = 0;
            Point lastPos;

            BLPath fig;     // working path

            for (size_t i = 0; i < fCommands.size(); i++)
            {
                switch (fCommands[i])
                {
                case ContourCommand::MoveTo:
                {
                    // If current figure not empty
                    // add it to the list of figures
                    if (!fig.empty())
                    {
                        figures.push_back(fig);
                    }

                    // start a new polygon
                    fig.clear();

                    lastPos = getPoint(ptidx);
                    fig.moveTo(lastPos);
                }
                break;

                case ContourCommand::Line:
                {
                    // finish previous figure if any
                    if (!fig.empty()) {
                        figures.push_back(fig);
                        fig.clear();
                    }

                    auto p1 = getPoint(ptidx);
                    auto p2 = getPoint(ptidx);

                    fig.addLine(BLLine(p1.x, p1.y, p2.x, p2.y));

                    figures.push_back(fig);
                    fig.clear();
                }
                break;

                case ContourCommand::LineTo:
                {
                    lastPos = getPoint(ptidx);
                    fig.lineTo(lastPos);
                }
                break;

                case ContourCommand::HLineTo:
                {
                    auto x = getNumber(ptidx);
                    lastPos.x = x;
                    fig.lineTo(lastPos);
                }
                break;

                case ContourCommand::VLineTo:
                {
                    auto y = getNumber(ptidx);
                    lastPos.y = y;
                    fig.lineTo(lastPos);
                }
                break;

                case ContourCommand::QuadraticBezierTo:
                {
                    // lastPos
                    auto c1 = getPoint(ptidx);
                    auto p2 = getPoint(ptidx);

                    fig.quadTo(c1, p2);

                    lastPos = p2;
                }
                break;

                case ContourCommand::CubicBezierTo:
                {
                    // lastPos
                    auto c1 = getPoint(ptidx);
                    auto c2 = getPoint(ptidx);
                    auto p2 = getPoint(ptidx);

                    fig.cubicTo(c1, c2, p2);

                    lastPos = p2;
                }
                break;

                case ContourCommand::Close:
                    fig.close();

                    // add to list of figures
                    figures.push_back(fig);
                    fig.clear();

                    break;

                default:
                    // do nothing
                    break;
                }

            }

            return ;
        }
    };

}
