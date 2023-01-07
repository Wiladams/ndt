#pragma once

#include "graphic.hpp"
#include "svgcolors.h"



// A simple rectangle that represents an SVG Color value
// this graphic shows the color, and puts the name on it.
struct SVGColorGraphic : public GraphicElement
{
    maths::vec4b fColor{};
    float fLuma{};

    SVGColorGraphic(const maths::rectf& fr, const std::string& name, const maths::vec4b& c)
        : GraphicElement(fr)
    {
        fColor = c;
        fColor.a = 255;
        fLuma = (0.2125 * fColor.r) + (0.7154 * fColor.g) + (0.0721 * fColor.b);

        setName(name);
    }


    bool operator==(const SVGColorGraphic& rhs) { return name() == rhs.name(); }


    void drawSelf(IGraphics& ctx)
    {
        ctx.noStroke();
        ctx.fill(fColor.r, fColor.g, fColor.b, fColor.a);
        ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

        // draw the text value
        // set the filling based on the luminance
        if (fLuma > 120) {
            ctx.fill(0);
        }
        else {
            ctx.fill(255);
        }

        // use center alignment
        float cx = boundsX() + (boundsWidth() / 2);
        float cy = boundsY() + boundsHeight() / 2;
        ctx.noStroke();
        ctx.textSize(12);
        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
        ctx.text(name().c_str(), cx, cy);
    }
};


//
// A whole page of color graphic chits
//
struct SVGColorPage : public GraphicGroup
{

    static constexpr int maxColumns = 8;
    static constexpr float fontSize = 12;
    static constexpr float rowSize = 60;
    static constexpr float columnSize = 120;
    static constexpr float scrollSize = fontSize * 3;

    static maths::vec2f preferredSize()
    {
        int rows = (svg::colors.size() / maxColumns) + 1;

        return { float((maxColumns * columnSize)), float((rows * rowSize)) };
    }

    std::shared_ptr<GraphicElement> fHoverGraphic = nullptr;

public:
    SVGColorPage(const maths::rectf& fr)
        :GraphicGroup(fr)
    {
        int column = 0;
        int row = 0;

        // Construct the children
        for (auto& entry : svg::colors)
        {
            maths::rectf f{ column * columnSize, row * rowSize, columnSize, rowSize };
            auto g = std::make_shared<SVGColorGraphic>(f, entry.first, entry.second);
            addGraphic(g);

            column = column + 1;
            if (column >= maxColumns) {
                column = 0;
                row = row + 1;
            }
        }

        setBounds(calculateExtent());
    }

    void mouseEvent(const MouseEvent& e) override
    {
        fHoverGraphic = graphicAt(e.x + fTranslation.x, e.y - fTranslation.y);
    }

    void scrollTo(float pos)
    {
        float maxY = boundsHeight() - frameHeight();
        float transY = maths::map(pos, 0, 1, 0, maxY);

        translateBoundsTo(0, -transY);
    }

    void drawBackground(IGraphics& ctx) override
    {
        ctx.push();
        ctx.noFill();
        ctx.stroke(255, 0, 0);
        ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());
        ctx.pop();
    }

    void drawForeground(IGraphics& ctx) override
    {
        if (fHoverGraphic == nullptr)
            return;

        auto fr = fHoverGraphic->frame();

        ctx.strokeWeight(3);
        ctx.stroke(255, 255, 0);
        ctx.noFill();
        ctx.rect(fr.x, fr.y, fr.w, fr.h);
    }
};
