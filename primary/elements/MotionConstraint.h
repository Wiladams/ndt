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
    maths::vec2f tryChange(const maths::bbox2f &subject, const maths::vec2f&change)
    {
        //printf("tryChange: ", change.dx, change.dy)
        float x = maths::clamp(float(subject.min.x + change.x), fminX, fmaxX);
        float y = maths::clamp((float)(subject.min.y + change.y), fminY, fmaxY);

        //printf("tryChange, 2.0: %f %f\n", x, y);

        float dx = x - subject.min.x;
        float dy = y - subject.min.y;

        return { dx,dy };
    }

    //
    // Calculate a position of a point within
    // the context of our constrained range
    maths::vec2f calcPosition(const maths::bbox2f&fr)
    {
        float xpos = maths::clamp(maths::map(float(fr.min.x), fminX, fmaxX, 0.0f, 1.0f), 0.0f,1.0f);
        float ypos = maths::clamp(maths::map(float(fr.min.y), fminY, fmaxY, 0.0f, 1.0f), 0.0f,1.0f);

        return { xpos, ypos };
    }

};