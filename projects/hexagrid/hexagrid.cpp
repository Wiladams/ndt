#include "p5.hpp"

// https://eperezcosano.github.io/hex-grid/

int r;

void drawHexagon(IGraphics &ctx, float x, float y) 
{
    BLPath path;
    path.beginPath();

    for (int i = 0; i < 6; i++) {
        path.lineTo(x + r * maths::cos(a * i), y + r * maths::sin(a * i));
    }
    path.closePath();
    path.stroke();

    ctx.path(path);
}

void drawGrid(int width, int height) 
{
    int y = r;
    for (int x = r, j = 0; x + r * (1 + maths::cos(a)) < width; x += r * (1 + maths::cos(a)), y += (-1) * *j++ * r * maths::sin(a)) 
    {
        drawHexagon(x, y);
    }
}
