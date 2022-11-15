//
// Reference
//  https://p5js.org/examples/math-graphing-2d-equations.html
//

#include "apphost.h"


void onLoad() 
{
    createAppWindow(710, 400, "graph2d");
}

void onLoop() 
{
    auto n = (mouseX * 10.0f) / canvasWidth;
    const float w = 16.0f; // 2D space width
    const float h = 16.0f; // 2D space height
    const float dx = w / canvasWidth; // Increment x this amount per pixel
    const float dy = h / canvasHeight; // Increment y this amount per pixel
    auto x = -w / 2; // Start x at -1 * width / 2
    float y;

    float r;
    float theta;
    float val;

    int bw; //variable to store grayscale
    int i;
    int j;
    int cols = canvasWidth;
    int rows = canvasHeight;

    for (i = 0; i < cols; i += 1) {
        y = -h / 2;
        for (j = 0; j < rows; j += 1) {
            r = maths::sqrt(x * x + y * y); // Convert cartesian to polar
            theta = maths::atan2(y, x); // Convert cartesian to polar
            // Compute 2D polar coordinate function
            //val = maths::sin(n * maths::cos(r) + 5 * theta); // Results in a value between -1 and 1
            val = maths::cos(r);                            // Another simple function
            //val = maths::sin(r);
            //val = maths::sin(theta);                        // Another simple function
            bw = int(((val + 1) * 255) / 2);
            int index = 4 * (i + j * canvasWidth);

            gAppFrameBuffer.setPixel(i,j,Pixel(bw,bw,bw,255));

            y += dy;
        }
        x += dx;
    }

    screenRefresh();
}
