#pragma once

#include "graphic.hpp"
#include "svgparser.h"

struct ShapeViewer : public GraphicElement
{
    BLPath fPath{};

    ShapeViewer(const std::string& subject, const char* nm)
        :GraphicElement({ 0,0,1920,1080 })
    {
        setName(nm);

        svg::blPathFromCommands(subject, fPath);
    }

    ShapeViewer(const std::string& subject, std::string& name)
        : GraphicElement({ 0,0,1920,1080 })
    {
        setName(name);

        svg::blPathFromCommands(subject, fPath);
    }

    // x,y are given in the coordinate space
    // of our frame.
    bool contains(float x, float y) override
    {
        auto result = fPath.hitTest({ x,y }, BL_FILL_RULE_EVEN_ODD) == BL_HIT_TEST_IN;

        //if (result)
        //    printf("SVGPathView::contains: %3.0f,%3.0f  [%s]\n", x, y , fName.c_str() );

        return result;
    }

    void mouseEvent(const MouseEvent& e) override
    {
        //printf("ShapeViewer::mouseEvent: %3.0f, %3.0f  [%s]\n", e.x, e.y, name().c_str());
    }

    void draw(IGraphics& ctx) override
    {
        ctx.push();

        ctx.stroke(0);
        ctx.fill(127);
        //ctx.noFill();
        ctx.path(fPath);

        ctx.pop();
    }

};
