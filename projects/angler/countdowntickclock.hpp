#pragma once

#include "graphic.hpp"
#include "secondtime.h"
#include "stopwatch.hpp"
#include "colorsampler.h"

// A single straight line tick mark that
// does some animation based on the progress
struct ProgressiveTickMark : public IDrawable
{
    // For animation
    double fStartAt=0;
    double fEndAt=0;
    double fDuration = 0;
    double fProgress = 0;

    // For drawing
    float fWeight=1.0f;
    Pixel fProgressColor{};
    Pixel fIdleColor{};

    float fAngle = 0.0f;

    float fShortRadius = 0.0f;
    float fOuterRadius = 0.0f;
    float fInnerRadius = 0.0f;


    ProgressiveTickMark(float r)
    {
        fOuterRadius = r;
        fInnerRadius = r - 36;
        fShortRadius = fInnerRadius + 20;

        fWeight = 1.0;
        fProgressColor = Pixel(255, 255, 255);
        fIdleColor = Pixel(100, 205, 170);
    }

    void setAnimationTime(double startAt, double endAt)
    {
        fStartAt = startAt;
        fEndAt = endAt;
        fDuration = fEndAt - fStartAt;
        fProgress = 0;
    }

    void setProgress(double prog)
    {
        fProgress = maths::Clamp(prog, 0.0, 1.0);
    }

    void update(double currentTime)
    {
        if (currentTime < fStartAt)
            setProgress(0);

        else if (currentTime > fEndAt)
            setProgress(1.0);
        else
            setProgress(maths::Map(currentTime, fStartAt, fEndAt, 0.0, 1.0));
    }

    virtual void draw(IGraphics& ctx)
    {
        ctx.push();

        ctx.rotate(fAngle);

        ctx.strokeWeight(fWeight);

        if (fProgress > 0.0) {
            // We're actively in progress
            // so extend the progress bar based
            // on that progress
            double outer = maths::Map(fProgress, 0, 1, fInnerRadius, fOuterRadius);

            // First draw the thing full length in idle color
            ctx.stroke(fIdleColor);
            ctx.line(fInnerRadius, 0, fOuterRadius, 0);

            // Now draw the progression in the tick color
            ctx.stroke(fProgressColor);
            ctx.line(fInnerRadius, 0, outer, 0);
        } else {
            // Not time to animate yet, so just
            // draw short in idle color
            ctx.stroke(fIdleColor);
            ctx.line(fInnerRadius, 0, fShortRadius, 0);
        }

        ctx.pop();
    }
};

struct CountdownTickClock : public Graphic
{
    static void getPreferredSize(int& x, int& y) { x = 300; y = 300; }

    float fStartRotation;
    float fCenterX;
    float fCenterY;
    float fRadius;
    float fSegmentRadians;
    int fNumTicks = 120;
    std::vector<ProgressiveTickMark> fTicks;
    Pixel fBeginColor;
    Pixel fEndColor;

    double fDuration;
    StopWatch fClock;

    CountdownTickClock(double dur)
        : Graphic(0, 0, 300, 300)
    {
        fDuration = dur;

        fStartRotation = maths::Radians(-90.0);
        fCenterX = width() / 2.0;
        fCenterY = height() / 2.0;
        fRadius = width() / 2.0;
        fSegmentRadians = maths::Radians(360.0 / fNumTicks);
        fBeginColor = Pixel(255, 255, 0);
        fEndColor = Pixel(255, 0, 0);

        createTicks();
    }

    // We only need to create the tick marks once.
    // Each tick is reponsible for its own drawing
    // but we must tell it what angle it's at, the
    // radius, color, and time range.
    void createTicks()
    {
        for (int i = 1; i <= fNumTicks; i++)
        {
            double t = (double)i / fNumTicks;
            Pixel c = ndt::lerpColor(fBeginColor, fEndColor, t);

            double startTime = (((double)(i - 1)) / fNumTicks) * fDuration;
            double endTime = (((double)(i)) / fNumTicks) * fDuration;

            ProgressiveTickMark tick(fRadius-4);
            tick.setAnimationTime(startTime, endTime);
            tick.fAngle = fSegmentRadians * i;
            tick.fProgressColor = c;

            if ((i % 15) == 0)
            {
                // Thicker for things every 45 degrees
                tick.fWeight = 4.0;
            }
            else {
                // Thinner lines for everything else
                tick.fWeight = 2.0;
            }

            fTicks.push_back(tick);
        }
    }

    // start the animation process by 
    // resetting the clock to zero
    // The drawing will automatically start
    // counting down again
    void start()
    {
        fClock.reset();
    }

    // 
    // drawForeground
    // This will be called automatically as part of drawing
    // a Graphic object.  We do the drawing here, reserving the
    // drawing of a background if we want that.

    void drawForeground(IGraphics& ctx)
    {
        ctx.push();
        
        ctx.translate(fCenterX, fCenterY);

        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
        //ctx.textFont("Consolas");
        ctx.noStroke();
        ctx.fill(225);

        double remaining = fDuration - fClock.seconds();
        
        SecondTime st(0);
        if (remaining > 0.0) {
            st.setSeconds(remaining);
            char buff[256];
            //sprintf_s(buff, "%3zd : %02zd : %02zd", st.fHours, st.fMinutes, st.fSeconds);
            
            sprintf_s(buff, "%02zd:%02zd", st.fMinutes, st.fSeconds);
            ctx.textSize(36);
            ctx.text(buff, 0, 0);

            ctx.textSize(16);
            ctx.text("REMAINING", 0, 24);
        }
        else
        {
            ctx.textSize(36);
            ctx.text("COMPLETED", 0, 0);
        }


        // Do the ticks
        ctx.rotate(fStartRotation);

        for (auto& tick : fTicks)
        {
            tick.update(fClock.seconds());
            tick.draw(ctx);
        }

        ctx.pop();
    }
};









