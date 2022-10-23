#pragma once
/*
    A slider is a graphic that provides for some constrained motion.The constraint
        is expressed in the form of a MotionConstraint object.

        There is a 'track', which represents the motion a 'thumb' will travel over
        during user interaction.The track and constraint go hand in hand, as the track
        is a representation of the MotionConstraint.

        The constraint can express two dimensions and not just one.

        A slider can be created with as little information as a starting
        and ending point.

        slider:create({ startPoint = {x = 10,y = 4}; endPoint = {x = 10,y = 20}; })

*/

#include "graphic.hpp"
#include "MotionConstraint.h"
#include "pubsub.h"

// A slider thumb is a simple thing that
// knows how to draw a thumb for a slider
// easily augmented with any draw method
// to specialize the appearance.
struct SliderThumb : public Graphic
{
    static const int thumbRadius = 4;
    static const int thumbThickness = 24;

    //BLRoundRect shape;
    double fRadius;
    Pixel thumbColor{};

    SliderThumb(const BLRect &f)
        : Graphic(f.x, f.y, f.w, f.h)
    {
        thumbColor = Pixel(0xc2, 0xc3, 0xc9);

        fRadius = thumbRadius;

        //shape = BLRoundRect(0, 0, width(), height(), fRadius, fRadius);
    }

    // a lozinger rounded rect
    void draw(IGraphics& ctx) override
    {
        ctx.strokeWeight(2);
        ctx.stroke(Pixel(127, 127, 127, 120));
        ctx.fill(thumbColor);
        ctx.rect(frame().x, frame().y, frame().w, frame().h,
            fRadius, fRadius);
    }
};



constexpr int SLIDER_VERTICAL = 1;
constexpr int SLIDER_HORIZONTAL = 2;

//
// Slider Class
//


class Slider : public Graphic, public Topic<float>
{
    static constexpr float trackThickness = 8.0f;

    MotionConstraint fConstraint;

    maths::vec2f fPosition{};
    maths::vec2f fLastLocation{};
    int fOrientation = 0;

    bool fDragging;
    Pixel fTrackColor;
    std::shared_ptr<Graphic> fThumb;


public:
    Slider(const BLRect &f, const maths::vec2f &pos, std::shared_ptr<Graphic> thumb)
        :Graphic(f.x, f.y, f.w, f.h)
        ,fDragging(false)
        ,fThumb(thumb)

    {
        addChild(thumb);
        setPosition(pos.x, pos.y);
    }

    void setOrientation(int orient)
    {
        fOrientation = orient;
    }

    //
    //    Returns a number in range[0..1]
    //
    const maths::vec2f & getPosition() const
    {
        return fPosition;
        //return map(self.thumb.frame.x, self.constraint.minX, self.constraint.maxX, 0, 1)
    }

    void setPosition(const float x, const float y)
    {
        fPosition.x = maths::clamp(x, 0.0f, 1.0f);
        fPosition.y = maths::clamp(y, 0.0f, 1.0f);

        auto locY = maths::map(y, 0.0, 1.0, fConstraint.fminY, fConstraint.fmaxY);
        auto locX = maths::map(x, 0.0, 1.0, fConstraint.fminX, fConstraint.fmaxX);
        if (fThumb != nullptr)
        {
            fThumb->moveTo(locX, locY);
            fLastLocation = { float(fThumb->frame().x), float(fThumb->frame().y) };
        }
    }


    void changeThumbLocation(const maths::vec2f &change)
    {
        maths::vec2f movement = fConstraint.tryChange(fThumb->frame(), change);

        fThumb->moveBy(movement.x, movement.y);

        maths::vec2f position = fConstraint.calcPosition(fThumb->frame());
        fPosition = position;

        //publish change event
        if (fOrientation == SLIDER_HORIZONTAL)
        {
            notify(fPosition.x);
        }
        else {
            notify(fPosition.y);
        }
    }

    void mousePressed(const MouseEvent& e) override
    {
        fDragging = true;
        fLastLocation = {float(e.x), float(e.y) };
    }

    void mouseReleased(const MouseEvent& e) override
    {
        fDragging = false;
    }

    void mouseMoved(const MouseEvent& e) override
    {
        if (fDragging) {
            maths::vec2f change{ e.x - fLastLocation.x, e.y - fLastLocation.y };
            changeThumbLocation(change);
        }
        fLastLocation = { float(e.x), float(e.y) };
    }

    void drawBackground(IGraphics& ctx) override
    {
        // Debugging
        //ctx.strokeWeight(4.0);
        //ctx.noFill();
        //ctx.stroke(255, 0, 0);
        //ctx.rect(fBounds.x, fBounds.y, fBounds.w, fBounds.h);


        //draw line between endpoints
        ctx.strokeWeight(trackThickness);
        ctx.stroke(0xf5);

        if (fOrientation == SLIDER_HORIZONTAL)
        {
            ctx.stroke(0xf5);
            // Set line join to rounded
            ctx.strokeCaps(BL_STROKE_CAP_ROUND);
            ctx.line(fBounds.x, fBounds.y + (fBounds.h / 2.0), fBounds.x + fBounds.w, fBounds.y + (fBounds.h / 2.0));
        }
        else {
            ctx.strokeCaps(BL_STROKE_CAP_ROUND);
            ctx.stroke(0x3d, 60);
            float x1 = fBounds.x + fBounds.w / 2.0f;
            float y1 = fBounds.y;
            float x2 = fBounds.x + fBounds.w / 2.0f;
            float y2 = fBounds.y + fBounds.h;
            ctx.line(x1, y1, x2, y2);

            ctx.strokeWeight(trackThickness / 4.0f);
            ctx.stroke(0xc0,220);
            ctx.line(x1, y1, x2, y2);

        }
    }


    static std::shared_ptr<Slider> create(const maths::vec2f& startPoint, const maths::vec2f& endPoint, const maths::vec2f & startPos = { 0,0 }, std::shared_ptr<Graphic> thumb = nullptr)
    {
        auto dx = maths::abs((endPoint.x - startPoint.x));
        auto dy = maths::abs((endPoint.y - startPoint.y));

        int orientation = SLIDER_VERTICAL;
        if (dx > dy)
        {
            orientation = SLIDER_HORIZONTAL;
        }

        // create the thumb
        BLRect thumbFrame{};

        if (orientation == SLIDER_VERTICAL)
        {
            thumbFrame.x = 0;
            thumbFrame.y = 0;
            thumbFrame.w = trackThickness*4.0;
            thumbFrame.h = trackThickness*2.0;
        } else {
            thumbFrame.x = 0;
            thumbFrame.y = 0;
            thumbFrame.w = trackThickness * 2.0;
            thumbFrame.h = trackThickness * 4.0;
        }
        std::shared_ptr<SliderThumb> sliderThumb = std::make_shared<SliderThumb>(thumbFrame);

        //Now figure out the frame of the entire slider
        BLRect sliderFrame{};
        MotionConstraint sliderConstraint;
        //maths::vec2f sliderStart{};
        //maths::vec2f sliderEnd{};

        if (orientation == SLIDER_VERTICAL)
        {
            sliderFrame.w = thumbFrame.w;
            sliderFrame.h = dy;

            sliderFrame.x = startPoint.x - (thumbFrame.w / 2.0);
            sliderFrame.y = startPoint.y;



            sliderConstraint = MotionConstraint(0, 0, 0, sliderFrame.h - sliderThumb->frame().h);

            //sliderStart = { trackThickness / 2, 0 };
            //sliderEnd = { trackThickness / 2, (float)sliderFrame.h };
        }
        else {
            sliderFrame.w = dx;
            sliderFrame.h = thumbFrame.h;

            sliderFrame.x = startPoint.x;
            sliderFrame.y = startPoint.y - (thumbFrame.h/2.0);

            sliderConstraint = MotionConstraint(0, 0, sliderFrame.w - sliderThumb->frame().w, 0);

            //sliderStart = { 0, trackThickness / 2 };
            //sliderEnd = { (float)sliderFrame.w, trackThickness / 2 };
        }

        auto slider = std::make_shared<Slider>(sliderFrame, startPos, sliderThumb);

        slider->setOrientation(orientation);
        slider->fTrackColor = Pixel(0xff, 0, 0);
        slider->fConstraint = sliderConstraint;

        return slider;
    }
};
