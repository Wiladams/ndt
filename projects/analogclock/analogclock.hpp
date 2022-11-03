#pragma once

#include "p5.hpp"
#include "calendar.hpp"

#include <string>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <cstdio>


struct AnalogClock 
{
    maths::vec2f fCenter;

    float fRadius;
    float fCenterRadius;
    Pixel fBackgroundColor;

    bool fDrivenExternally;
    SYSTEMTIME fTime;

    // For animation
    float fOvershootAmount;
    float fOvershootRemaining;
    float fRecoveryIncrement;
    long fLastSec;

    // For flying
    bool fInFlight = false;
    double fFlightTime=0;     // at which seconds will we start flying
    float fFlightSpeed=0;
    float fFlightDuration = 0;
    maths::vec2f fFlightDirection;

public:
    static const int SmallestRadius = 90;
    static const int LargestRadius = 300;

    AnalogClock(float cx, float cy, float radius = 240)
    {
        fCenter = { cx,cy };
        fRadius = radius;
        fCenterRadius = fRadius*.618;
        fBackgroundColor = p5::color(0xe2, 0x84, 0x30, 0x7f);

        // For doing hand animation
        fOvershootAmount = p5::radians(1.25);
        fOvershootRemaining = 0;
        fRecoveryIncrement = p5::radians(1.0);
        fLastSec = 0;

        fDrivenExternally = false;

        // Flight Stuff
        GetLocalTime(&fTime);

        calculateFlightTime();
    }

    void setDirection(const maths::vec2f& dv)
    {
        // If we're not currently moving
        // don't take on additional velocity
        if (!fInFlight)
        {
            return;
        }

        //printf("setDirection: %3.2f,%3.2f\n", dv.x, dv.y);

        fFlightDirection = dv;
    }

    void adjustSpeed(const float sp)
    {
        if (!fInFlight)
            return;

        fFlightSpeed *= sp;
    }

    void calculateFlightTime()
    {
        // Every time we calculate the flight time, the 
        // fCenterRadius should reduce until it gets down
        // to the smallest internal radius (3)
        fCenterRadius = p5::constrain(fCenterRadius-0.3, 3, fCenterRadius);

        // delay before flight takeoff, 
        // 10 to 40 seconds
        fFlightTime = p5::seconds() + p5::random(4, 30);

        // flight duration 
        // from 10 to 40 seconds
        fFlightDuration = p5::random(10, 40);

        fFlightDirection = maths::normalize(maths::vec2f{ p5::random(-1, 1), p5::random(-1, 1) });

       
        // The speed is inversely proportional to the size
        // smaller clocks move faster
        fFlightSpeed = p5::map(fRadius, SmallestRadius, LargestRadius, 10, 4);

    }

    void setColor(const Pixel& c)
    {
        fBackgroundColor = c;
    }

    void moveTo(const float cx, const float cy)
    {
        fCenter = { cx, cy };
    }

    void autoMove()
    {
        fCenter += (fFlightDirection * fFlightSpeed);

        maths::vec2f newdir = fFlightDirection;

        // boundary test
        // if we hit a limit, reverse direction
        if ((fCenter.x < 0) || (fCenter.x > canvasWidth))
        {
            fCenter.x = maths::clamp(fCenter.x, 0.0f, canvasWidth - 1.0f);
            newdir.x *= -1;
        }

        if ((fCenter.y < 0) || (fCenter.y > canvasHeight))
        {
            fCenter.y = maths::clamp(fCenter.y, 0.0f, canvasHeight - 1.0f);
            newdir.y *= -1;
        }

        setDirection(newdir);

        // If the current time is beyond our intended time
        // then we're no longer in flight
        if (p5::seconds() > fFlightTime) 
        {
            if (p5::seconds() < (fFlightTime + fFlightDuration))
            {
                fInFlight = true;
            }
            else {
                fInFlight = false;
                calculateFlightTime();
            }
        }
    }

    void drawSecondTickmarks(IGraphics &ctx)
    {
        ctx.push();

        double segmentRads = p5::radians(360 / 60);

        ctx.strokeWeight(2);
        ctx.stroke(30);

        for (int i = 0; i <= 59; i++)
        {
            ctx.line(fRadius - 10, 0, fRadius - 4, 0);
            ctx.rotate(segmentRads);
        }

        ctx.pop();
    }


    /*
        This needs to draw outside the context of other
            rotations and translations so we don't get
            mixed up in the various transformations.

            Ideally, we'd be able to use the current rotation
            and translation, and counter rotate the text
            along the way.
    */

    void drawHourNumbers(IGraphics& ctx)
    {
        ctx.push();

        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
        ctx.noStroke();
        ctx.fill(30);

        double segmentRads = p5::radians(360 / 12);
        double angle = segmentRads - p5::radians(90);
        double r = fRadius - 34;

        for (int i = 1; i <= 12; i++) {
            std::string Number = std::to_string(i);
            ctx.text(Number.c_str(), fCenter.x + (r * cos(angle)), fCenter.y + (r * sin(angle)));
            angle = angle + segmentRads;
        }

        ctx.pop();
    }
                    
        
    void drawHourTickmarks(IGraphics& ctx)
    {
        ctx.push();
        double segmentRads = p5::radians(360 / 12);

        ctx.strokeWeight(4);
        ctx.stroke(30);

        for (int i = 0; i <= 11; i++) {
            ctx.line(fRadius - 20, 0, fRadius - 4, 0);
            ctx.rotate(segmentRads);
        }

        ctx.pop();
    }


    void drawHourHand(IGraphics& ctx)
    {
        ctx.push();

        // Do the rotation thing
        double minuteContribution = p5::radians((double)fTime.wMinute / (60.0 * 30.0));
        double hourRads = p5::radians(p5::map(fTime.wHour % 12, 0, 11, 0, 330)) + minuteContribution;
        ctx.rotate(hourRads);

        // Draw the indicator line
        ctx.strokeWeight(4);
        ctx.stroke(30);
        ctx.line(-4, 0, fRadius - 40, 0);

        ctx.pop();
    }

    void drawMinuteHand(IGraphics& ctx)
    {
        ctx.push();

        // Do the rotation thing
        double minuteRads = p5::radians(p5::map(fTime.wMinute, 0, 59, 0, 354));
        ctx.rotate(minuteRads);

        // Draw the indicator line
        ctx.strokeWeight(4);
        ctx.stroke(30);
        ctx.line(-2, 0, fRadius - 20, 0);

        ctx.pop();
    }
 
    void drawSecondsHand(IGraphics& ctx)
    {
        ctx.push();

        // Do the rotation thing
        // translate(self.centerX, self.centerY)
        double secRads = p5::radians(p5::map(fTime.wSecond, 0, 59, 0, 354));
        if (fTime.wSecond != fLastSec) {
            fOvershootRemaining = fOvershootAmount;
            fLastSec = fTime.wSecond;
        }

        secRads = secRads + fOvershootRemaining;

        fOvershootRemaining = p5::constrain(fOvershootRemaining - fRecoveryIncrement, 0, fOvershootAmount);

        ctx.rotate(secRads);

        // Draw the indicator line
        ctx.strokeWeight(1);
        ctx.stroke(255, 0, 0);
        ctx.line(-8, 0, fRadius - 10, 0);

        ctx.pop();
    }
    
    void drawDate(IGraphics& ctx)
    {
        char buff[1024];
        int bufflen = 1024;

        sprintf_s(buff, bufflen, "%s %s %d", 
            Calendar::WeekDaysShort[fTime.wDayOfWeek].c_str(), 
            Calendar::MonthsShort[fTime.wMonth].c_str(), 
            fTime.wDay);
        
        //ctx.stroke(0x20);
        ctx.noStroke();
        ctx.fill(0x20);
        ctx.text(buff, fCenter.x, fCenter.y + 24);
    }

    void drawBackground(IGraphics& ctx)
    {
        // fill background
        BLGradient gradient(BLRadialGradientValues(fCenter.x, fCenter.y, fCenter.x, fCenter.y, fRadius));
        gradient.addStop(0, p5::color(220, 127));       // center
        gradient.addStop(0.20, p5::color(fBackgroundColor.r(), fBackgroundColor.g(), fBackgroundColor.b(), 127));     // center
        gradient.addStop(0.80, fBackgroundColor);
        gradient.addStop(1.0, p5::color(65, 127));     // edge
        ctx.fill(gradient);
        //ctx.fill(fBackgroundColor);
        ctx.noStroke();

        ctx.ellipseMode(ELLIPSEMODE::RADIUS);
        ctx.circle(fCenter.x, fCenter.y, fRadius);

        // draw inside radius
        // This indicates where mouse activation occurs
        ctx.noStroke();
        ctx.fill(240,65);
        ctx.circle(fCenter.x, fCenter.y, fCenterRadius);
    }

    void draw(IGraphics& ctx)
    {
        // if self driven, get our own time
        if (!fDrivenExternally) {
            GetLocalTime(&fTime);
        }

        drawBackground(ctx);

        ctx.push();

        //draw hour numbers before we get all rotated
        drawHourNumbers(ctx);

        //orient so 0 angle is at 12 - oclock
        // and do all angle oriented drawing within the
        // context of this save / restore
        ctx.translate(fCenter.x, fCenter.y);
        ctx.rotate(p5::radians(-90));

        drawHourTickmarks(ctx);
        drawSecondTickmarks(ctx);

        drawHourHand(ctx);
        drawMinuteHand(ctx);
        drawSecondsHand(ctx);
        ctx.pop();

        drawDate(ctx);

        // If the mouse down while inside us, we'll stop
        if (mouseIsPressed)
        {
            //double ldistance = p5::dist(p5::mouseX, p5::mouseY, fCenterX, fCenterY);
            float ldistance = maths::distance(maths::vec2f{ (float)mouseX,(float)mouseY }, fCenter);
            if ((ldistance >= 1) && (ldistance < fCenterRadius))
            {
                // Move closer to the cursor location position
                float x = p5::lerp(mouseX, fCenter.x, 0.3);
                float y = p5::lerp(mouseY, fCenter.y, 0.3);
                moveTo(x, y);

                calculateFlightTime();
            }
        } else {
            // otherwise, see if it's time to fly away
            if ((fFlightTime > 0) && (p5::seconds() > fFlightTime)) {
                autoMove();
            }
        }
    }
};


