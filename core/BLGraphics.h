#pragma once

    //Binding P5 graphics API to a blend2d context
    // we don't know what the blend2d context is attached to
    // so we make no assumptions about the API, particularly in 
    // how to get raw bits in and out.



#include "Graphics.h"


class BLGraphics : public IGraphics
{
private:
    static BLEllipse calcEllipseParams(ELLIPSEMODE mode, double& a, double& b, double& c, double& d)
    {
        double cx = 0;
        double cy = 0;
        double rx = 0;
        double ry = 0;

        switch (mode) {
        case ELLIPSEMODE::CORNER:
            rx = c / 2;
            ry = d / 2;
            cx = a + rx;
            cy = b + ry;
            break;
        case ELLIPSEMODE::CORNERS:
            rx = (c - a) / 2;
            ry = (d - b) / 2;
            cx = a + rx;
            cy = b + ry;
            break;
        case ELLIPSEMODE::CENTER:
            rx = c / 2;
            ry = d / 2;
            cx = a;
            cy = b;
            break;
        case ELLIPSEMODE::RADIUS:
            cx = a;
            cy = b;
            rx = c;
            ry = d;
        }

        return { cx, cy, rx, ry };
    }


    BLContext fCtx;
    int fAngleMode;
    ELLIPSEMODE fEllipseMode = ELLIPSEMODE::RADIUS;
    RECTMODE fRectMode = RECTMODE::CORNER;

    bool fUseFill = true;
    bool fUseStroke = true;

    // Text Stuff
    BLFontFace fFontFace;
    BLFont fFont;
    float fFontSize = 12;
    ALIGNMENT fTextHAlignment = ALIGNMENT::LEFT;
    ALIGNMENT fTextVAlignment = ALIGNMENT::BASELINE;

public:
    BLGraphics() 
        :fUseFill(true),
        fUseStroke(true),
        fEllipseMode(ELLIPSEMODE::RADIUS),
        fRectMode(RECTMODE::CORNER),
        fAngleMode(RADIANS)
    {}

    BLGraphics(BLContext& ctx)
        : fCtx(ctx)
        ,fUseFill(true)
        ,fUseStroke(true)
        ,fEllipseMode(ELLIPSEMODE::CENTER)
        ,fRectMode(RECTMODE::CORNER)
        ,fAngleMode(RADIANS)
    {
    }

    BLContext& getBlend2dContext()
    {
        return fCtx;
    }

    // Various Modes
    virtual void angleMode(int mode) { fAngleMode = mode; }
    virtual void ellipseMode(const ELLIPSEMODE mode) {fEllipseMode = mode;}
    virtual void rectMode(const RECTMODE mode) { fRectMode = mode; }
    virtual void blendMode(int op) { fCtx.setCompOp(op); }

    // stroking attributes
    virtual void strokeCaps(int caps) { fCtx.setStrokeCaps(caps); }
    virtual void strokeJoin(int style) { fCtx.setStrokeJoin(style); }
    virtual void strokeWeight(int weight) { fCtx.setStrokeWidth(weight); }

    // Attribute State Stack
    virtual void push() { fCtx.save(); }
    virtual void pop() { fCtx.restore(); }

    // Coordinate transformation
    virtual void translate(double dx, double dy) { fCtx.translate(dx, dy); }
    virtual void scale(double sx, double sy) { fCtx.scale(sx, sy); }
    virtual void scale(double sxy) { scale(sxy, sxy); }
    virtual void rotate(double angle, double cx, double cy) { fCtx.rotate(angle, cx, cy); }
    virtual void rotate(double angle) { rotate(angle, 0, 0); }


    // Color management
    virtual void fill(const BLGradient& g) { fUseFill = true; fCtx.setFillStyle(g); }
    virtual void fill(const Color& c) { fUseFill = true; fCtx.setFillStyle(c); }
    virtual void noFill() { fUseFill = false; }

    virtual void stroke(const Color& c) { fUseStroke = true; fCtx.setStrokeStyle(c); }
    virtual void noStroke() { fUseStroke = false; }


    // Synchronization
    virtual void flush()
    {
        fCtx.flush(BL_CONTEXT_FLUSH_SYNC);
    }

    virtual void loadPixels()
    {
        flush();
    }

    virtual void updatePixels()
    {
        flush();
    }

    // Background management
    virtual void clear() 
    {
        fCtx.save();
        fCtx.clearAll();
        fCtx.restore();
        flush();
    }

    virtual void background(const Color& c)
    {
        fCtx.save();
        fCtx.setFillStyle(c);
        fCtx.fillAll();
        fCtx.restore();
    }

    // Clipping
    virtual void clip(double x, double y, double w, double h) {fCtx.clipToRect(x, y, w, h);}
    virtual void noClip() { fCtx.restoreClipping(); }

    // Geometry
    // hard set a specfic pixel value
    virtual void set(double x, double y, const Color& c)
    {
        push();
        fill(c);
        rect(x, y, 1, 1);
        pop();
    }
    


    virtual void point(double x, double y) { set(x, y, color(0)); }
    virtual void line(double x1, double y1, double x2, double y2) {
        if (!fUseStroke)
        {
            return;
        }

        fCtx.strokeLine(x1, y1, x2, y2);
    }

    virtual void rect(const BLRect& arect)
    {
        if (fUseFill)
            fCtx.fillRect(arect);

        if (fUseStroke)
            fCtx.strokeRect(arect);
    }

    virtual void rect(double x, double y, double width, double height, double xradius, double yradius)
    {
        if (fUseFill) {
            fCtx.fillRoundRect(x, y, width, height, xradius, yradius);
        }

        if (fUseStroke) {
            fCtx.strokeRoundRect(x, y, width, height, xradius, yradius);
        }
    }

    virtual void rect(double x, double y, double width, double height)
    {
        if (fUseFill) {
            fCtx.fillRect(x, y, width, height);
        }

        if (fUseStroke) {
            fCtx.strokeRect(x, y, width, height);
        }
    }

    virtual void ellipse(double a, double b, double c, double d) {
        BLEllipse params = BLGraphics::calcEllipseParams(fEllipseMode, a, b, c, d);

        if (fUseFill) {
            fCtx.fillEllipse(params);
        }

        if (fUseStroke) {
            fCtx.strokeEllipse(params);
        }
    }

    virtual void circle(double cx, double cy, double diameter) { ellipse(cx, cy, diameter, diameter); }

    virtual void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        BLTriangle tri = { x1,y1, x1,y2,x3,y3 };

        if (fUseFill) {
            fCtx.fillGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);
        }

        if (fUseStroke) {
            fCtx.strokeGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);
        }
    }

    virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        BLPath path;
        path.moveTo(x1, y1);
        path.cubicTo(x2, y2, x2, y3, x4, y4);
        fCtx.strokePath(path);
    }
    
    virtual void polyline(const BLPoint* pts, size_t n)
    {
        fCtx.strokePolyline(pts, n);
    }
    
    virtual void polygon(const BLPoint* pts, size_t n)
    {
        if (fUseFill) {
            fCtx.fillPolygon(pts, n);
        }

        if (fUseStroke) {
            fCtx.strokePolygon(pts, n);
        }
    }
    
    virtual void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        BLPoint pts[] = { {x1,y1},{x2,y2},{x3,y3},{x4,y4} };
        polygon(pts, 4);
    }

    // Bitmaps
    virtual void image(BLImage& img, int x, int y)
    {
        fCtx.blitImage(BLPointI(x, y), img);
    }
    
    virtual void scaleImage(BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight)
    {
        BLRect dst{ dstX,dstY,dstWidth,dstHeight };
        BLRectI srcArea{ (int)srcX,(int)srcY,(int)srcWidth,(int)srcHeight };

        fCtx.blitImage(dst, src, srcArea);
    }

    // Here we are crudely doing a minimal amount necessary
    // to see some text.  We should be using FontMonger to cache
    // font information so we're not reading it every time.
    BLPoint calcTextPosition(const char *txt, double x, double y)
    {
        BLPoint txtSize = textWidth(txt);
        double cx = txtSize.x;
        double cy = txtSize.y;

        switch (fTextHAlignment)
        {
                case ALIGNMENT::LEFT:
                    x = x;
                    break;
                case ALIGNMENT::CENTER:
                    x = x - (cx / 2);
                    break;
                case ALIGNMENT::RIGHT:
                    x = x - cx;
                    break;
        }

        switch (fTextVAlignment)
        {
        case ALIGNMENT::TOP:
            y = y + cy;
            break;
        case ALIGNMENT::CENTER:
            y = y + (cy / 2);
            break;
        case ALIGNMENT::BASELINE:
            y = y;
            break;
        case ALIGNMENT::BOTTOM:
            y = y;
            break;
        }

        return BLPoint(x, y);
    }

    virtual void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical)
    {
        fTextHAlignment = horizontal;
        fTextVAlignment = vertical;
    }

    virtual void textFont(const char* fontfile)
    {
        fFontFace.createFromFile(fontfile);
        textSize(fFontSize);
    }
    
    virtual void textSize(double size)
    {
        fFontSize = (float)size;
        fFont.reset();
        fFont.createFromFace(fFontFace, fFontSize);
    }

    // Get the width (in font units) of the text
    virtual BLPoint textWidth(const char* txt)
    {
        BLFontMetrics fm = fFont.metrics();
        BLTextMetrics tm;
        BLGlyphBuffer gb;
        
        gb.setUtf8Text(txt);
        fFont.shape(gb);
        fFont.getTextMetrics(gb, tm);

        double cx = tm.boundingBox.x1 - tm.boundingBox.x0;
        double cy = fFont.size();


        return BLPoint(cx, cy);
    }

    void text(const char* txt, double x, double y)
    {
        BLPoint xy = calcTextPosition(txt, x, y);
        //printf("text: (%3.2f,%3.2f) => (%3.2f,%3.2f)\n", x, y, xy.x, xy.y);

        fCtx.fillUtf8Text(xy, fFont, txt);
    }

};
