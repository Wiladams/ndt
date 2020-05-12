#include "p5.hpp"
#include "screensnapshot.hpp"
#include <cstdio>

using namespace p5;

ScreenSnapshot* ss = nullptr;

int captureWidth = 800;
int captureHeight = 600;

int fragmentWidth = captureWidth / 2;
int fragmentHeight = captureHeight / 2;

Surface colorPlane(fragmentWidth, captureHeight / 2);
Surface grayPlane(fragmentWidth, captureHeight / 2);
Surface redPlane(fragmentWidth, captureHeight/2);
Surface greenPlane(fragmentWidth, captureHeight/2);
Surface bluePlane(fragmentWidth, captureHeight/2);

inline uint8_t toGray(const Color &pix)
{
    return (0.2125 * pix.r) + (0.7154 * pix.g) + (0.0721 * pix.b);
}

void grayFilter(Surface& src, Surface &dst)
{
    //printf("redFilter: %dx%d\n", dst.getWidth(), dst.getHeight());
    src.flush();

    for (int y = 0; y < dst.getHeight(); y++)
        for (int x = 0; x < dst.getWidth(); x++) {
            Color srcColor = src.get(x, y);
            uint8_t gr = toGray(srcColor);
            Color newColor = color(gr, gr,gr);
            
            dst.set(x, y, newColor);            
        }
    
    dst.flush();
}



bool rgbSplitter(Surface& src, Surface& redSurface, Surface &greenSurface, Surface &blueSurface)
{
    src.flush();

    for (int y = 0; y < src.getHeight(); y++)
        for (int x = 0; x < src.getWidth(); x++) {
            Color srcColor = src.get(x, y);
            redSurface.set(x, y, color(red(srcColor), 0, 0));
            greenSurface.set(x, y, color(0,green(srcColor), 0));            
            blueSurface.set(x, y, color(0, 0, blue(srcColor)));
        }

    return true;
}


void draw()
{
    // Get capture current screen
    ss->moveNext();

    // Display normal but shrunken
    Surface &surf = ss->getCurrent();
    BLImage& img = surf.getBlend2dImage();

    // draw into the color plane so it's the right size
    colorPlane.scaleImage(img, 0, 0, img.width(), img.height(), 0, 0, colorPlane.getWidth(), colorPlane.getHeight());
    colorPlane.flush();

    loadPixels();
    rgbSplitter(colorPlane, redPlane, greenPlane, bluePlane);
    grayFilter(colorPlane, grayPlane);

    // Display regular scaled down
    scaleImage(colorPlane.getBlend2dImage(), 0,0, colorPlane.getWidth(), colorPlane.getHeight(), 10, 10, fragmentWidth, fragmentHeight);

    // Display grayscale
    scaleImage(grayPlane.getBlend2dImage(), 0, 0, grayPlane.getWidth(), grayPlane.getHeight(), 10 + (((fragmentWidth + 10) * 2)), 10, fragmentWidth, fragmentHeight);

    // red green and blue fragments
    scaleImage(redPlane.getBlend2dImage(), 0, 0, redPlane.getWidth(), redPlane.getHeight(), 10 + (((fragmentWidth + 10) * 0)), fragmentHeight + 10+10, fragmentWidth, fragmentHeight);
    scaleImage(greenPlane.getBlend2dImage(), 0, 0, greenPlane.getWidth(), greenPlane.getHeight(), 10 + (((fragmentWidth+10)*1)), fragmentHeight+10 + 10, fragmentWidth, fragmentHeight);
    scaleImage(bluePlane.getBlend2dImage(), 0, 0, bluePlane.getWidth(), bluePlane.getHeight(), 10 + (((fragmentWidth + 10) * 2)), fragmentHeight + 10 + 10, fragmentWidth, fragmentHeight);

    updatePixels();
}


void setup()
{
    createCanvas(1280,800);
    frameRate(30);

    ss = new ScreenSnapshot(0, 0, captureWidth, captureHeight);
}