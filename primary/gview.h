#pragma once

#include "apphost.h"
#include "drawable.h"

#include <memory>


/*
    The GView has a couple of primary
        purposes.
        1) Be a clipping region for an underlying graphic
        2) Be a drawing transformation for the underlying graphic

        The clipping region is taken care of by simply having a.frame
        property.The higher level(window, or graphicgroup) will automatically
        clip, and translate to this coordinate system.

        Beyond that, the view is essentially a two dimensional slider thumb.
        You can alter it's base transformation using setPosition(posX, posY)

        The position varies between 0 and 1 in both directions.

        The view must know how to map from the position value to the appropriate
        transformation values.This is done based on the size and potential
        location of the view relative to the size and location of the page.
*/

using std::shared_ptr;


class GView : public IDrawable
{
protected:
    BLMatrix2D fTransform;
    BLRect fFrame;
    shared_ptr<IDrawable> fPage;
    bool fDebug;

public:
    GView(const BLRect& frame)
        : fFrame(frame),
        fDebug(false)
    {
        fTransform = BLMatrix2D::makeIdentity();
    }

    bool contains(int x, int y)
    {
        return ((x >= fFrame.x) && (y >= fFrame.y) &&
            (x - fFrame.x <= fFrame.w) &&
            (y - fFrame.y <= fFrame.h));
    }

    void setTransform(BLMatrix2D& m) {    fTransform = m;}
    BLMatrix2D& getTransform() { return fTransform; }

    void setFrame(const BLRect& frame) { fFrame = frame; }
    virtual BLRect& getFrame() {return fFrame;}

    void moveBy(double dx, double dy)
    {
        //printf("moveBy: %f, %f\n", dx, dy);
        fFrame.x += dx;
        fFrame.y += dy;
    }

    void moveTo(double x, double y)
    {
        fFrame.x = x;
        fFrame.y = y;
    }

    void setPage(shared_ptr<IDrawable> page) {fPage = page;}

    shared_ptr<IDrawable> getPage() { return fPage; }

    virtual void drawBackground(IGraphics* ctx)
    {
        ctx->flush();
    }

    virtual void drawContent(IGraphics* ctx)
    {
        if (nullptr != fPage) {
            fPage->draw(ctx);
        }
    }

    virtual void drawForeground(IGraphics* ctx) 
    {
        ctx->flush();
    }

    virtual void draw(IGraphics* ctx)
    {
        ctx->push();
        ctx->clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);

        // Debug, draw frame
        if (fDebug) {
            ctx->noFill();
            ctx->stroke(255, 0, 0);
            ctx->strokeWeight(1);
            ctx->rect(fFrame.x, fFrame.y, fFrame.w, fFrame.h);
        }


        // BUGBUG - maybe perform arbitrary transform?
        auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
        ctx->translate(pt.x, pt.y);

        drawBackground(ctx);
        drawContent(ctx);
        drawForeground(ctx);

        ctx->noClip();
        ctx->pop();
    }

    virtual void mouseMoved(const MouseEvent& e)
    {
        // translate according to the transformation
        auto pt = fTransform.mapPoint(e.x, e.y);
        auto newEvent(e);
        newEvent.x = (int)(pt.x + fFrame.x);
        newEvent.y = (int)(pt.y + fFrame.y);
        //fPage.mouseMoved(newEvent);
    }

    virtual void mouseDragged(const MouseEvent& e)
    {
        // do nothing
    }

    virtual void mousePressed(const MouseEvent& e)
    {
        // do nothing
    }

    virtual void mouseReleased(const MouseEvent& e)
    {
        // do nothing
    }

    void translateBy(double x, double y)
    {
        fTransform.translate(x, y);
    }


};


