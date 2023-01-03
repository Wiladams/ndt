#pragma once

#include "graphic.hpp"
#include "shaper.h"
#include "datachunk.h"

struct ShapeViewer : public GraphicElement
{
    BLPath fPath{};
    BLVar fStyle;

    ShapeViewer(const char * subject, size_t subjectLength, const char* nm)
        :GraphicElement({ 0,0,1920,1080 })
    {
        setName(nm);
        blVarAssignRgba32(&fStyle, BLRgba32(127, 127, 127, 255).value);
		ndt::DataChunk chunk = ndt::chunk_from_data_size( (void *)subject, subjectLength );
        ndt::blPathFromCommands(chunk, fPath);
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

    void setStyle(const BLVar& s)
    {
        fStyle.assign(s);
    }

    void draw(IGraphics& ctx) override
    {
        ctx.push();

        ctx.stroke(0);
        //ctx.fill(127);
        //ctx.noFill();
        ctx.fill(fStyle);
        ctx.path(fPath);

        ctx.pop();
    }

};
