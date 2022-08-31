#pragma once

#include "p5.hpp"
#include "calendar.hpp"

#include <string>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <cstdio>

//using namespace p5;


class AnalogClock {

    double fCenterX;
    double fCenterY;
    double fRadius;
    double fCenterRadius;
    Pixel fBackgroundColor;

    bool fDrivenExternally;
    SYSTEMTIME fTime;

    // For animation
    double fOvershootAmount;
    double fOvershootRemaining;
    double fRecoveryIncrement;
    long fLastSec;

    // For flying
    double fFlightTime=0;     // at which seconds will we start flying
    double fFlightSpeed=0;
    double fFlightDuration = 0;
    BLPoint fFlightDirection;


public:
    static const int SmallestRadius = 90;
    static const int LargestRadius = 300;


    static BLSize getPreferredSize() { return BLSize(240, 240); }

    AnalogClock(double cx, double cy, double radius = 240)
    {

        fCenterX = cx;
        fCenterY = cy;
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

    void calculateFlightTime()
    {
        // Every time we calculate the flight time, the 
        // fCenterRadius should reduce until it gets down
        // to the smallest internal radius (3)
        fCenterRadius = p5::constrain(fCenterRadius-0.3, 3, fCenterRadius);

        // delay before flight takeoff, 10 to 60 seconds
        fFlightTime = p5::seconds() + p5::random(10, 60);

        // flight duration from 10 to 60 seconds
        fFlightDuration = p5::random(10, 60);


        fFlightDirection = BLPoint(p5::random(-2, 2), p5::random(-2, 2));
        // The speed is inversely proportional to the size
        // smaller clocks move faster
        fFlightSpeed = p5::map(fRadius, SmallestRadius, LargestRadius, 10, 1);

    }

    void setColor(const Pixel& c)
    {
        fBackgroundColor = c;
    }

    void moveTo(const double cx, const double cy)
    {
        fCenterX = cx;
        fCenterY = cy;
    }

    void autoMove()
    {
        fCenterX += fFlightDirection.x * fFlightSpeed;
        fCenterY += fFlightDirection.y * fFlightSpeed;
        
        //printf("automove: %f %f\n", fCenterX, fCenterY);

        // boundary test
        // if we hit a limit, reverse direction
        if ((fCenterX < 0) || (fCenterX > displayWidth))
            fFlightDirection.x *= -1;

        if ((fCenterY < 0) || (fCenterY > displayHeight))
            fFlightDirection.y *= -1;

        if (p5::seconds() > fFlightTime + fFlightDuration) {
            calculateFlightTime();
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
        ctx.fill(30);

        double segmentRads = p5::radians(360 / 12);
        double angle = segmentRads - p5::radians(90);
        double r = fRadius - 34;

        for (int i = 1; i <= 12; i++) {
            std::string Number = std::to_string(i);
            ctx.text(Number.c_str(), fCenterX + (r * cos(angle)), fCenterY + (r * sin(angle)));
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
        
        ctx.stroke(0x20);
        ctx.fill(0x20);
        ctx.text(buff, fCenterX, fCenterY + 24);
    }

    void drawBackground(IGraphics& ctx)
    {
        // fill background
        BLGradient gradient(BLRadialGradientValues(fCenterX, fCenterY, fCenterX, fCenterY, fRadius));
        gradient.addStop(0, p5::color(220, 127));       // center
        gradient.addStop(0.20, p5::color(fBackgroundColor.r(), fBackgroundColor.g(), fBackgroundColor.b(), 127));     // center
        gradient.addStop(0.80, fBackgroundColor);
        gradient.addStop(1.0, p5::color(65, 127));     // edge
        ctx.fill(gradient);
        //ctx.fill(fBackgroundColor);

        p5::noStroke();

        ctx.ellipseMode(ELLIPSEMODE::RADIUS);
        ctx.circle(fCenterX, fCenterY, fRadius);

        // draw inside radius
        // This indicates where mouse activation occurs
        p5::noStroke();
        ctx.fill(240,65);
        ctx.circle(fCenterX, fCenterY, fCenterRadius);
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
        ctx.translate(fCenterX, fCenterY);
        ctx.rotate(p5::radians(-90));

        drawHourTickmarks(ctx);
        drawSecondTickmarks(ctx);

        drawHourHand(ctx);
        drawMinuteHand(ctx);
        drawSecondsHand(ctx);
        ctx.pop();

        drawDate(ctx);

        // If the mouse down while inside us, we'll stop
        if (p5::mouseIsPressed)
        {
            double ldistance = p5::dist(p5::mouseX, p5::mouseY, fCenterX, fCenterY);

            if ((ldistance >= 1) && (ldistance < fCenterRadius))
            {
                // Move closer to the cursor location position
                double x = p5::lerp(p5::mouseX, fCenterX, 0.3);
                double y = p5::lerp(p5::mouseY, fCenterY, 0.3);
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


