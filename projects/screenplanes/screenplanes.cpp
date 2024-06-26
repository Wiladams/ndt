#include "p5.hpp"
#include "canvas.h"


#include "elements/screensnapshot.hpp"
#include "elements/framestats.h"



using namespace p5;



// Take a source image, and split it into 
// three plane images
class ColorPlaneWindow : public GraphicElement
{
    int cellWidth = 64;
    int cellHeight = 64;
    int margin = 4;

    ScreenSnapper & fSnapper;

    GCanvas redSurface;
    GCanvas greenSurface;
    GCanvas blueSurface;
    GCanvas graySurface;


    // convert an rgb pixel to a chroma value
    static INLINE uint8_t toGray(const Pixel& pix)
    {
        return (0.2125 * pix.r()) + (0.7154 * pix.g()) + (0.0721 * pix.b());
    }

    // This is an individual pixel operation, which could 
    // probably be done using a blend2d composite command, but since
    // it's a pretty simple operation, it's probably not going to get
    // much faster.
    bool splitPlanes()
    {
        // read the pixel value for each location
        // and create the r,g,b,and gray pixels
        // from there.
        for (int y = 0; y < fSnapper.height(); y++)
        {
            Pixel* ptr = (Pixel*)fSnapper.rowPointer(y);
            for (int x = 0; x < fSnapper.width(); x++)
            {
                Pixel srcColor = ptr[x];

                redSurface.set(x, y, { srcColor.r(),0,0,255 });
                greenSurface.set(x, y, { 0,srcColor.g(),0,255 });
                blueSurface.set(x, y, { 0,0,srcColor.b(),255 });

                auto g = toGray(srcColor);
                graySurface.set(x, y, { g,g,g,255 });
            }
        }
        return true;
    }

public:
    ColorPlaneWindow(int w, int h, ScreenSnapper & ss)
        :GraphicElement(0,0,w,h)
        ,fSnapper(ss)
        ,redSurface(ss.width(),ss.height())
        ,greenSurface(ss.width(), ss.height())
        ,blueSurface(ss.width(), ss.height())
        ,graySurface(ss.width(), ss.height())
    {
        cellWidth = (w / 2) - (margin/2);
        cellHeight = (h / 2) - (margin/2);
    }

    void drawBackground(IGraphics & ctx) override
    {
        splitPlanes();

        // red green and blue fragments
        ctx.scaleImage(redSurface.getImage(), 0, 0, redSurface.getWidth(), redSurface.getHeight(), margin, margin, cellWidth, cellHeight);
        ctx.scaleImage(greenSurface.getImage(), 0, 0, greenSurface.getWidth(), greenSurface.getHeight(), margin + (((cellWidth + margin) * 1)), margin, cellWidth, cellHeight);
        ctx.scaleImage(blueSurface.getImage(), 0, 0, blueSurface.getWidth(), blueSurface.getHeight(), margin, margin + ((cellHeight + margin) * 1), cellWidth, cellHeight);
        ctx.scaleImage(graySurface.getImage(), 0, 0, graySurface.getWidth(), graySurface.getHeight(), margin + (((cellWidth + margin) * 1)), margin + ((cellHeight + margin) * 1), cellWidth, cellHeight);
    }
};


static int captureWidth = 1280;
static int captureHeight = 1024;
FrameStats _stats;


static ScreenSnapper ss;


void draw()
{
    clear();

    // capture current screen
    ss.next();
}

void onComposed()
{
    _stats.draw(*gAppSurface);
}

void setup()
{
    captureWidth = displayWidth/2;
    captureHeight = displayHeight;
    
    double windowScale = 1;

    frameRate(20);
    createCanvas(displayWidth / 2, displayHeight, "screenplanes",4);
    //fullscreen();

    ss.reset(0, 0, captureWidth, captureHeight);

    // create a window to hold the split plane graphic
    auto win = window(0, 0, captureWidth* windowScale, captureHeight* windowScale);
    win->setTitle("Split Panes");
    
    auto colorpanes = std::make_shared<ColorPlaneWindow>(captureWidth * windowScale, captureHeight* windowScale, ss);
    win->addGraphic(colorpanes);
}

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}