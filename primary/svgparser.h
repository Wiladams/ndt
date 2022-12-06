#pragma once

#include "binstream.hpp"
#include "blend2d.h"
#include "maths.hpp"
#include "shaper.h"

#include "elements/textscan.h"

#include <charconv>
#include <string>
#include <vector>
#include <map>


namespace svg
{
    struct ColorEntry {
        //const char* name = nullptr;
        maths::vec4b value{};
    };

    // Database of colors
    std::map<const char*, maths::vec4b> colors =
    {
        {"white",  {255, 255, 255}},
        {"ivory", {255, 255, 240}},
        {"lightyellow", {255, 255, 224}},
        {"mintcream", {245, 255, 250}},
        {"azure", {240, 255, 255}},
        {"snow", {255, 250, 250}},
    {"honeydew", {240, 255, 240}},
    {"floralwhite", {255, 250, 240}},
    {"ghostwhite", {248, 248, 255}},
    {"lightcyan", {224, 255, 255}},
    {"lemonchiffon", {255, 250, 205}},
    {"cornsilk", {255, 248, 220}},
    {"lightgoldenrodyellow", {250, 250, 210}},
    {"aliceblue", {240, 248, 255}},
    {"seashell", {255, 245, 238}},
    {"oldlace", {253, 245, 230}},
    {"whitesmoke", {245, 245, 245}},
    {"lavenderblush", {255, 240, 245}},
    {"beige", {245, 245, 220}},
    {"linen", {250, 240, 230}},
    {"papayawhip", {255, 239, 213}},
    {"blanchedalmond", {255, 235, 205}},
    {"antiquewhite", {250, 235, 215}},
    {"yellow", {255, 255, 0}},
    {"mistyrose", {255, 228, 225}},
    {"lavender", {230, 230, 250}},
    {"bisque", {255, 228, 196}},
    {"moccasin", {255, 228, 181}},
    {"palegoldenrod", {238, 232, 170}},
    {"khaki", {240, 230, 140}},
    {"navajowhite", {255, 222, 173}},
    {"aquamarine", {127, 255, 212}},
    {"paleturquoise", {175, 238, 238}},
    {"wheat", {245, 222, 179}},
    {"peachpuff", {255, 218, 185}},
    {"palegreen", {152, 251, 152}},
    {"greenyellow", {173, 255, 47}},
    {"gainsboro", {220, 220, 220}},
    {"powderblue", {176, 224, 230}},
    {"lightgreen", {144, 238, 144}},
    {"lightgray", {211, 211, 211}},
    {"chartreuse", {127, 255, 0}},
    {"gold", {255, 215, 0}},
    {"lightblue", {173, 216, 230}},
    {"lawngreen", {124, 252, 0}},
    {"pink", {255, 192, 203}},
    {"aqua", {0, 255, 255}},
    {"cyan", {0, 255, 255}},
    {"lightpink", {255, 182, 193}},
    {"thistle", {216, 191, 216}},
    {"lightskyblue", {135, 206, 250}},
    {"lightsteelblue", {176, 196, 222}},
    {"skyblue", {135, 206, 235}},
    {"silver", {192, 192, 192}},
    {"springgreen", {0, 255, 127}},
    {"mediumspringgreen", {0, 250, 154}},
    {"turquoise", {64, 224, 208}},
    {"burlywood", {222, 184, 135}},
    {"tan", {210, 180, 140}},
    {"yellowgreen", {154, 205, 50}},
    {"lime", {0, 255, 0}},
    {"mediumaquamarine", {102, 205, 170}},
    {"mediumturquoise", {72, 209, 204}},
    {"darkkhaki", {189, 183, 107}},
    {"lightsalmon", {255, 160, 122}},
    {"plum", {221, 160, 221}},
    {"sandybrown", {244, 164, 96}},
    {"darkseagreen", {143, 188, 143}},
    {"orange", {255, 165, 0}},
    {"darkgray", {169, 169, 169}},
    {"goldenrod", {218, 165, 32}},
    {"darksalmon", {233, 150, 122}},
    {"darkturquoise", {0, 206, 209}},
    {"limegreen", {50, 205, 50}},
    {"violet", {238, 130, 238}},
    {"deepskyblue", {0, 191, 255}},
    {"darkorange", {255, 140, 0}},
    {"salmon", {250, 128, 114}},
    {"rosybrown", {188, 143, 143}},
    {"lightcoral", {240, 128, 128}},
    {"coral", {255, 127, 80}},
    {"mediumseagreen", {60, 179, 113}},
    {"lightseagreen", {32, 178, 170}},
    {"cornflowerblue", {100, 149, 237}},
    {"cadetblue", {95, 158, 160}},
    {"peru", {205, 133, 63}},
    {"hotpink", {255, 105, 180}},
    {"orchid", {218, 112, 214}},
    {"palevioletred", {219, 112, 147}},
    {"darkgoldenrod", {184, 134, 11}},
    {"lightslategray", {119, 136, 153}},
    {"tomato", {255, 99, 71}},
    {"gray", {128, 128, 128}},
    {"dodgerblue", {30, 144, 255}},
    {"mediumpurple", {147, 112, 219}},
    {"olivedrab", {107, 142, 35}},
    {"slategray", {112, 128, 144}},
    {"chocolate", {210, 105, 30}},
    {"steelblue", {70, 130, 180}},
    {"olive", {128, 128, 0}},
    {"mediumslateblue", {123, 104, 238}},
    {"indianred", {205, 92, 92}},
    {"mediumorchid", {186, 85, 211}},
    {"seagreen", {46, 139, 87}},
    {"darkcyan", {0, 139, 139}},
    {"forestgreen", {34, 139, 34}},
    {"royalblue", {65, 105, 225}},
    {"dimgray", {105, 105, 105}},
    {"orangered", {255, 69, 0}},
    {"slateblue", {106, 90, 205}},
    {"teal", {0, 128, 128}},
    {"darkolivegreen", {85, 107, 47}},
    {"sienna", {160, 82, 45}},
    {"green", {0, 128, 0}},
    {"darkorchid", {153, 50, 204}},
    {"saddlebrown", {139, 69, 19}},
    {"deeppink", {255, 20, 147}},
    {"blueviolet", {138, 43, 226}},
    {"magenta", {255, 0, 255}},
    {"fuchsia", {255, 0, 255}},
    {"darkslategray", {47, 79, 79}},
    {"darkgreen", {0, 100, 0}},
    {"darkslateblue", {72, 61, 139}},
    {"brown", {165, 42, 42}},
    {"mediumvioletred", {199, 21, 133}},
    {"crimson", {220, 20, 60}},
    {"firebrick", {178, 34, 34}},
    {"red", {255, 0, 0}},
    {"darkviolet", {148, 0, 211}},
    {"darkmagenta", {139, 0, 139}},
    {"purple", {128, 0, 128}},
    {"midnightblue", {25, 25, 112}},
    {"darkred", {139, 0, 0}},
    {"maroon", {128, 0, 0}},
    {"indigo", {75, 0, 130}},
    {"blue", {0, 0, 255}},
    {"mediumblue", {0, 0, 205}},
    {"darkblue", {0, 0, 139}},
    {"navy", {0, 0, 128}},
    {"black", {0, 0, 0}},

    };
/*
    ColorEntry svgcolors[] = {
    {"white", {255, 255, 255}},
    {"ivory", {255, 255, 240}},
    {"lightyellow", {255, 255, 224}},
    {"mintcream", {245, 255, 250}},
    {"azure", {240, 255, 255}},
    {"snow", {255, 250, 250}},
    {"honeydew", {240, 255, 240}},
    {"floralwhite", {255, 250, 240}},
    {"ghostwhite", {248, 248, 255}},
    {"lightcyan", {224, 255, 255}},
    {"lemonchiffon", {255, 250, 205}},
    {"cornsilk", {255, 248, 220}},
    {"lightgoldenrodyellow", {250, 250, 210}},
    {"aliceblue", {240, 248, 255}},
    {"seashell", {255, 245, 238}},
    {"oldlace", {253, 245, 230}},
    {"whitesmoke", {245, 245, 245}},
    {"lavenderblush", {255, 240, 245}},
    {"beige", {245, 245, 220}},
    {"linen", {250, 240, 230}},
    {"papayawhip", {255, 239, 213}},
    {"blanchedalmond", {255, 235, 205}},
    {"antiquewhite", {250, 235, 215}},
    {"yellow", {255, 255, 0}},
    {"mistyrose", {255, 228, 225}},
    {"lavender", {230, 230, 250}},
    {"bisque", {255, 228, 196}},
    {"moccasin", {255, 228, 181}},
    {"palegoldenrod", {238, 232, 170}},
    {"khaki", {240, 230, 140}},
    {"navajowhite", {255, 222, 173}},
    {"aquamarine", {127, 255, 212}},
    {"paleturquoise", {175, 238, 238}},
    {"wheat", {245, 222, 179}},
    {"peachpuff", {255, 218, 185}},
    {"palegreen", {152, 251, 152}},
    {"greenyellow", {173, 255, 47}},
    {"gainsboro", {220, 220, 220}},
    {"powderblue", {176, 224, 230}},
    {"lightgreen", {144, 238, 144}},
    {"lightgray", {211, 211, 211}},
    {"chartreuse", {127, 255, 0}},
    {"gold", {255, 215, 0}},
    {"lightblue", {173, 216, 230}},
    {"lawngreen", {124, 252, 0}},
    {"pink", {255, 192, 203}},
    {"aqua", {0, 255, 255}},
    {"cyan", {0, 255, 255}},
    {"lightpink", {255, 182, 193}},
    {"thistle", {216, 191, 216}},
    {"lightskyblue", {135, 206, 250}},
    {"lightsteelblue", {176, 196, 222}},
    {"skyblue", {135, 206, 235}},
    {"silver", {192, 192, 192}},
    {"springgreen", {0, 255, 127}},
    {"mediumspringgreen", {0, 250, 154}},
    {"turquoise", {64, 224, 208}},
    {"burlywood", {222, 184, 135}},
    {"tan", {210, 180, 140}},
    {"yellowgreen", {154, 205, 50}},
    {"lime", {0, 255, 0}},
    {"mediumaquamarine", {102, 205, 170}},
    {"mediumturquoise", {72, 209, 204}},
    {"darkkhaki", {189, 183, 107}},
    {"lightsalmon", {255, 160, 122}},
    {"plum", {221, 160, 221}},
    {"sandybrown", {244, 164, 96}},
    {"darkseagreen", {143, 188, 143}},
    {"orange", {255, 165, 0}},
    {"darkgray", {169, 169, 169}},
    {"goldenrod", {218, 165, 32}},
    {"darksalmon", {233, 150, 122}},
    {"darkturquoise", {0, 206, 209}},
    {"limegreen", {50, 205, 50}},
    {"violet", {238, 130, 238}},
    {"deepskyblue", {0, 191, 255}},
    {"darkorange", {255, 140, 0}},
    {"salmon", {250, 128, 114}},
    {"rosybrown", {188, 143, 143}},
    {"lightcoral", {240, 128, 128}},
    {"coral", {255, 127, 80}},
    {"mediumseagreen", {60, 179, 113}},
    {"lightseagreen", {32, 178, 170}},
    {"cornflowerblue", {100, 149, 237}},
    {"cadetblue", {95, 158, 160}},
    {"peru", {205, 133, 63}},
    {"hotpink", {255, 105, 180}},
    {"orchid", {218, 112, 214}},
    {"palevioletred", {219, 112, 147}},
    {"darkgoldenrod", {184, 134, 11}},
    {"lightslategray", {119, 136, 153}},
    {"tomato", {255, 99, 71}},
    {"gray", {128, 128, 128}},
    {"dodgerblue", {30, 144, 255}},
    {"mediumpurple", {147, 112, 219}},
    {"olivedrab", {107, 142, 35}},
    {"slategray", {112, 128, 144}},
    {"chocolate", {210, 105, 30}},
    {"steelblue", {70, 130, 180}},
    {"olive", {128, 128, 0}},
    {"mediumslateblue", {123, 104, 238}},
    {"indianred", {205, 92, 92}},
    {"mediumorchid", {186, 85, 211}},
    {"seagreen", {46, 139, 87}},
    {"darkcyan", {0, 139, 139}},
    {"forestgreen", {34, 139, 34}},
    {"royalblue", {65, 105, 225}},
    {"dimgray", {105, 105, 105}},
    {"orangered", {255, 69, 0}},
    {"slateblue", {106, 90, 205}},
    {"teal", {0, 128, 128}},
    {"darkolivegreen", {85, 107, 47}},
    {"sienna", {160, 82, 45}},
    {"green", {0, 128, 0}},
    {"darkorchid", {153, 50, 204}},
    {"saddlebrown", {139, 69, 19}},
    {"deeppink", {255, 20, 147}},
    {"blueviolet", {138, 43, 226}},
    {"magenta", {255, 0, 255}},
    {"fuchsia", {255, 0, 255}},
    {"darkslategray", {47, 79, 79}},
    {"darkgreen", {0, 100, 0}},
    {"darkslateblue", {72, 61, 139}},
    {"brown", {165, 42, 42}},
    {"mediumvioletred", {199, 21, 133}},
    {"crimson", {220, 20, 60}},
    {"firebrick", {178, 34, 34}},
    {"red", {255, 0, 0}},
    {"darkviolet", {148, 0, 211}},
    {"darkmagenta", {139, 0, 139}},
    {"purple", {128, 0, 128}},
    {"midnightblue", {25, 25, 112}},
    {"darkred", {139, 0, 0}},
    {"maroon", {128, 0, 0}},
    {"indigo", {75, 0, 130}},
    {"blue", {0, 0, 255}},
    {"mediumblue", {0, 0, 205}},
    {"darkblue", {0, 0, 139}},
    {"navy", {0, 0, 128}},
    {"black", {0, 0, 0}},
    };
*/
}

namespace svg 
{
 

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
        void moveTo(ndt::PathSegment &cmd)
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
        SVGPathParser builder{};

        builder.parseCommands(segments);
        apath = builder.getPath();

        return true;
    }

    static bool blPathFromCommands(const std::string& subject, BLPath& apath)
    {
        std::vector<ndt::PathSegment> segments{};

        ndt::tokenizePath(subject, segments);

        return blPathFromSegments(segments, apath);
    }
}