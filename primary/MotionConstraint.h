#pragma once

#include "maths.hpp"


struct MotionConstraint
{
    float fminX;
    float fminY;
    float fmaxX;
    float fmaxY;

public:
    MotionConstraint()
        :fminX(0)
        , fminY(0)
        , fmaxX(0)
        , fmaxY(0)
    {}

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
    BLPoint tryChange(const BLRect &subject, const BLPoint&change)
    {
        //printf("tryChange: ", change.dx, change.dy)
        float x = maths::Clamp((double)subject.x + change.x, fminX, fmaxX);
        float y = maths::Clamp((double)subject.y + change.y, fminY, fmaxY);

        //print("tryChange, 2.0: ", x, y)

        float dx = x - subject.x;
        float dy = y - subject.y;

        return { dx,dy };
    }

    //
    // Calculate a position of a point within
    // the context of our constrained range
    BLPoint calcPosition(const BLRect&frame)
    {
        float xpos = maths::Clamp(maths::Map(frame.x, fminX, fmaxX, 0, 1), 0,1);
        float ypos = maths::Clamp(maths::Map(frame.y, fminY, fmaxY, 0, 1), 0,1);

        return { xpos, ypos };
    }

};