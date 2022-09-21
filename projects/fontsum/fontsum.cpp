#include "p5.hpp"

#include <array>

using namespace p5;

static std::string lineText = "The quick brown fox jumps over the lazy dog. 1234587890";

struct LineEntry {
    size_t size;
    size_t lineHeight;
};

std::array<LineEntry, 7> const lines { {
{12, 16 },
{18, 18 },
{24, 24 },
{36, 36 },
{48, 48 },
{60, 60 },
{72, 72 },
} };


class GFontSummary : public Graphic
{
    std::string fontFamily;

public:
    BLSizeI static getPrefferedSize()
    {
        return { 3000, 1024 };
    }



    GFontSummary(const char *family)
    {
        auto psize = GFontSummary::getPrefferedSize();
        setFrame(BLRect(0, 0, psize.w, psize.h));
    
        fontFamily = family;
    }

    void draw(IGraphics & ctx)
    {
        // print("GFontSummary.draw 1.0")
        ctx.background(color(255));

        // Save state on entry
        ctx.push();

        // Get line stroking done first
        ctx.stroke(0);
        ctx.strokeWeight(0.25);
        ctx.line(0, 52, fFrame.w, 52);
        ctx.line(0, 93.5, fFrame.w, 93.5);



        ctx.noStroke();
        ctx.fill(0);

        // Draw font attribute titles
        //ctx.textFont("consolas");
        ctx.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
        //ctx.textFont("Consolas");
        ctx.textSize(16);
        ctx.text("Font Name:", 92, 10);
        ctx.text("Attributes:", 92, 24);

        ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
        ctx.text(fontFamily.c_str(), 96, 10);


        ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
        ctx.textFont(fontFamily.c_str());
        ctx.textSize(16);
        ctx.text("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", 2, 67);
        ctx.text("1234567890.:,; ' \" (!?) +-*/=", 2, 85);
        
        ctx.translate(4, 90);

        for (size_t i = 0; i < lines.size(); i++) {
            auto line = lines[i];
            ctx.translate(0, line.lineHeight);

            // draw lines showing our baseline
            //ctx.stroke(255, 0, 0);
            //ctx.line(0, 0, canvasWidth, 0);
            //ctx.line(0, 0, 0, -((double)line.lineHeight)*0.85);

            //draw the size indicator
            ctx.noStroke();
            ctx.textFont("Consolas");
            ctx.textSize(16);
            ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
            ctx.text(std::to_string(line.size).c_str(), 0, 0);

            // Draw the line of text
            ctx.noStroke();
            ctx.fill(0);
            ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
            ctx.textFont(fontFamily.c_str());
            ctx.textSize(line.size);
            //ctx.text(lineText.c_str(), 40, 0);
            ctx.text(lineText.c_str(), 24, 0);
        }

        ctx.pop();

    }

};


void draw()
{
    background(0xC0);

    //GFontSummary s("algerian");
    //GFontSummary s("d:\\Fonts\\Old Copperfield.ttf");
    //GFontSummary s("c:\\Windows\\Fonts\\STENCIL.ttf");
    //GFontSummary s("c:\\Windows\\Fonts\\comic.ttf");
    GFontSummary s("Bookman Old Style");
    //GFontSummary s("Snap ITC");
    //GFontSummary s("Stencil");

    s.draw(*gAppSurface);

    noLoop();
}

void loadSomeFonts()
{
    std::vector<const char*> fontNames{
            "c:\\Windows\\Fonts\\bookos.ttf",
            "c:\\Windows\\Fonts\\SNAP____.ttf",
            "c:\\Windows\\Fonts\\STENCIL.ttf",
    };

    loadFontFiles(fontNames);
}

void setup()
{
    loadSomeFonts();

    createCanvas(1920, 1280, "fontsum");

    // Set user space units to be 96/inch
    //gAppSurface->setPpiUnits(systemPpi, 72);    // Using points (1/72in)
}



