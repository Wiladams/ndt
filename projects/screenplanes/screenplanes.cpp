#include "p5.hpp"
#include "screensnapshot.hpp"
#include <cstdio>

using namespace p5;



int captureWidth = 800;
int captureHeight = 600;

int fragmentWidth = captureWidth / 2;
int fragmentHeight = captureHeight / 2;
int margin = 10;

std::shared_ptr<ScreenSnapshot> ss = nullptr;

Surface colorPlane(fragmentWidth, captureHeight / 2);


class ColorPlaneWindow : public GWindow
{
    std::shared_ptr<ScreenSnapshot> fScreenSnapshot;

    Surface redSurface;
    Surface greenSurface;
    Surface blueSurface;
    Surface graySurface;

    // convert an rgb pixel to a chroma value
    inline uint8_t toGray(const Pixel& pix)
    {
        return (0.2125 * pix.r) + (0.7154 * pix.g) + (0.0721 * pix.b);
    }

    // Take a source image, and split it into 
    // three plane images
    // This is an individual pixel operation, which could 
    // probably be done using a blend2d composite command, but since
    // it's a pretty simple operation, it's probably not going to get
    // much faster.
    bool splitPlanes()
    {
        auto src = ss->getCurrent();

        for (int y = 0; y < src.getHeight(); y++)
            for (int x = 0; x < src.getWidth(); x++)
            {
                Pixel srcColor = src.get(x, y);

                redSurface.set(x, y, { srcColor.r,0,0,255 });
                greenSurface.set(x, y, { 0,srcColor.g,0,255 });
                blueSurface.set(x, y, { 0,0,srcColor.b,255 });
                
                auto g = toGray(srcColor);
                graySurface.set(x, y, { g,g,g,255 });
            }

        return true;
    }

public:
    ColorPlaneWindow(int w, int h, std::shared_ptr<ScreenSnapshot> ss)
        :GWindow(0,0,w,h),
        fScreenSnapshot(ss),
        redSurface(ss->fWidth,ss->fHeight),
        greenSurface(ss->fWidth, ss->fHeight),
        blueSurface(ss->fWidth, ss->fHeight),
        graySurface(ss->fWidth, ss->fHeight)
    {
    }

    void drawBackground(IGraphics* ctx)
    {
        fScreenSnapshot->moveNext();
        splitPlanes();

        // red green and blue fragments
        ctx->scaleImage(redSurface.getBlend2dImage(), 0, 0, redSurface.getWidth(), redSurface.getHeight(), margin, margin, fragmentWidth, fragmentHeight);
        ctx->scaleImage(greenSurface.getBlend2dImage(), 0, 0, greenSurface.getWidth(), greenSurface.getHeight(), margin + (((fragmentWidth + margin) * 1)), margin, fragmentWidth, fragmentHeight);
        ctx->scaleImage(blueSurface.getBlend2dImage(), 0, 0, blueSurface.getWidth(), blueSurface.getHeight(), margin, margin + ((fragmentHeight + margin) * 1), fragmentWidth, fragmentHeight);
        ctx->scaleImage(graySurface.getBlend2dImage(), 0, 0, graySurface.getWidth(), graySurface.getHeight(), margin + (((fragmentWidth + margin) * 1)), margin + ((fragmentHeight + margin) * 1), fragmentWidth, fragmentHeight);
    }
};



void draw()
{
    clear();

    // Get capture current screen
    //ss->moveNext();

    // Display normal but shrunken
    //Surface &surf = ss->getCurrent();
    //BLImage& img = surf.getBlend2dImage();

    // draw into the color plane so it's the right size
    //colorPlane.scaleImage(img, 0, 0, img.width(), img.height(), 0, 0, colorPlane.getWidth(), colorPlane.getHeight());
    //colorPlane.flush();

     // Display regular scaled down
    //scaleImage(colorPlane.getBlend2dImage(), 0,0, colorPlane.getWidth(), colorPlane.getHeight(), 10, 10, fragmentWidth, fragmentHeight);
}


void setup()
{
    fullscreen();
    //createCanvas(1280,800);
    frameRate(30);

    ss = std::make_shared<ScreenSnapshot>(0, 0, captureWidth, captureHeight);
    auto win = std::make_shared<ColorPlaneWindow>(captureWidth, captureHeight, ss);
    addWindow(win);
}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}