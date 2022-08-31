#include "p5.hpp"
#include "screensnapshot.hpp"
#include <cmath>

// 2D Water Ripples
// Video: https://youtu.be/BZUdGqeOD0w
// Algorithm: https://web.archive.org/web/20160418004149/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm
// https://www.gamedev.net/articles/programming/graphics/the-water-effect-explained-r915/
//
using namespace p5;


template <typename T>
T** Array2D(int rows, int columns, const T& val)
{
    T** arr = new T * [rows];
    for (int i = 0; i < rows; i++) {
        arr[i] = new T[columns];
    }

    // Initialize
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < columns; x++) {
            arr[y][x] = val;
        }
    }

    return arr;
}


struct rippler {
    int cols;
    int rows;
    float** fCurrent;
    float** fPrevious;
    float fDampening = 0.99;
    int fWaveDepth = 500;


    rippler(int w, int h, float damper=0.9)
        :cols(w),
        rows(h),
        fDampening(damper)
    {
        fCurrent = Array2D<float>(rows, cols, 0);
        fPrevious = Array2D<float>(rows, cols, 0);
    }

    float** current() { return fCurrent; }

    float getDampening() { return fDampening; }
    void setDampening(const float d)
    {
        fDampening = constrain(d, 0.01, 0.99);
    }

    int getWaveDepth() { return fWaveDepth; }
    void setWaveDepth(const float value) { fWaveDepth = value; }

    void swap()
    {
        // Swap buffers
        float** temp = fPrevious;
        fPrevious = fCurrent;
        fCurrent = temp;
    }

    void update()
    {
        for (int y = 1; y < rows - 1; y++) {
            for (int x = 1; x < cols - 1; x++) {
                fCurrent[y][x] = (
                    fPrevious[y - 1][x] +
                    fPrevious[y + 1][x] +
                    fPrevious[y][x - 1] +
                    fPrevious[y][x + 1]) / 2 -
                    fCurrent[y][x];
                fCurrent[y][x] = fCurrent[y][x] * fDampening;
            }
        }

        swap();
    }

    bool getDisplacement(const int x, const int y, int &dx, int &dy)
    {
        float rIndex = 2.0;     // refraction index of water
        int xDiff = int(fCurrent[y][x + 1] - fCurrent[y][x]);
        int yDiff = int(fCurrent[y + 1][x] - fCurrent[y][x]);

        float xAngle = atan(xDiff);
        float xRefraction = asin(sin(xAngle) / rIndex);
        float xDisplace = int(tan(xRefraction) * xDiff);

        float yAngle = atan(yDiff);
        float yRefraction = asin(sin(yAngle) / rIndex);
        float yDisplace = int(tan(yRefraction) * yDiff);

        if (xDiff < 0) {
            if (yDiff < 0) {
                dx = x - xDisplace;
                dy = y - yDisplace;
            }
            else {
                dx = x - xDisplace;
                dy = y + yDisplace;
            }
        }
        else {
            if (yDiff < 0) {
                dx = x + xDisplace;
                dy = y - yDisplace;
            }
            else {
                dx = x + xDisplace;
                dy = y + yDisplace;
            }
        }

        return true;
    }

    void draw(Surface &ctx)
    {
        update();

        ctx.loadPixels();
        Pixel* pixels = ctx.getData();
        for (int y = 1; y < rows - 1; y++) {
            for (int x = 1; x < cols - 1; x++) {
                int index = x + y * cols;
                pixels[index] = color(fCurrent[y][x]);
            }
        }
        //updatePixels();
    }

    void droplet(int x, int y)
    {
        //printf("droplet: %d, %d   (%d)\n", x, y, fWaveDepth);

        fPrevious[y][x] = fWaveDepth;
    }
};



rippler *r1=nullptr;
ScreenSnapshot* snapper;

void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE) {
        halt();
    }
}

void mouseDragged(const MouseEvent& e)
{
    int mx = constrain(mouseX, 0, canvasWidth - 1);
    int my = constrain(mouseY, 0, canvasHeight - 1);
    r1->droplet(mx, my);
}

void mousePressed(const MouseEvent& e)
{
    int mx = constrain(mouseX, 0, canvasWidth - 1);
    int my = constrain(mouseY, 0, canvasHeight - 1);
    r1->droplet(mx, my);
}

void mouseWheel(const MouseEvent& e)
{
    int oldValue = r1->getWaveDepth();
    if (e.delta > 0)
    {
        oldValue += 50;
    }
    else {
        oldValue -= 50;
    }
    r1->setWaveDepth(oldValue);
}

void draw()
{
    // update the wave map
    r1->update();

    // get current screen snapshot
    snapper->next();
    Surface &surf = snapper->getCurrent();

    loadPixels();
    // Draw pixel by pixel, asking the wave map what the source
    // pixel should be for a given position
    for (int y = 1; y < canvasHeight - 1; y++) {
        for (int x = 1; x < canvasWidth - 1; x++) {
            int dx;
            int dy;
            
            // get displacement for x,y
            r1->getDisplacement(x, y, dx, dy);
            Pixel c = surf.get(dx, dy);
            pixels[y * canvasWidth + x] = c;
            //set(x, y, c);
        }
    }
}

void setup()
{
    createCanvas(800, 800);

    r1 = new rippler(canvasWidth, canvasHeight, 0.95);
    snapper = new ScreenSnapshot(0, 0, canvasWidth, canvasHeight);

    frameRate(30);
}