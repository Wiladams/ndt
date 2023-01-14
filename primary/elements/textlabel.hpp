#pragma once


#include "fonthandler.hpp"
#include "graphic.hpp"
#include "elements/guistyle.hpp"
#include "elements/textlabel.hpp"

class TextLabel : public GraphicElement
{
    std::string fText;
    std::string fFontFamily;
    float fFontSize;
    Pixel fTextFillColor;

public:
    TextLabel(const std::string & txt, const maths::rectf& f,
        const char* fontfamily = "Consolas", float fontSize = 12)
        :GraphicElement(f)
    {
        fText = txt;
        fFontFamily = fontfamily;
        fFontSize = fontSize;
        fTextFillColor = Pixel(0, 0, 0);
    }

    void setTextFillColor(const Pixel& c) { fTextFillColor = c; }

    // For debugging, just show the boundary
    void drawBackground(IGraphics& ctx) override
    {
        // Debug, show bounds rectangle
        ctx.push();
        ctx.noFill();
        ctx.stroke(0xff, 0, 0);
        ctx.strokeWeight(1.0);
        //ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());
        ctx.pop();
    }

    void drawSelf(IGraphics& ctx) override
    {
        ctx.push();

        ctx.noStroke();
        ctx.fill(fTextFillColor);
        ctx.textFont(fFontFamily.c_str());
        ctx.textSize(fFontSize);
        float x = bounds().x;
        float y = bounds().y + ctx.textAscent();
        ctx.textAtBaseline(fText.c_str(), x, y);

        ctx.pop();
    }

    static std::shared_ptr<TextLabel> create(const std::string &txt,
        float x = 0, float y = 0, float fontSize = 12, const char* familyname = "Consolas")
    {
        // Measure the text
        auto txtSize = gFontHandler->textMeasure(txt.c_str(), familyname, fontSize);
        if (txtSize.x == 0 || txtSize.y == 0)
            return nullptr;

        // create the actual text label
        maths::bbox2f gframe{ {x,y},{x+txtSize.x,y+txtSize.y} };
        auto label = std::make_shared<TextLabel>(txt, gframe, familyname, fontSize);

        return label;
    }
};
