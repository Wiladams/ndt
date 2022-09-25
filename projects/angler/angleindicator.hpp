#pragma once

#include "graphic.hpp"



struct AngleIndicator : public Graphic
{
    static void getPreferredSize(int& x, int& y) { x = 200; y = 200; }

    float fAngle;
    float fCenterX;
    float fCenterY;

    AngleIndicator(float rad = 0.0f)
        : Graphic(0, 0, 200, 200)
        , fAngle(rad)
    {
        fCenterX = width() / 2.0;
        fCenterY = height() / 2.0;
    }

    void setAngle(float ang)
    {
        fAngle = ang;
    }

    void drawBackground(IGraphics& ctx)
    {
        //ctx.push();

        //fill background
        ctx.ellipseMode(ELLIPSEMODE::RADIUS);
        ctx.stroke(255);
        ctx.strokeWeight(2);
        ctx.fill(32, 32, 32);
        ctx.circle(fCenterX, fCenterY, width() / 2.0);

        //ctx.pop();
    }

    void drawForeground(IGraphics& ctx)
    {
        ctx.push();

        //Do the rotation thing
        ctx.translate(fCenterX, fCenterY);
        ctx.rotate(fAngle);

        //Draw the indicator line
        ctx.strokeWeight(1);
        ctx.stroke(255, 0, 0);
        ctx.line(0, 0, fCenterX, 0);

        ctx.pop();
    }
};









