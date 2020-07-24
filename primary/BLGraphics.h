#pragma once

    //Binding P5 graphics API to a blend2d context
    // we don't know what the blend2d context is attached to
    // so we make no assumptions about the API, particularly in 
    // how to get raw bits in and out.



#include "Graphics.h"
#include <vector>


class BLGraphics : public IGraphics
{
    int fCommandCount = 0;  // How many commands since last flush
    int fCommandThreshold = 256;

    BLContext fCtx;
    BLImageData fImageData;

    int fAngleMode = RADIANS;
    ELLIPSEMODE fEllipseMode = ELLIPSEMODE::RADIUS;
    RECTMODE fRectMode = RECTMODE::CORNER;

    bool fUseFill = true;

    // Text Stuff
    BLFontFace fFontFace;
    BLFont fFont;
    float fFontSize = 12;
    ALIGNMENT fTextHAlignment = ALIGNMENT::LEFT;
    ALIGNMENT fTextVAlignment = ALIGNMENT::BASELINE;

    // Vertex shaping
    SHAPEMODE fShapeMode = SHAPEMODE::NONE;
    std::vector<BLPoint> fShapeVertices;

    Pixel fFillColor = BLRgba32(0, 0, 0, 255);
    Pixel fStrokeColor = BLRgba32(0, 0, 0, 255);
    Pixel fTextFillColor = BLRgba32(0, 0, 0, 255);

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

    static BLRect calcRectParams(RECTMODE mode, double& a, double& b, double& c, double& d)
    {
        double x = 0;
        double y = 0;
        double w = 0;
        double h = 0;

        switch (mode) {
        case RECTMODE::CORNER:
            x = a;
            y = b;
            w = c;
            h = d;
            break;
        case RECTMODE::CORNERS:
            x = a;
            y = b;
            w = c-a;
            h = d-b;
            break;
        case RECTMODE::CENTER:
            x = a - (c / 2);
            y = b - (d / 2);
            w = c;
            h = d;
            break;
        }

        return { x, y, w, h };
    }

    // Here we are crudely doing a minimal amount necessary
// to see some text.  We should be using FontMonger to cache
// font information so we're not reading it every time.
    BLPoint calcTextPosition(const char* txt, double x, double y)
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



    // Increment command count since last flush
// We track number of commands since last flush so that
// we can flush after a certain limit
    void incrCmd() {
        fCommandCount += 1;

        if (fCommandCount >= fCommandThreshold) {
            flush();
        }
    }
    // Reset command count to zero since last flush
    void resetCommandCount() { fCommandCount = 0; }

    // Create initial state
    void initialize()
    {
        // white fill
        fill(Pixel(255, 255, 255, 255));

        // black stroke
        stroke(Pixel(0, 0, 0, 255));

        // Start with a default font so we can start drawing text
        textFont("c:\\windows\\fonts\\segoeui.ttf");

    }


public:
    BLGraphics() 
        :fUseFill(true),
        fEllipseMode(ELLIPSEMODE::RADIUS),
        fRectMode(RECTMODE::CORNER),
        fAngleMode(RADIANS)
    {
        initialize();
    }

    BLGraphics(BLContext& ctx)
        : fCtx(ctx)
        ,fUseFill(true)
        ,fEllipseMode(ELLIPSEMODE::CENTER)
        ,fRectMode(RECTMODE::CORNER)
        ,fAngleMode(RADIANS)
    {
        initialize();
    }

    virtual ~BLGraphics() {
          // do nothing
    }

    BLContext& getBlend2dContext()
    {
        return fCtx;
    }

    // how many threads is the context using
    int threadCount() { return fCtx.queryThreadCount(); }

    void setCommandThreshold(int maxCommands)
    {
        fCommandThreshold = maxCommands;
    }

    // Various Modes
    virtual void angleMode(int mode) { fAngleMode = mode; }
    virtual void ellipseMode(const ELLIPSEMODE mode) {fEllipseMode = mode;}
    virtual void rectMode(const RECTMODE mode) { fRectMode = mode; }
    virtual void blendMode(int op) { fCtx.setCompOp(op); }

    // stroking attributes
    virtual void strokeCaps(int caps) { fCtx.setStrokeCaps(caps); }
    virtual void strokeJoin(int style) { fCtx.setStrokeJoin(style); }
    virtual void strokeWeight(double weight) { fCtx.setStrokeWidth(weight); }

    // Attribute State Stack
    virtual void push() { fCtx.save(); }
    virtual void pop() { fCtx.restore(); }

    // Coordinate transformation
    virtual void translate(double dx, double dy) { fCtx.translate(dx, dy); }
    virtual void scale(double sx, double sy) { fCtx.scale(sx, sy); }
    virtual void scale(double sxy) { scale(sxy, sxy); }
    virtual void rotate(double angle, double cx, double cy) { fCtx.rotate(angle, cx, cy); }
    virtual void rotate(double angle) { rotate(angle, 0, 0); }


    // Pixel management
    virtual void fill(const BLGradient& g) { fUseFill = true; fCtx.setFillStyle(g); }
    virtual void fill(const Pixel& c) { fUseFill = true; fCtx.setFillStyle(c); }
    virtual void noFill() { fCtx.setFillStyle(BLRgba32(0, 0, 0, 0)); fUseFill = false; }

    virtual void stroke(const Pixel& c) {
        fCtx.setStrokeStyle(c); 
    }
    virtual void noStroke() { fCtx.setStrokeStyle(BLRgba32(0, 0, 0, 0)); }


    // Synchronization
    // Wait for any outstanding drawing commands to be applied
    //
    virtual void flush()
    {
        BLResult bResult = fCtx.flush(BL_CONTEXT_FLUSH_SYNC);
        if (bResult != BL_SUCCESS)
        {
            std::cout << "BLGraphics.flush(), ERROR: " << bResult << std::endl;
        }

        resetCommandCount();
    }

    virtual void loadPixels()
    {
        // First, flush all queued commands
        flush();

        // The get a hold of the pixels from the image
        // if one exists
        BLImage* img = fCtx.targetImage();
        if (nullptr == img)
            return;

        // We have the image, so get the image data so 
        // we can get a pointer to the pixel data
        BLResult err = img->getData(&fImageData);
        if (BL_SUCCESS != err)
            return;
    }

    virtual void updatePixels()
    {
        flush();
        fImageData.reset();
    }

    // Background management
    virtual void clear() 
    {
        //printf("BLGraphics.clear\n");
        fCtx.save();
        fCtx.clearAll();
        fCtx.restore();
        flush();
    }

    virtual void clearRect(double x, double y, double w, double h)
    {
        fCtx.clearRect(x, y, w, h);
        incrCmd();
    }

    virtual void background(const Pixel& c)
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
    virtual void set(double x, double y, const Pixel& c)
    {
        if (nullptr == fImageData.pixelData)
            return;
        
        ((BLRgba32 *)(fImageData.pixelData))[(int)y * fImageData.size.w + (int)x] = c;
    }
    


    virtual void point(double x, double y) 
    { 
        line(x, y, x+1, y);
    }

    virtual void line(double x1, double y1, double x2, double y2) 
    {
        BLResult bResult = fCtx.strokeLine(x1, y1, x2, y2);

        incrCmd();
    }

    virtual void rect(const BLRect& rr)
    {
        //printf("BLGraphics.rect( %f %f %f %f);\n", rr.x, rr.y, rr.w, rr.h);
        BLResult bResult = fCtx.strokeRect(rr);
        bResult = fCtx.fillRect(rr);

        incrCmd();
    }

    virtual void rect(double x, double y, double width, double height, double xradius, double yradius)
    {
        //printf("BLGraphics.rrect( %f %f %f %f %f %f);\n", x, y, width, height, xradius, yradius);

        fCtx.fillRoundRect(x, y, width, height, xradius, yradius);
        fCtx.strokeRoundRect(x, y, width, height, xradius, yradius);

        incrCmd();
    }

    
    virtual void rect(double a, double b, double c, double d)
    {
            BLRect params = BLGraphics::calcRectParams(fRectMode, a, b, c, d);
            rect(params);
    }
    /*
    virtual void rect(double x, double y, double width, double height)
    {
        if (fUseFill) {
            fCtx.fillRect(x, y, width, height);
        }


            fCtx.strokeRect(x, y, width, height);


        incrCmd();
    }
    */
    virtual void ellipse(double a, double b, double c, double d) {
        BLEllipse params = BLGraphics::calcEllipseParams(fEllipseMode, a, b, c, d);

        if (fUseFill) {
            fCtx.fillEllipse(params);
        }

        fCtx.strokeEllipse(params);

        incrCmd();
    }

    virtual void circle(double cx, double cy, double diameter) { ellipse(cx, cy, diameter, diameter); }

    virtual void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        BLTriangle tri = { x1,y1, x2,y2,x3,y3 };

        if (fUseFill) {
            fCtx.fillGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);
        }


            fCtx.strokeGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);


        incrCmd();
    }

    virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        BLPath path;
        path.moveTo(x1, y1);
        path.cubicTo(x2, y2, x3, y3, x4, y4);
        fCtx.strokePath(path);

        incrCmd();
    }
    
    virtual void polyline(const BLPoint* pts, size_t n)
    {
        fCtx.strokePolyline(pts, n);
        incrCmd();
    }
    
    virtual void polygon(const BLPoint* pts, size_t n)
    {
        if (fUseFill) {
            fCtx.fillPolygon(pts, n);
        }

        fCtx.strokePolygon(pts, n);

        incrCmd();
    }
    
    virtual void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        BLPoint pts[] = { {x1,y1},{x2,y2},{x3,y3},{x4,y4} };
        polygon(pts, 4);
        incrCmd();
    }

    virtual void path(const BLPath& path)
    {
        if (fUseFill) {
            fCtx.fillPath(path);
        }
        fCtx.strokePath(path);

        incrCmd();
    }

    // Bitmaps
    virtual void image(const BLImage& img, int x, int y)
    {
        fCtx.blitImage(BLPointI(x, y), img);
        incrCmd();
    }
    
    virtual void scaleImage(const BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight)
    {
        BLRect dst{ dstX,dstY,dstWidth,dstHeight };
        BLRectI srcArea{ (int)srcX,(int)srcY,(int)srcWidth,(int)srcHeight };

        fCtx.blitImage(dst, src, srcArea);
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
        
        
        fCtx.strokeUtf8Text(xy, fFont, txt);

        // there are proably a whole lot more 'commands'
        // generated when rendering text, but we have no 
        // way of knowing how many.
        incrCmd();
    }


    // Vertex shaping
    void beginShape(SHAPEMODE shapeKind = SHAPEMODE::OPEN)
    {
        // To begin a shape, clear out existing shape
        // and set the shape mode to that specified
        fShapeMode = shapeKind;
        fShapeVertices.clear();
    }

    void vertex(double x, double y)
    {
        // Make sure beginShape() has been called
        if (fShapeMode == SHAPEMODE::NONE)
            return;

        fShapeVertices.push_back({ x,y });
    }

    // endKind - SHAPEEND
    void endShape(SHAPEEND endKind)
    {
        // Sanity check to ensure we're in a shape
        if (fShapeMode == SHAPEMODE::NONE)
            return;

        push();

        switch (fShapeMode) {
        case SHAPEMODE::OPEN: {
            BLPath path;
            if (endKind == SHAPEEND::CLOSE) {
                polygon(fShapeVertices.data(), fShapeVertices.size());
            } else if (endKind == SHAPEEND::STROKE) {
                if (fUseFill) {
                    fCtx.fillPolygon(fShapeVertices.data(), fShapeVertices.size());
                }

                polyline(fShapeVertices.data(), fShapeVertices.size());
            }
            break; 
        }
        case SHAPEMODE::POINTS: {
            for (size_t i = 1; i <= fShapeVertices.size(); i++) {
                point(fShapeVertices[i - 1].x, fShapeVertices[i - 1].y);
            }
        }
            break;
        case SHAPEMODE::LINES: {
            for (size_t i = 0; i < fShapeVertices.size(); i += 2) {
                BLPoint p1 = fShapeVertices[i];
                BLPoint p2 = fShapeVertices[i + 1];
                line(p1.x, p1.y, p2.x, p2.y);
            }
        }
            break;
        case SHAPEMODE::TRIANGLES: {
            // consume 3 points at a time doing triangles
            for (size_t i = 0; i < fShapeVertices.size(); i += 3) {
                BLPoint p1 = fShapeVertices[i];
                BLPoint p2 = fShapeVertices[i + 1];
                BLPoint p3 = fShapeVertices[i + 2];
                triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
            }
        }
            break;

        case SHAPEMODE::TRIANGLE_STRIP: {
            // consume 3 points at a time doing triangles
            for (size_t i = 0; i < fShapeVertices.size() - 2; i += 1) {
                BLPoint p0 = fShapeVertices[i + 0];
                BLPoint p1 = fShapeVertices[i + 1];
                BLPoint p2 = fShapeVertices[i + 2];
                triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
            }
        }
            break;

        case SHAPEMODE::TRIANGLE_FAN: {
            // consume 3 points at a time doing triangles
            // get the first point
            BLPoint p0 = fShapeVertices[0];
            for (size_t i = 1; i < fShapeVertices.size() - 1; i += 1) {
                BLPoint p1 = fShapeVertices[i];
                BLPoint p2 = fShapeVertices[i + 1];
                triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
            }
        }
            break;

        case SHAPEMODE::QUADS: {
            // consume 4 points at a time doing quads
            for (size_t i = 0; i < fShapeVertices.size(); i += 4) {
                BLPoint p1 = fShapeVertices[i];
                BLPoint p2 = fShapeVertices[i + 1];
                BLPoint p3 = fShapeVertices[i + 2];
                BLPoint p4 = fShapeVertices[i + 3];
                quad(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y);
            }
        }
            break;

        case SHAPEMODE::QUAD_STRIP: {
            // consume 4 points at a time doing quads
            for (int i = 0; i < fShapeVertices.size() - 2; i += 2) {
                BLPoint p1 = fShapeVertices[i + 0];
                BLPoint p2 = fShapeVertices[i + 1];
                BLPoint p3 = fShapeVertices[i + 2];
                BLPoint p4 = fShapeVertices[i + 3];
                quad(p1.x, p1.y, p2.x, p2.y, p4.x, p4.y, p3.x, p3.y);
            }
        }
        break;

        }

        pop();
    }
};
