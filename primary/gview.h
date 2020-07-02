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
    BLMatrix2D fTransform;
    BLRect fFrame;
    shared_ptr<IDrawable> fPage;
    bool fDebug;

public:
    GView(const BLRect& frame)
        : fFrame(frame),
        fDebug(true)
    {
        fTransform = BLMatrix2D::makeIdentity();
    }

    void setTransform(BLMatrix2D& m) {    fTransform = m;}
    BLMatrix2D& getTransform() { return fTransform; }

    void setFrame(const BLRect& frame) { fFrame = frame; }
    virtual BLRect& getFrame() {return fFrame;}

    void setPage(shared_ptr<IDrawable> page) {fPage = page;}

    shared_ptr<IDrawable> getPage() { return fPage; }


    virtual void draw(IGraphics* ctx)
    {
        ctx->push();

        // Debug, draw frame
        if (fDebug) {
            ctx->noFill();
            ctx->stroke(255, 0, 0);
            ctx->strokeWeight(1);
            ctx->rect(fFrame.x, fFrame.y, fFrame.w, fFrame.h);
        }

        // set the clip to our frame
        // call draw on our embedded page
        if (nullptr != fPage) {
            ctx->push();
            ctx->clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);
            
            // BUGBUG - maybe perform arbitrary transform?
            auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
            //ctx->translate(fFrame.x, fFrame.y);
            ctx->translate(pt.x, pt.y);

            fPage->draw(ctx);
            ctx->pop();
        }

        ctx->pop();
    }

    void mouseMoved(const MouseEvent& e)
    {
        // translate according to the transformation
        auto pt = fTransform.mapPoint(e.x, e.y);
        auto newEvent(e);
        newEvent.x = pt.x + fFrame.x;
        newEvent.y = pt.y + fFrame.y;
        //fPage.mouseMoved(newEvent);
    }

    void translateBy(double x, double y)
    {
        fTransform.translate(x, y);
    }

    /*
        void calcTransform(int &px, int &py)
        {
            auto pageSize = page.getPreferredSize();
            int x = map(px, 0, 1, 0, Max(0, pageSize.width - fFrame.w));
            int y = map(py, 0, 1, 0, Max(0, pageSize.height - fFrame.h));

            //print("BView.calcTransform: ", x, y)
            px = x;
            py = y;
        }

        void setPosition(int px, int py)
        {
            //print("BView.setPosition: ", px, py)
            Tx, self.Ty = self:calcTransform(px, py)
        }

        function BView.handleVerticalPositionChange(self, slider)
            --print("BView.handleVerticalPositionChange: ", slider, slider:getPosition())

            local pos = { x = 0,y = 0 }
            if slider then
                pos = slider:getPosition()
            end

            self : setPosition(pos.x, pos.y)
        end

        function BView.handleHorizontalPositionChange(self, slider)
            --print("BView.handleHorizontalPositionChange: ", slider)
            local ypos = 0;
            local xpos = 0;

            if slider then
                xpos = slider:getPosition()
            end

            self : setPosition(xpos, ypos)
        end


*/
};


