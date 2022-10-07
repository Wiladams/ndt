#pragma once

#include "maths.hpp"


struct MotionConstraint
{
    float fminX=0;
    float fminY=0;
    float fmaxX=0;
    float fmaxY=0;

public:
    MotionConstraint() = default;


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
    maths::vec2f tryChange(const BLRect &subject, const maths::vec2f&change)
    {
        //printf("tryChange: ", change.dx, change.dy)
        float x = maths::clamp(float(subject.x + change.x), fminX, fmaxX);
        float y = maths::clamp((float)(subject.y + change.y), fminY, fmaxY);

        //printf("tryChange, 2.0: %f %f\n", x, y);

        float dx = x - subject.x;
        float dy = y - subject.y;

        return { dx,dy };
    }

    //
    // Calculate a position of a point within
    // the context of our constrained range
    maths::vec2f calcPosition(const BLRect&frame)
    {
        float xpos = maths::clamp(maths::map(frame.x, fminX, fmaxX, 0.0, 1.0), 0.0,1.0);
        float ypos = maths::clamp(maths::map(frame.y, fminY, fmaxY, 0.0, 1.0), 0.0,1.0);

        return { xpos, ypos };
    }

};