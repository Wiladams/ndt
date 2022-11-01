#include "p5.hpp"
//#include "apphost.h"
//#include "maths.hpp"
//#include "Surface.h"


#include <memory>

//Surface gSurface;
using namespace p5;

int max_distance;

enum SHAPEKIND {
    FIRSTSHAPE
    ,ELLIPSE = FIRSTSHAPE
    ,SQUARE
    ,LASTSHAPE = SQUARE
};

int gKindOfShape = FIRSTSHAPE;

void setup()
{
    //createCanvas();
    //createCanvas(1920, 1080);
    //setCanvasPosition(0, 0);
    fullscreen();

    noStroke();
    fill(Pixel(255, 255, 255, 255));

    max_distance = maths::distance(
        maths::vec2f{ (float)canvasWidth / 2.0f, 0 },
        maths::vec2f{ (float)canvasWidth, (float)canvasHeight });

    layered();

    //recorder = std::make_shared<Recorder>(gSurface, "dist2d-");

}

/*
void createCanvas()
{
    createAppWindow(displayWidth, displayHeight, "distance2d");
    gAppWindow->moveTo(0, 0);

    gSurface.attachPixelArray(*gAppFrameBuffer);

    layered();
}
*/

void keyReleased(const KeyboardEvent& e)
{
    switch (e.keyCode) {
    case 'R':	// toggle recording
        //recorder->toggleRecording();
        break;

    case VK_SPACE:
        gKindOfShape += 1;
        if (gKindOfShape > LASTSHAPE)
            gKindOfShape = FIRSTSHAPE;
        break;

    case VK_ESCAPE:
        halt();
        break;
    }
}




void draw() {
    if (isLayered())
    {
        clear();
        background(Pixel(10, 10,10, 127));
    }
    else
        background(Pixel(10, 10, 10, 127));

    fill(Pixel(250,250,250,220));
    for (int i = 0; i <= canvasWidth; i += 20) {
        for (int j = 0; j <= canvasHeight; j += 20) {
            float size = (maths::distance(maths::vec2f{ (float)mouseX, (float)mouseY }, 
                maths::vec2f{ (float)i, (float)j })) / 2;
            size = (size / max_distance) * 66;

            switch (gKindOfShape)
            {
            case ELLIPSE:
                ellipse(i, j, size, size);
                break;

            case SQUARE:
                square(i, j, size*2);
                break;
            }
        }
    }

    screenRefresh();
}



