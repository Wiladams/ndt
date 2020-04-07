#include "p5.hpp"
#include "PixelBufferCodec.hpp"

void draw()
{
    // create a couple of commands and execute them
    PBOpSetAllPixels clearAll(*gAppSurface, colors.yellow);

    clearAll();

    noLoop();
}

void setup()
{
    createCanvas(800, 600);
}