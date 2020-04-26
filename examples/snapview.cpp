#include "p5.hpp"

#include "screensnapshot.hpp"

using namespace p5;

ScreenSnapshot *ss = nullptr;

int gScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
int gScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

int captureWidth = gScreenWidth/2;
int captureHeight = gScreenHeight /2;


void displayQuad(BLImage& src, const BLRectI& rect, int lvl=1)
{
    // 1 full rectangle
    scaleImage(src, 0, 0, src.width(), src.height(), rect.x, rect.y, rect.w, rect.h);


    // 3 - half sized
    scaleImage(src, 0, 0, src.width(), src.height(), rect.x+rect.w/2, rect.y, rect.w/2, rect.h/2);
    scaleImage(src, 0, 0, src.width(), src.height(), rect.x + rect.w / 2, rect.y+rect.h/2, rect.w / 2, rect.h / 2);
    scaleImage(src, 0, 0, src.width(), src.height(), rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2);

    lvl--;

    if (lvl > 0) {
        displayQuad(src, {rect.x+(int)(rect.w/2), (int)rect.y, (int)rect.w/2, (int)rect.h/2}, lvl-1);
        displayQuad(src, { rect.x + (int)(rect.w / 2), (int)rect.y+(int)(rect.h/2), (int)rect.w / 2, (int)rect.h / 2 }, lvl-1);
    }
}

void draw()
{
    ss->moveNext();
    
    displayQuad(ss->getCurrent().getBlend2dImage(), { 0,0,(int)width, (int)height }, 4);
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
    createCanvas(displayWidth/4, displayHeight/4);
    frameRate(30);

    ss = new ScreenSnapshot(0, 0, captureWidth, captureHeight);
}