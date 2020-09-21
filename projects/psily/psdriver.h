#pragma once

/*
    // device independent state
    CTM         array       current transformation matrix
    position        two numbers     Coordinates of current point
    path            (internal)  current path being built
    clipping path   (internal)  path defining current boundary
    clipping path stack
    color space
    color
    font
    line width
    line cap
    miter limit
    dash pattern
    stroke adjustment
*/
struct PSState {
    
    // path
    // clipping path
    // clipping path stack
    double fWidth;

};

class PSDriver {
public:
    void setFont();
    void gSave();
    void gRestore();
    void getCTM();
    void clipPath();
    void setLineWidth();
    void setLineCap();
    void setLineJoin();

    void setMiterLimit();
    void currentMiterLimit();
    void setStrokeAdjust();
    void currentStrokeAdjust();
    void setDash();
    void currentDash();
    void setColorspace();
    void currentColorspace();

    void setGraya();
    void setRgbaColor();

    //void sethsbcolor();
    //void currenthsbcolor();
    //void setrgbcolor();
    //void currentrgbcolor();
    //void setcmykcolor();
    //void currentcmykcolor();

    void applyCTM();
    void concat();
    void transformPoint();

    // void matrix();
    // void initmatrix();
    // void identmatrix();
    // void defaultmatrix();

    void currentmatrix();
    void setMatrix();
    void translate();
    void scale();
    void rotateBy();

    // Path construction
    void setPath();
    void newPath();
    void pathBox();
    void getCurrentPosition();
    void moveTo(const double x, const double y)
    {

    }

    void lineTo();
    void arc();
    void arn();
    void arct();
    void curveTo();
    void closePath();

    // Painting operators
    void erasepage();
    void newFigure();
    void fill();
    void rectFill();
    void rectStroke();
    void stroke();

    // Device Setup
    void showPage();

    // Font operators
    void stringWidth();
    void charPath();
    void show();
};
