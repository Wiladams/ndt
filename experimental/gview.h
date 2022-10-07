#pragma once

#include "apphost.h"
#include "graphic.hpp"

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

class PageView : public Graphic
{
    shared_ptr<IDrawable> fPage;

public:
    PageView(const BLRect& r)
        :Graphic(r)
    {
    }

    PageView(const double x, const double y, const double w, const double h)
        : Graphic(BLRect(x, y, w, h))
    {
    }

    shared_ptr<IDrawable> getPage() { return fPage; }
    void setPage(shared_ptr<IDrawable> page) { fPage = page; }

    virtual void drawSelf(std::shared_ptr<IGraphics> ctx)
    {
        if (nullptr != fPage) {
            fPage->draw(ctx);
        }
    }
};



