#include "p5.hpp"
#include "screensnapshot.hpp"
#include "canvas.h"
#include "framestats.h"
#include <cstdio>

using namespace p5;

static const int captureWidth = 800;
static const int captureHeight = 600;
FrameStats _stats;

std::shared_ptr<ScreenSnapshot> ss = nullptr;


// Take a source image, and split it into 
// three plane images
class ColorPlaneWindow : public Graphic
{
    int cellWidth = 64;
    int cellHeight = 64;
    int margin = 4;

    std::shared_ptr<ScreenSnapshot> fScreenSnapshot;

    GCanvas redSurface;
    GCanvas greenSurface;
    GCanvas blueSurface;
    GCanvas graySurface;


    // convert an rgb pixel to a chroma value
    inline uint8_t toGray(const Pixel& pix)
    {
        return (0.2125 * pix.r) + (0.7154 * pix.g) + (0.0721 * pix.b);
    }

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
        :Graphic(0,0,w,h),
        fScreenSnapshot(ss),
        redSurface(ss->width(),ss->height()),
        greenSurface(ss->width(), ss->height()),
        blueSurface(ss->width(), ss->height()),
        graySurface(ss->width(), ss->height())
    {
        cellWidth = (w / 2) - (margin/2);
        cellHeight = (h / 2) - (margin/2);
    }

    void drawBackground(std::shared_ptr<IGraphics> ctx)
    {
        splitPlanes();

        // red green and blue fragments
        ctx->scaleImage(redSurface.getImage(), 0, 0, redSurface.getWidth(), redSurface.getHeight(), margin, margin, cellWidth, cellHeight);
        ctx->scaleImage(greenSurface.getImage(), 0, 0, greenSurface.getWidth(), greenSurface.getHeight(), margin + (((cellWidth + margin) * 1)), margin, cellWidth, cellHeight);
        ctx->scaleImage(blueSurface.getImage(), 0, 0, blueSurface.getWidth(), blueSurface.getHeight(), margin, margin + ((cellHeight + margin) * 1), cellWidth, cellHeight);
        ctx->scaleImage(graySurface.getImage(), 0, 0, graySurface.getWidth(), graySurface.getHeight(), margin + (((cellWidth + margin) * 1)), margin + ((cellHeight + margin) * 1), cellWidth, cellHeight);
    }
};



void draw()
{
    clear();

    // Get capture current screen
    ss->next();

    _stats.draw(gAppSurface);
}


void setup()
{
    double windowScale = 1;

    fullscreen();
    //frameRate(30);

    // create a window to hold the split plane graphic
    auto win = window(0, 0, captureWidth* windowScale, captureHeight* windowScale);
    ss = std::make_shared<ScreenSnapshot>(0, 0, captureWidth, captureHeight);
    auto splitwin = std::make_shared<ColorPlaneWindow>(captureWidth * windowScale, captureHeight* windowScale, ss);
    win->addChild(splitwin);
    win->setTitle("Split Panes");
    win->moveBy(displayWidth / 2, 0);
}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}