#pragma once

#include "maths.hpp"
#include "geometry.h"

class MotionConstraint
{
    double fminX;
    double fminY;
    double fmaxX;
    double fmaxY;

public:
    MotionConstraint(double minX, double minY, double maxX, double maxY)
    {
        fminX = minX;
        fminY = minY;
        fmaxX = maxX;
        fmaxY = maxY;
    }

    Vec2f tryChange(Vec2f subject, Vec2f change)
    {
        //printf("tryChange: ", change.dx, change.dy)
        double x = constrain(subject.x + change.x, fminX, fmaxX);
        double y = constrain(subject.y + change.y, fminY, fmaxY);

        //print("tryChange, 2.0: ", x, y)

        double dx = x - subject.x;
        double dy = y - subject.y;

        return { dx,dy };
    }

    Vec2f calcPosition(const Vec3f &frame)
    {
        double xpos = map(frame.x, fminX, fmaxX, 0, 1, true);
        double ypos = map(frame.y, fminY, fmaxY, 0, 1, true);
        //printf("MotionConstraint.calcPosition: ", xpos, ypos, frame.x, self.minX, self.maxX)

        return { xpos, ypos };
    }

};