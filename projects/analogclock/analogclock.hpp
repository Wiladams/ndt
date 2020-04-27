#pragma once

#include "p5.hpp"

#include <string>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <cstdio>

//using namespace std;
using namespace p5;


static const std::string WeekDaysLong[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

static const std::string WeekDaysShort[] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
};

static const std::string MonthsShort[] = {
    "",
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
};


class AnalogClock {

    double fCenterX;
    double fCenterY;
    double fRadius;
    double fCenterRadius;
    Color fBackgroundColor;

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
    static BLSize getPreferredSize() { return BLSize(240, 240); }

    AnalogClock(double cx, double cy, double radius = 240)
    {

        fCenterX = cx;
        fCenterY = cy;
        fRadius = radius;
        fCenterRadius = 6;
        fBackgroundColor = color(0xe2, 0x84, 0x30, 0x7f);

        // For doing hand animation
        fOvershootAmount = radians(1.25);
        fOvershootRemaining = 0;
        fRecoveryIncrement = radians(1.0);
        fLastSec = 0;

        fDrivenExternally = false;

        // Flight Stuff

        GetLocalTime(&fTime);

        calculateFlightTime();
    }

    void calculateFlightTime()
    {
        // minimum flight delay, 10 seconds
        // maximum flight delay, 60 seconds
        fFlightTime = seconds() + random(10, 60);
        fFlightDuration = random(10, 60);


        fFlightDirection = BLPoint(random(-1, 1), random(-1, 1));
        fFlightSpeed = random(1, 10);
    }

    void setColor(const Color& c)
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

        if (seconds() > fFlightTime + fFlightDuration) {
            calculateFlightTime();
        }
    }

    void drawSecondTickmarks(IGraphics &ctx)
    {
        ctx.push();

        double segmentRads = radians(360 / 60);

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

    std::string ITOA(int Number) {
        std::ostringstream convert;
        convert << Number;
        return convert.str();
    }

    void drawHourNumbers(IGraphics& ctx)
    {
        ctx.push();

        ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
        ctx.fill(30);

        double segmentRads = radians(360 / 12);
        double angle = segmentRads - radians(90);
        double r = fRadius - 34;

        for (int i = 1; i <= 12; i++) {
            //local x = self.centerX + (r * cos(angle))
            //local y = self.centerY + (r * sin(angle))
            std::string Number = ITOA(i);
            ctx.text(Number.c_str(), fCenterX + (r * cos(angle)), fCenterY + (r * sin(angle)));
            angle = angle + segmentRads;
        }

        ctx.pop();
    }
                    
        
    void drawHourTickmarks(IGraphics& ctx)
    {
        ctx.push();
        double segmentRads = radians(360 / 12);

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
        double minuteContribution = radians((double)fTime.wMinute / (60.0 * 30.0));
        double hourRads = radians(map(fTime.wHour % 12, 0, 11, 0, 330)) + minuteContribution;
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
        double minuteRads = radians(map(fTime.wMinute, 0, 59, 0, 354));
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

        //Do the rotation thing
        // translate(self.centerX, self.centerY)
        double secRads = radians(map(fTime.wSecond, 0, 59, 0, 354));
        if (fTime.wSecond != fLastSec) {
            fOvershootRemaining = fOvershootAmount;
            fLastSec = fTime.wSecond;
        }

        secRads = secRads + fOvershootRemaining;

        fOvershootRemaining = constrain(fOvershootRemaining - fRecoveryIncrement, 0, fOvershootAmount);


        ctx.rotate(secRads);

        //Draw the indicator line
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
            WeekDaysShort[fTime.wDayOfWeek].c_str(), 
            MonthsShort[fTime.wMonth].c_str(), 
            fTime.wDay);
        
        ctx.stroke(0x20);
        ctx.fill(0x20);
        ctx.text(buff, fCenterX, fCenterY + 24);
    }

    void drawBackground(IGraphics& ctx)
    {
        // fill background
        ctx.ellipseMode(ELLIPSEMODE::RADIUS);
        ctx.stroke(255);
        ctx.strokeWeight(2);
        //ctx.fill(210, 210, 210);
        ctx.fill(fBackgroundColor);
        ctx.circle(fCenterX, fCenterY, fRadius);

        // draw inside radius
        ctx.fill(240);
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
        ctx.rotate(radians(-90));

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
            double ldistance = dist(mouseX, mouseY, fCenterX, fCenterY);

            if ((ldistance >= 1) && (ldistance < 70)) 
            {
                // Move closer to the cursor location position
                double x = lerp(mouseX, fCenterX, 0.3);
                double y = lerp(mouseY, fCenterY, 0.3);
                moveTo(x, y);

                calculateFlightTime();
            }
        } else {
            // otherwise, see if it's time to fly away
            if ((fFlightTime > 0) && (seconds() > fFlightTime)) {
                autoMove();
            }
        }
/*
        // calculate distance to current mouse position
        
        //printf("ldistance: %f\n", ldistance);
        // if the distance is really close, then alter our center to match that of the mouse
        // that will make us a but stick to the mouse position if they don't move the mouse very fast
        // and let go when they escape
        //printf("distance: %f  flightTime: %f  now: %f\n", ldistance, fFlightTime, seconds());
        if (ldistance < 70.0) {
            // draw a little targeting ellipse
            fill(255, 0, 0);
            circle(mouseX, mouseY, 10);

            // Move halfway between mouse and current position
            double x = lerp(mouseX, fCenterX, 0.3);
            double y = lerp(mouseY, fCenterY, 0.3);
            moveTo(x, y);

            // Reset flight time whenever we move
            calculateFlightTime();
        } else if ((fFlightTime > 0) && (seconds() > fFlightTime)){
            // we're flying
            autoMove();
        }
 */
    }
};


