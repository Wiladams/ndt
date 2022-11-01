#pragma once

#include "graphic.hpp"

#include <array>

struct LineEntry {
    size_t size;
    size_t lineHeight;
};

class FontSummary : public Graphic 
{
    static maths::vec2f getPrefferedSize()
    {
        return { 3000, 1024 };
    }

    std::string fFontFamily;

public:

    FontSummary(int x, int y, int w, int h, const char* family)
        :Graphic(x,y,w,h)
    {
        //auto psize = FontSummary::getPrefferedSize();

        setFamily(family);
    }

    void setFamily(const std::string & familyname)
    {
        this->fFontFamily = familyname;
    }

    // Act as a functor so that we can subscribe to 
    // name change publishing.
    void operator() (const std::string& s)
    {
        printf("FontSummary: %s\n", s.c_str());
        setFamily(s);
    }

    void drawFontName(IGraphics& ctx)
    {
        ctx.noStroke();
        ctx.fill(0);

        // Draw font attribute titles
        //ctx.textFont("consolas");
        ctx.textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
        ctx.textFont("Consolas");
        ctx.textSize(16);
        ctx.text("Font Name:", 100, 10);
        ctx.text("Attributes:", 100, 24);

        ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);
        ctx.text(fFontFamily.c_str(), 108, 10);

        // Need a string for the attributes
    }

    void drawSelf(IGraphics& ctx)override
    {
        std::array<LineEntry, 7> lines{ {
            {12, 16 },
            {18, 18 },
            {24, 24 },
            {36, 36 },
            {48, 48 },
            {60, 60 },
            {72, 72 },
        } };

        const char* lineText = "The quick brown fox jumps over the lazy dog. 1234587890";

        // print("GFontSummary.draw 1.0")
        ctx.background(Pixel(255,255,255));

        // Save state on entry
        ctx.push();

        // Get line stroking done first
        ctx.stroke(0);
        ctx.strokeWeight(0.25);
        ctx.line(0, 52, frameWidth(), 52);
        ctx.line(0, 93.5, frameWidth(), 93.5);




        drawFontName(ctx);




        ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
        ctx.textFont(fFontFamily.c_str());
        ctx.textSize(16);
        ctx.text("abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", 2, 67);
        ctx.text("1234567890.:,; ' \" (!?) +-*/=", 2, 85);

        ctx.translate(4, 90);

        for (size_t i = 0; i < lines.size(); i++) {
            auto &line = lines[i];
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
            ctx.textFont(fFontFamily.c_str());
            ctx.textSize(line.size);
            //ctx.text(lineText.c_str(), 40, 0);
            ctx.text(lineText, 24, 0);
        }
        ctx.pop();

    }

};
