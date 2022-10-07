//
// Reference
//  https://p5js.org/examples/math-graphing-2d-equations.html
//
#include "p5.hpp"

using namespace p5;

int width = 0;
int height = 0;

void setup() 
{
    createCanvas(710, 400, "graph2d");
    //pixelDensity(1);
    frameRate(20);

    width = canvasWidth;
    height = canvasHeight;
}

void draw() 
{
    loadPixels();

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

    Pixel bw; //variable to store grayscale
    int i;
    int j;
    int cols = width;
    int rows = height;

    for (i = 0; i < cols; i += 1) {
        y = -h / 2;
        for (j = 0; j < rows; j += 1) {
            r = p5::sqrt(x * x + y * y); // Convert cartesian to polar
            theta = maths::atan2(y, x); // Convert cartesian to polar
            // Compute 2D polar coordinate function
            val = sin(n * cos(r) + 5 * theta); // Results in a value between -1 and 1
            //var val = cos(r);                            // Another simple function
            //var val = sin(theta);                        // Another simple function
            bw = color(((val + 1) * 255) / 2);
            int index = 4 * (i + j * width);
            pixels[index] = red(bw);
            pixels[index + 1] = green(bw);
            pixels[index + 2] = blue(bw);
            pixels[index + 3] = alpha(bw);

            y += dy;
        }
        x += dx;
    }
    updatePixels();
}
