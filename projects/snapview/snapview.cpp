// Show a MIP Map of a screen capture
// The screen capture itself is just a readily available
// bitmap we can play with.
// The real demonstration here is bitmap scaling as we 
// draw iteratively smaller images a few times.


#include "p5.hpp"

#include "elements/screensnapshot.hpp"

using namespace p5;

ScreenSnapper ss;



void displayQuad(IGraphics &ctx, BLImage& src, const BLRectI& rect, int lvl=1)
{
    // 1 full rectangle
    ctx.scaleImage(src, 0, 0, src.width(), src.height(), rect.x, rect.y, rect.w, rect.h);

    // Progressively smaller rectangles
    int xoffset = src.width();
    int yoffset = 0;
    for (int iter = 1; iter <= lvl; iter++)
    {
        int yheight = src.height() / pow(2, iter);
        int xwidth = src.width() / pow(2, iter);
        ctx.scaleImage(src, 0, 0, src.width(), src.height(), xoffset, yoffset, xwidth, yheight);
        
        xoffset += 0;
        yoffset += yheight;
    }
}

void draw()
{
    ss.next();
    auto & src = ss.getImage();
    displayQuad(*gAppSurface, src, { 0,0,src.width(), src.height() }, 8);
}

void keyReleased(const KeyboardEvent &e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}

void setup()
{
    createCanvas(displayWidth/2, displayHeight/2);
    frameRate(15);

    ss.reset(0, 0, displayWidth/3, displayHeight/2);
}