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
    static const int thumbRadius = 8;
    static const int thumbThickness = 24;

    BLRoundRect shape;
    double radius;
    Pixel thumbColor{};

    SliderThumb(const BLRect &f)
        : Graphic(f.x, f.y, f.w, f.h)
    {
        //thumbColor = Pixel(0xff, 0, 0);
        thumbColor = Pixel(0x7f, 0xC0, 0xC0);

        radius = thumbRadius;

        shape = BLRoundRect(0, 0, width(), height(), radius, radius);
    }

    // a lozinger rounded rect
    void draw(IGraphics& ctx) override
    {
        ctx.strokeWeight(2);
        ctx.stroke(Pixel(127, 127, 127, 120));
        ctx.fill(thumbColor);
        ctx.rect(getFrame().x, getFrame().y, getFrame().w, getFrame().h,
            radius, radius);
    }
};



constexpr int SLIDER_VERTICAL = 1;
constexpr int SLIDER_HORIZONTAL = 2;

//
// Slider Class
//
class Slider : public Graphic, public Topic<BLPoint>
{
    static const int trackThickness = 8;

    MotionConstraint fConstraint;

    BLPoint fStartPoint;
    BLPoint fEndPoint;

    double fLowValue;
    double fHighValue;

    BLPoint fPosition;
    BLPoint fLastLocation;


    bool fDragging;
    Pixel fTrackColor;
    std::shared_ptr<Graphic> fThumb;


public:
    Slider(double lowValue, double highValue, const BLPoint &pos, std::shared_ptr<Graphic> thumb)
        :Graphic(),
        fLowValue(lowValue),
        fHighValue(highValue),
        fThumb(thumb),
        fDragging(false)
    {
        addChild(thumb);
        setPosition(pos.x, pos.y);
    }

    void drawBackground(IGraphics &ctx) override
    {
        // Debugging
        //ctx.strokeWeight(1.0);
        //ctx.noFill();
        //ctx.stroke(255, 255, 0);
        //ctx.rect(fBounds.x, fBounds.y, fBounds.w, fBounds.h);


        //print("slider.drawBackground: ", self.frame.x, self.frame.y, self.frame.width, self.frame.height)
        //draw line between endpoints
        //ctx.strokeWeight(Slider::trackThickness);
        ctx.strokeWeight(trackThickness);
        ctx.stroke(120);
        ctx.line(fBounds.x, fBounds.y+(fBounds.h/2.0), fBounds.x+fBounds.w, fBounds.y + (fBounds.h / 2.0));

        //draw a couple of circles at the ends
        ctx.noStroke();
        ctx.fill(10);
        ctx.circle(fBounds.x, fBounds.y + (fBounds.h / 2.0), (Slider::trackThickness / 2.0) + 2.0);
        ctx.circle(fBounds.x+fBounds.w, fBounds.y + (fBounds.h / 2.0), (Slider::trackThickness / 2.0) + 2.0);
    }

    //
    //    Returns a number in range[0..1]
    //
    BLPoint getPosition()
    {
        return fPosition;
        //return map(self.thumb.frame.x, self.constraint.minX, self.constraint.maxX, 0, 1)
    }

    void setPosition(const double x, const double y)
    {
        fPosition.x = maths::Clamp(x, 0.0, 1.0);
        fPosition.y = maths::Clamp(y, 0.0, 1.0);

        auto locY = maths::Map(y, 0.0, 1.0, fConstraint.fminY, fConstraint.fmaxY);
        auto locX = maths::Map(x, 0.0, 1.0, fConstraint.fminX, fConstraint.fmaxX);

        if (fThumb != nullptr) {
            fThumb->moveTo(locX, locY);
            fLastLocation = { fThumb->getFrame().x, fThumb->getFrame().y };
        }
    }

    double getValue()
    {
        return maths::Map(getPosition().x, 0.0, 1.0, fLowValue, fHighValue);
    }

    void changeThumbLocation(const BLPoint &change)
    {
        BLPoint movement = fConstraint.tryChange(fThumb->getFrame(), change);

        fThumb->moveBy(movement.x, movement.y);

        BLPoint position = fConstraint.calcPosition(fThumb->getFrame());
        fPosition = position;

        //publish change event
        notify(fPosition);
    }

    void mousePressed(const MouseEvent& e) override
    {
        fDragging = true;
        fLastLocation = { (double)e.x, (double)e.y };
    }

    void mouseReleased(const MouseEvent& e)
    {
        fDragging = false;
    }

    void mouseMoved(const MouseEvent& e)
    {
        if (fDragging) {
            BLPoint change(e.x - fLastLocation.x, e.y - fLastLocation.y);
            changeThumbLocation(change);
        }
        fLastLocation = BLPoint{ (double)e.x, (double)e.y };
    }


    static std::shared_ptr<Slider> create(const BLPoint& startPoint, const BLPoint& endPoint, double lowValue = 0, double highValue = 255, BLPoint pos = { 0,0 }, std::shared_ptr<Graphic> thumb = nullptr)
    {
        //local thickness = params.thickness or 24
        double dx = maths::Abs(endPoint.x - startPoint.x);
        double dy = maths::Abs(endPoint.y - startPoint.y);

        int orientation = SLIDER_VERTICAL;
        if (dx > dy)
        {
            orientation = SLIDER_HORIZONTAL;
        }


        // create the thumb
        //local thumbParams = params.thumb or {}
        //thumbParams.length = thumbParams.length or 60;
        //thumbParams.thumbColor = thumbParams.thumbColor or 0x70;
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
            thumbFrame.w = trackThickness * 2;
            thumbFrame.h = trackThickness * 4;
        }
        std::shared_ptr<SliderThumb> sliderThumb = std::make_shared<SliderThumb>(thumbFrame);

        //Now figure out the frame of the entire slider
        BLRect sliderFrame{};
        MotionConstraint sliderConstraint;
        BLPoint sliderStart{};
        BLPoint sliderEnd{};

        if (orientation == SLIDER_VERTICAL)
        {
            sliderFrame.w = trackThickness;
            sliderFrame.h = abs(dy);

            sliderFrame.x = startPoint.x - (trackThickness / 2.0);
            sliderFrame.y = startPoint.y;



            sliderConstraint = MotionConstraint(0, 0, 0, sliderFrame.h - sliderThumb->getFrame().h);

            sliderStart = BLPoint(trackThickness / 2.0, 0);
            sliderEnd = BLPoint(trackThickness / 2.0, sliderFrame.h);
        }
        else {
            sliderFrame.w = dx;
            sliderFrame.h = thumbFrame.h;

            sliderFrame.x = startPoint.x;
            sliderFrame.y = startPoint.y - (thumbFrame.h/2.0);

            sliderConstraint = MotionConstraint(0, 0, sliderFrame.w - sliderThumb->getFrame().w, 0);

            sliderStart = BLPoint(0, trackThickness / 2.0);
            sliderEnd = BLPoint(sliderFrame.w, trackThickness / 2.0);
        }

        auto slider = std::make_shared<Slider>(lowValue, highValue, BLPoint(0, 0), sliderThumb);
        slider->setFrame(sliderFrame);
        slider->fStartPoint = sliderStart;
        slider->fEndPoint = sliderEnd;
        slider->fTrackColor = Pixel(0xff, 0, 0);
        slider->fConstraint = sliderConstraint;
        slider->fPosition = BLPoint(0, 0);

        return slider;
    }
};