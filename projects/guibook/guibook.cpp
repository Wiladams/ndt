#include "p5.hpp"

#include "guistyle.hpp"
#include "fonthandler.hpp"

using namespace p5;


class TextLabel : public GraphicElement
{
    std::string fText;
    std::string fFontFamily;
    float fFontSize;

public:
    TextLabel(const char* txt, const BLRect &f,
        const char *fontfamily = "Consolas", float fontSize=12)
        :GraphicElement(f.x, f.y, f.w, f.h)
    {
        fText = txt;
        fFontFamily = fontfamily;
        fFontSize = fontSize;
    }
    
    // For debugging, just show the boundary
    void drawBackground(IGraphics& ctx) override
    {
        ctx.push();

        ctx.noFill();
        ctx.stroke(0xff, 0, 0);
        ctx.rect(fBounds.x, fBounds.y, fBounds.w, fBounds.h);
    
        ctx.pop();
    }

    void drawSelf(IGraphics& ctx) override
    {
        ctx.push();

        ctx.noStroke();
        ctx.fill(0);
        ctx.textFont(fFontFamily.c_str());
        ctx.textSize(fFontSize); 
        float x = fBounds.x;
        float y = fBounds.y+ctx.textAscent();
        ctx.textAtBaseline(fText.c_str(), x, y);
    
        ctx.pop();
    }

    static std::shared_ptr<TextLabel> create(const char* txt,
        float x = 0, float y = 0, float fontSize = 12, const char* familyname = "Consolas")
    {
        // Measure the text
        auto txtSize = gFontHandler->textMeasure(txt, familyname, fontSize);
        if (txtSize.x == 0 || txtSize.y == 0)
            return nullptr;

        // create the actual text label
        BLRect frame(x, y, txtSize.x, txtSize.y);
        auto label = std::make_shared<TextLabel>(txt, frame, familyname, fontSize);

        return label;
    }
};

/*
struct Button : public Graphic
{
    void textRaised(const char* txt, int x, int y, int w, int h)
    {
        fSurface.push();
        fSurface.translate(x, y);
        fStyler.drawRaisedRect(&fSurface, 0, 0, w, h);
        fSurface.noStroke();
        ((IGraphics*)&fSurface)->fill(0, 0, 0);
        fSurface.text(txt, 4, (double)h - 6);
        fSurface.pop();
    }

    void textSunken(const char* txt, int x, int y, int w, int h)
    {
        fSurface.push();
        fSurface.translate(x, y);
        fStyler.drawSunkenRect(&fSurface, 0, 0, w, h);
        fSurface.noStroke();
        ((IGraphics*)&fSurface)->fill(0, 0, 0);
        fSurface.text(txt, 4, (double)h - 6);
        fSurface.pop();
    }
};
*/

void draw()
{
    background(0xcD);
}

void setup()
{
	createCanvas(800, 800);
    //background(0xCD);

    auto win = window(0, 0, 320, 240);
    auto label = TextLabel::create("Text Label quit",10,10, 32);

    win->addChild(label);
    //label->draw(*gAppSurface);

    //noLoop();
}