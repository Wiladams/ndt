#pragma once

#include "maths.hpp"
#include "geometry.h"

class MotionConstraint
{
    float fminX;
    float fminY;
    float fmaxX;
    float fmaxY;

public:
    MotionConstraint(float minX, float minY, float maxX, float maxY)
    {
        fminX = minX;
        fminY = minY;
        fmaxX = maxX;
        fmaxY = maxY;
    }

    //
    // Given a subject, which is already within a certain range
    // try to apply a change to the position, represented
    // by the 'change' parameter
    Vec2f tryChange(const Vec2f &subject, const Vec2f &change)
    {
        //printf("tryChange: ", change.dx, change.dy)
        float x = constrain((double)subject.x + change.x, fminX, fmaxX);
        float y = constrain((double)subject.y + change.y, fminY, fmaxY);

        //print("tryChange, 2.0: ", x, y)

        float dx = x - subject.x;
        float dy = y - subject.y;

        return { dx,dy };
    }

    //
    // Calculate a position of a point within
    // the context of our constrained range
    Vec2f calcPosition(const Vec2f &frame)
    {
        float xpos = map(frame.x, fminX, fmaxX, 0, 1, true);
        float ypos = map(frame.y, fminY, fmaxY, 0, 1, true);
        //printf("MotionConstraint.calcPosition: ", xpos, ypos, frame.x, self.minX, self.maxX)

        return { xpos, ypos };
    }

};