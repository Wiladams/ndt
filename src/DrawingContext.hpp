#pragma once

#include "grtypes.hpp"
#include "PixelBuffer.hpp"
#include "PixelOps.hpp"

#include <math.h>


/*
    These sit here so they are not class members
*/
typedef void(* EllipseHandler)(PixelBuffer &pb, GRCOORD cx, GRCOORD cy, GRCOORD x, GRCOORD y, const PixRGBA color, const PixelTransferOp &tOp);

void Plot4EllipsePoints(PixelBuffer &pb, GRCOORD cx, GRCOORD cy, GRCOORD x, GRCOORD y, const PixRGBA color, const PixelTransferOp &tOp)
{
    pb.transferPixel(cx + x, cy + y, color, tOp);
    pb.transferPixel(cx - x, cy + y, color, tOp);
    pb.transferPixel(cx - x, cy - y, color, tOp);
    pb.transferPixel(cx + x, cy - y, color, tOp);
}

void fill2EllipseLines(PixelBuffer &pb, GRCOORD cx, GRCOORD cy, GRCOORD x, GRCOORD y, const PixRGBA color, const PixelTransferOp &tOp)
{
	int x1 = cx - x;
	int y1 = cy+y;
	int x2 = cx + x;
	int y2 = cy+y;

	//if (clipLine(pb->frame, x1, y1, x2, y2)) {
        pb.horizontalLine(x1, y1, x2-x1, color, tOp);
	//}
	
	y1 = cy - y;
	y2 = cy - y;
	//if (clipLine(pb->frame, x1, y1, x2, y2)) {
		pb.horizontalLine(x1, y2, x2 - x1, color, tOp);
	//}
}



class DrawingContext {

private:
    PixelBuffer &pb;        // The pixel buffer we will be drawing into
    PixelTransferOp *tOp;

    PixRGBA strokePix;      // pixel color for stroking
    PixRGBA fillPix;        // pixel color for filling
    PixRGBA bgPix;          // pixel color for background

    PixRGBA *scratch;       // A single row of pixels for various speedups

    // We don't want these to be constructed using
    // a default constructor, so make it private
    DrawingContext();

public:
    DrawingContext(PixelBuffer &pb)
    :pb(pb), 
    strokePix(0xff000000), 
    fillPix(0xffffffff),
    bgPix(0xff707070),
    tOp(nullptr)
    {
        tOp = new PixelSRCOVER();

        // create a scratch row for better optimization
        // of copy operators
        scratch = {new PixRGBA[pb.getWidth()]{}};
    }

    virtual ~DrawingContext()
    {
        delete [] scratch;
    }

    void setTransferOp(PixelTransferOp *newOp)
    {
        if (tOp != nullptr) {
            delete tOp;
        }

        this->tOp = newOp;
    }

    bool setBackground(const PixRGBA pix)
    {
        bgPix = pix;
        return true;
    }

    bool setFill(const PixRGBA pix)
    {
        fillPix = pix;
        
        // Fill up a single row of pixels
        for (GRSIZE idx=0;idx < pb.getWidth();idx++)
        {
            this->scratch[idx] = pix;
        }

        return true;
    }

    bool setStroke(const PixRGBA pix)
    {
        strokePix = pix;
        return true;
    }

    // Clear the canvas to all transparent pixels
    bool clear()
    {
        PixRGBA pix = {0x00000000};
        pb.setAllPixels(pix);
        return true;
    }

    // clear the canvas to the background pixel
    bool clearToBackground()
    {
        pb.setAllPixels(bgPix);
        return true;
    }

    // Uses fill color
    // Should be able to apply a drawing operator
    bool setPixel(GRCOORD x, GRCOORD y, PixRGBA c)
    {
        pb.transferPixel(x, y,c, *tOp);
        return true;
    }

    bool fillPixel(GRCOORD x, GRCOORD y)
    {
        pb.transferPixel(x, y, fillPix, *tOp);
        return true;
    }

    bool strokeHorizontalLine(GRCOORD x, GRCOORD y, GRSIZE width)
    {
        // BUGBUG - should sanity check line fits
        // within bounds
        GRCOORD idx = x;
        while (idx < x+width)
        {
            pb.transferPixel(idx, y, strokePix, *tOp);
            idx = idx + 1;
        }

        return true;
    }

    bool fillHorizontalLine(GRCOORD x, GRCOORD y, GRSIZE width)
    {
        // BUGBUG - should sanity check line fits
        // within bounds
        GRCOORD idx = x;
        while (idx < x+width)
        {
            pb.transferPixel(idx, y, fillPix, *tOp);
            idx = idx + 1;
        }

        return true;
    }

    bool strokeVerticalLine(GRCOORD x, GRCOORD y, GRSIZE length)
    {
        // BUGBUG - should sanity check line fits
        // within bounds
        GRCOORD idx = y;
        while (idx < y+length) {
            this->pb.transferPixel(x, idx, this->strokePix, *tOp);
            idx = idx + 1;
        }
        return true;
    }

    /*
    strokeLine()

    Stroke a line using the current stroking pixel.
    Uses Bresenham line drawing.  Does not check for clipping,
    but the underlying PixelBuffer might.

    Note: an easy optimization would be to use the specialized
    horizontal and vertical line drawing routines when necessary

    BUGBUG - should sanity check line fits
    within bounds
    */
    bool strokeLine(GRCOORD x1, GRCOORD y1, GRCOORD x2, GRCOORD y2)
    {
        /*
        // BUGBUG
        // Specialize for vertical and horizontal lines
        if (x1 == x2) {
            // need to figure out the proper ordering
            // of y2 and y1
            strokeVerticalLine(x1, y1, y2-y1);
        }
        */

	    int sdx, sdy, dxabs, dyabs;
	    unsigned int x, y, px, py;

        int dx = x2 - x1;      // the horizontal distance of the line
        int dy = y2 - y1;      // the vertical distance of the line
        dxabs = abs(dx);
        dyabs = abs(dy);
        sdx = sgn(dx);
        sdy = sgn(dy);
        x = dyabs >> 1;
        y = dxabs >> 1;
        px = x1;
        py = y1;

        this->pb.transferPixel(x1, y1, this->strokePix, *tOp);

        if (dxabs >= dyabs) // the line is more horizontal than vertical
        {
            for (size_t i = 0; i<dxabs; i++)
            {
                y = y + dyabs;
                if (y >= (unsigned int)dxabs)
                {
                    y = y - dxabs;
                    py = py + sdy;
                }
                px = px + sdx;
                this->pb.transferPixel(px, py, this->strokePix, *tOp);
            }
        } else // the line is more vertical than horizontal
        {
            for (size_t i = 0; i<dyabs; i++)
            {
                x += dxabs;
                if (x >= (unsigned int)dyabs)
                {
                    x -= dyabs;
                    px += sdx;
                }
                py += sdy;
                this->pb.transferPixel(px, py, this->strokePix, *tOp);
            }
        }

        return true;
    }


    // drawPolyLine(int nPts, Point2D *pts)
    bool strokePolyLine(int nPts, Point2D *pts)
    {
        // draw the lines
        for (int i=1; i<nPts; i++) {
            strokeLine(pts[i-1].x, pts[i-1].y, pts[i].x, pts[i].y);
        }

        return true;
    }

    // Cubic bezier basis functions
    // the 'u' value ranges from 0.0 to 1.0
    double BEZ03(double u) {return powf(1-u, 3); }            // (1-u)^3
    double BEZ13(double u) {return 3 * u*((1-u) * (1-u));}    // 3u(1-u)^2
    double BEZ23(double u) {return 3 * u*u * (1-u);}          // 3u^2(1-u)
    double BEZ33(double u) {return u * u*u;}                  // u^3

    bool strokeCubicBezier(
        GRCOORD x1, GRCOORD y1, 
        GRCOORD x2, GRCOORD y2, 
        GRCOORD x3, GRCOORD y3, 
        GRCOORD x4, GRCOORD y4)
    {

    }


    bool strokeRectangle(GRCOORD x, GRCOORD y, GRSIZE width, GRSIZE height)
    {
        // Draw Horizontal Lines
        strokeHorizontalLine(x, y ,width);
        strokeHorizontalLine(x, y+height-1, width);

        // Draw Vertical lines
        strokeVerticalLine(x,y,height);
        strokeVerticalLine(x+width-1, y, height);

        return true;
    }

    bool fillRectangle(GRCOORD x, GRCOORD y, GRSIZE width, GRSIZE height)
    {
        // If no alpha, then don't fill in
        if (fillPix.alpha == 0){
            return true;
        }

        for (GRSIZE row = 0; row<height; row++)
        {
            fillHorizontalLine(x, y+row, width);
        }

        return true;
    }
    
    bool drawRectangle(GRCOORD x, GRCOORD y, GRSIZE width, GRSIZE height)
    {
        // fill rectangle
        fillRectangle(x, y, width, height);

        // stroke rectangle
        strokeRectangle(x, y, width, height);
        
        return true;
    }

    // drawRoundedRectangle()
    // strokeRoundedRectangle()
    // fillRoundedRectangle()

/*
        Ellipse drawing
*/

   //The drive_ellipse() function takes care of calculating ellipse
   // edge coordinates.  It then applies the handler function to each
   // of the pairs to either draw edge strokes or fills
    void drive_ellipse(GRCOORD cx, GRCOORD cy, size_t xradius, size_t yradius, const PixRGBA color, EllipseHandler handler)
    {
        int twoasquare = 2 * xradius*xradius;
        int twobsquare = 2 * yradius*yradius;

	    int x = xradius;
	    int y = 0;

        int xchange = yradius*yradius*(1 - 2 * xradius);
        int ychange = xradius*xradius;
        int ellipseerror = 0;
        int stoppingx = twobsquare*xradius;
        int stoppingy = 0;

        // first set of points, sides
        while (stoppingx >= stoppingy)
        {
            handler(pb, cx, cy, x, y, color, *tOp);
            y = y + 1;
            stoppingy = stoppingy + twoasquare;
            ellipseerror += ychange;
            ychange += twoasquare;

            if ((2 * ellipseerror + xchange) > 0) {
                x--;
                stoppingx -= twobsquare;
                ellipseerror += xchange;
                xchange += twobsquare;
            }
        }

        // second set of points, top and bottom
        x = 0;
        y = yradius;
        xchange = yradius*yradius;
        ychange = xradius*xradius*(1 - 2 * yradius);
        ellipseerror = 0;
        stoppingx = 0;
        stoppingy = twoasquare*yradius;

        while (stoppingx <= stoppingy) {
            handler(pb, cx, cy, x, y, color, *tOp);
            x = x + 1;
            stoppingx = stoppingx + twobsquare;
            ellipseerror = ellipseerror + xchange;
            xchange = xchange + twobsquare;
            
            if ((2 * ellipseerror + ychange) > 0) {
                y = y - 1;
                stoppingy -= twoasquare;
                ellipseerror += ychange;
                ychange += twoasquare;
            }
        }
    }

    // strokeEllipse()
    // strokeEllipse with current stroke color
    bool strokeEllipse(GRCOORD cx, GRCOORD cy, size_t xradius, size_t yradius)
    {
        drive_ellipse(cx, cy, xradius, yradius, strokePix, Plot4EllipsePoints);
        
        return true;
    }

    // fillEllipse()
    // Fill ellipse with current fill color
    bool fillEllipse(GRCOORD cx, GRCOORD cy, size_t xradius, size_t yradius)
    {
        drive_ellipse(cx, cy, xradius, yradius, fillPix, fill2EllipseLines);
        return false;
    }
    
    // drawEllipse
    // fill and stroke an elllipse
    bool drawEllipse(GRCOORD cx, GRCOORD cy, size_t xradius, size_t yradius)
    {
        fillEllipse(cx, cy, xradius, yradius);
        strokeEllipse(cx, cy, xradius, yradius);

        return true;
    }





    // strokeTriangle()
    // Assumes coordinates are in top down order
    bool strokeTriangle(GRCOORD x1, GRCOORD y1, GRCOORD x2, GRCOORD y2, GRCOORD x3, GRCOORD y3)
    {
        strokeLine(x1, y1, x2, y2);
        strokeLine(x2, y2, x3, y3);
        strokeLine(x3, y3, x1, y1);

        return true;
    }

    bool strokeTriangle(const GRTriangle &geo)
    {
        strokeLine(geo.verts[0].x, geo.verts[0].y, geo.verts[1].x, geo.verts[1].y);
        strokeLine(geo.verts[1].x, geo.verts[1].y, geo.verts[2].x, geo.verts[2].y);
        strokeLine(geo.verts[2].x, geo.verts[2].y, geo.verts[0].x, geo.verts[0].y);

        return true;
    }

    // fillTriangle()
    bool fillTriangle(const GRTriangle &geo)
    {
        return false;
    }

    // drawTriangle()
    bool drawTriangle(const GRTriangle geo)
    {
        fillTriangle(geo);
        strokeTriangle(geo);

        return true;
    }
    

    // strokePolygon()
    bool strokePolygon(int nPts, Point2D *pts)
    {
        strokePolyLine(nPts, pts);
        if (nPts >= 3) {
            // We have enough points to close the 
            // polygon, so draw from last point to first point
            strokeLine(pts[nPts-1].x, pts[nPts-1].y, pts[0].x, pts[0].y);
        }

        return true;
    }
    // fillPolygon()
    bool fillPolygon(int nPts, Point2D *pts)
    {
        return false;
    }

    // drawPolygon()
    bool drawPolygon(int nPts, Point2D *pts)
    {
        fillPolygon(nPts, pts);
        strokePolygon(nPts, pts);
    }

    bool copyImage(const PixelBuffer &img, int x, int y)
    {
        int destX = x;
        int destY = y;
        int destWidth = img.getWidth();
        int destHeight = img.getHeight();
        //printf("copyImage: %d x %d\n", destWidth, destHeight);

        pb.blit(img, 0, 0, img.getWidth(),img.getHeight(), destX, destY, destWidth, destHeight);

        return false;
    }
};