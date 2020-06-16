#include "p5.hpp"

// Show a MIP Map of a screen capture
// There's nothing special about the screen capture
// in this case.  It's realll mipmap which is interesting
// the screen capture just makes for a quick and easy source
// of 2D graphics data
#include "screensnapshot.hpp"

using namespace p5;

ScreenSnapshot *ss = nullptr;


void displayQuad(BLImage& src, const BLRectI& rect, int lvl=1)
{
    // 1 full rectangle
    scaleImage(src, 0, 0, src.width(), src.height(), rect.x, rect.y, rect.w, rect.h);

    int xoffset = src.width();
    int yoffset = 0;
    for (int iter = 1; iter <= lvl; iter++)
    {
        int yheight = src.height() / pow(2, iter);
        int xwidth = src.width() / pow(2, iter);
        scaleImage(src, 0, 0, src.width(), src.height(), xoffset, yoffset, xwidth, yheight);
        
        xoffset += 0;
        yoffset += yheight;
    }
}

void draw()
{
    ss->moveNext();
    auto src = ss->getCurrent().getBlend2dImage();
    displayQuad(src, { 0,0,src.width(), src.height() }, 8);
}

int T_SP = ' ';

void keyTyped(const KeyEvent& event)
{
    // if [Space] save file
    if (keyCode == T_SP) {
        BLImageCodec codec;
        codec.findByName("BMP");
        gAppSurface->getBlend2dImage().writeToFile("screen.bmp", codec);
    }
}

void setup()
{
    createCanvas(displayWidth/2, displayHeight/2);
    frameRate(30);

    ss = new ScreenSnapshot(0, 0, displayWidth/3, displayHeight/2);
}