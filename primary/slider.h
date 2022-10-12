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
        ctx.rect(getFrame().x, getFrame().y, getFrame().w, getFrame().h,
            fRadius, fRadius);
    }
};



constexpr int SLIDER_VERTICAL = 1;
constexpr int SLIDER_HORIZONTAL = 2;

//
// Slider Class
//


class Slider : public Graphic, public Topic<maths::vec2f &>
{
    static constexpr float trackThickness = 8.0f;

    MotionConstraint fConstraint;

    maths::vec2f fStartPoint{};
    maths::vec2f fEndPoint{};

    float fLowValue;
    float fHighValue;

    maths::vec2f fPosition{};
    maths::vec2f fLastLocation{};
    int fOrientation = 0;

    bool fDragging;
    Pixel fTrackColor;
    std::shared_ptr<Graphic> fThumb;


public:
    Slider(float lowValue, float highValue, const BLPoint &pos, std::shared_ptr<Graphic> thumb)
        :Graphic(),
        fLowValue(lowValue),
        fHighValue(highValue),
        fThumb(thumb),
        fDragging(false)
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
                if (fThumb != nullptr) {            fThumb->moveTo(locX, locY);
            fLastLocation = { float(fThumb->getFrame().x), float(fThumb->getFrame().y) };
        }
    }

    double getValue()
    {
        return maths::map(getPosition().x, 0.0, 1.0, fLowValue, fHighValue);
    }

    void changeThumbLocation(const maths::vec2f &change)
    {
        maths::vec2f movement = fConstraint.tryChange(fThumb->getFrame(), change);

        fThumb->moveBy(movement.x, movement.y);

        maths::vec2f position = fConstraint.calcPosition(fThumb->getFrame());
        fPosition = position;

        //publish change event
        notify(fPosition);
    }

    void mousePressed(const MouseEvent& e) override
    {
        fDragging = true;
        fLastLocation = {float(e.x), float(e.y) };
    }

    void mouseReleased(const MouseEvent& e)
    {
        fDragging = false;
    }

    void mouseMoved(const MouseEvent& e)
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
        //ctx.strokeWeight(1.0);
        //ctx.noFill();
        //ctx.stroke(255, 255, 0);
        //ctx.rect(fBounds.x, fBounds.y, fBounds.w, fBounds.h);


        //draw line between endpoints
        ctx.strokeWeight(trackThickness);
        ctx.stroke(0xf5);

        if (fOrientation == SLIDER_HORIZONTAL)
        {
            ctx.stroke(0xf5);
            ctx.line(fBounds.x, fBounds.y + (fBounds.h / 2.0), fBounds.x + fBounds.w, fBounds.y + (fBounds.h / 2.0));
        
            ctx.noStroke();
            ctx.fill(10);
            ctx.circle(fBounds.x, fBounds.y + (fBounds.h / 2.0), (Slider::trackThickness / 2.0) + 2.0);
            ctx.circle(fBounds.x + fBounds.w, fBounds.y + (fBounds.h / 2.0), (Slider::trackThickness / 2.0) + 2.0);

        }
        else {
            ctx.stroke(0xC0);
            float x1 = fBounds.x + fBounds.w / 2.0f;
            float y1 = fBounds.y;
            float x2 = fBounds.x + fBounds.w / 2.0f;
            float y2 = fBounds.y + fBounds.h;
            ctx.line(x1, y1, x2, y2);

            ctx.noStroke();
            ctx.fill(10);
            ctx.circle(x1, y1, (Slider::trackThickness / 2.0f) + 2);
            ctx.circle(x1, y2, (Slider::trackThickness / 2.0f) + 2);
        }
    }


    static std::shared_ptr<Slider> create(const maths::vec2f& startPoint, const maths::vec2f& endPoint, float lowValue = 0, float highValue = 255, const maths::vec2f & pos = { 0,0 }, std::shared_ptr<Graphic> thumb = nullptr)
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
            thumbFrame.w = trackThickness*2;
            thumbFrame.h = trackThickness*4;
        } else {
            thumbFrame.x = 0;
            thumbFrame.y = 0;
            thumbFrame.w = trackThickness * 4;
            thumbFrame.h = trackThickness * 2;
        }
        std::shared_ptr<SliderThumb> sliderThumb = std::make_shared<SliderThumb>(thumbFrame);

        //Now figure out the frame of the entire slider
        BLRect sliderFrame{};
        MotionConstraint sliderConstraint;
        maths::vec2f sliderStart{};
        maths::vec2f sliderEnd{};

        if (orientation == SLIDER_VERTICAL)
        {
            sliderFrame.w = thumbFrame.w;
            sliderFrame.h = dy;

            sliderFrame.x = startPoint.x - (thumbFrame.w / 2.0);
            sliderFrame.y = startPoint.y;



            sliderConstraint = MotionConstraint(0, 0, 0, sliderFrame.h - sliderThumb->getFrame().h);

            sliderStart = { trackThickness / 2, 0 };
            sliderEnd = { trackThickness / 2, (float)sliderFrame.h };
        }
        else {
            sliderFrame.w = dx;
            sliderFrame.h = thumbFrame.h;

            sliderFrame.x = startPoint.x;
            sliderFrame.y = startPoint.y - (thumbFrame.h/2.0);

            sliderConstraint = MotionConstraint(0, 0, sliderFrame.w - sliderThumb->getFrame().w, 0);

            sliderStart = { 0, trackThickness / 2 };
            sliderEnd = { (float)sliderFrame.w, trackThickness / 2 };
        }

        auto slider = std::make_shared<Slider>(lowValue, highValue, BLPoint(0, 0), sliderThumb);
        slider->setFrame(sliderFrame);
        slider->setOrientation(orientation);
        slider->fStartPoint = sliderStart;
        slider->fEndPoint = sliderEnd;
        slider->fTrackColor = Pixel(0xff, 0, 0);
        slider->fConstraint = sliderConstraint;
        slider->fPosition = pos;

        return slider;
    }
};
