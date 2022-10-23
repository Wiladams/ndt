//
// Reference
//  https://p5js.org/examples/math-graphing-2d-equations.html
//
//#include "p5.hpp"
#include "apphost.h"

//using namespace p5;

int width = 0;
int height = 0;
int mouseX = 0;
int mouseY = 0;

void handleMouse(const MouseEvent& e)
{
    mouseX = e.x;
    mouseY = e.y;
}

void onLoad() 
{
    createAppWindow(710, 400, "graph2d");

    width = canvasWidth;
    height = canvasHeight;

    subscribe(handleMouse);
}

void onLoop() 
{
    auto n = (mouseX * 10.0) / width;
    const double w = 16.0; // 2D space width
    const double h = 16.0; // 2D space height
    const double dx = w / width; // Increment x this amount per pixel
    const double dy = h / height; // Increment y this amount per pixel
    auto x = -w / 2; // Start x at -1 * width / 2
    double y;

    double r;
    double theta;
    double val;

    int bw; //variable to store grayscale
    int i;
    int j;
    int cols = width;
    int rows = height;

    for (i = 0; i < cols; i += 1) {
        y = -h / 2;
        for (j = 0; j < rows; j += 1) {
            r = maths::sqrt(x * x + y * y); // Convert cartesian to polar
            theta = maths::atan2(y, x); // Convert cartesian to polar
            // Compute 2D polar coordinate function
            val = maths::sin(n * maths::cos(r) + 5 * theta); // Results in a value between -1 and 1
            //var val = cos(r);                            // Another simple function
            //var val = sin(theta);                        // Another simple function
            bw = int(((val + 1) * 255) / 2);
            int index = 4 * (i + j * width);

            canvasPixelData[index] = bw;
            canvasPixelData[index + 1] = bw;
            canvasPixelData[index + 2] = bw;
            canvasPixelData[index + 3] = bw;

            y += dy;
        }
        x += dx;
    }

    screenRefresh();
}
