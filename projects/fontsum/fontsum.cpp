#include "p5.hpp"

#include <array>

using namespace p5;

static std::string lineText = " The quick brown fox jumps over the lazy dog. 1234587890";

struct LineEntry {
    size_t size;
    size_t lineHeight;
};

std::array<LineEntry, 8> const lines { {
{12, 16 },
{18, 24 },
{24, 36 },
{36, 48 },
{48, 60 },
{60, 72 },
{72, 96 },
{96, 120 } } };


class GFontSummary : public Graphic
{
    std::string fontFamily;

public:
    BLSizeI static getPrefferedSize()
    {
        return { 1280, 768 };
    }



    GFontSummary(const char *family)
    {
        auto psize = GFontSummary::getPrefferedSize();
        setFrame(BLRect(0, 0, psize.w, psize.h));
    
        fontFamily = family;
    }

    void draw(std::shared_ptr<IGraphics> ctx)
    {
        // print("GFontSummary.draw 1.0")
        ctx->background(color(255));

        // Save state on entry
        ctx->push();

        // Get line stroking done first
        ctx->stroke(0);
        ctx->strokeWeight(1);
        ctx->line(0, 52, fFrame.w, 52);
        ctx->line(0, 110, fFrame.w, 110);



        ctx->noStroke();
        ctx->fill(0);

        // Draw font attribute titles
        ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
        ctx->textFont("consolas");
        

        ctx->textSize(12);

        ctx->text("Font Name", 2, 10);
        ctx->text("Version:", 2, 26);
        ctx->text("Attributes", 2, 42);
        


        ctx->textFont(fontFamily.c_str());
        ctx->textSize(18);
        ctx->text("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", 2, 76);
        ctx->text("1234567890.:,; ' \" (!?) +-*/=", 2, 96);
        
        ctx->translate(0, 110);

        for (size_t i = 0; i < lines.size(); i++) {
            auto line = lines[i];
            //ctx->translate(0, line.lineHeight * 1.5);
            ctx->translate(0, line.lineHeight);

            //draw the size indicator
            ctx->textFont("segoe ui");
            ctx->textSize(12);
            ctx->text(std::to_string(line.size).c_str(), 2, 0);

            // Draw the line of text
            ctx->textFont(fontFamily.c_str());
            ctx->textSize(line.size);
            ctx->text(lineText.c_str(), 40, 0);
        }

        ctx->pop();

    }

};


void draw()
{
    background(0xC0);

    //GFontSummary s("algerian");
    //GFontSummary s("d:\\Fonts\\Old Copperfield.ttf");
    GFontSummary s("c:\\Windows\\Fonts\\STENCIL.ttf");

    s.draw(gAppSurface);

    noLoop();
}

void setup()
{
    createCanvas(1024, 768);
}



