#pragma once
/*
    A slider is a graphic that provides for some constrained motion.The constraint
        is expressed in the form of a MotionConstraint object.

        There is a 'track', which represents the motion a 'thumb' will travel over
        during user interaction.The track and constraint go hand in hand, as the track
        is a representation of the MotionConstraint.

        The constraint can express two dimensions and not just one.

        --A slider can be created with as little information as a starting
        and ending point.

        slider:create({ startPoint = {x = 10,y = 4}; endPoint = {x = 10,y = 20}; })

*/

//local GraphicGroup = require("GraphicGroup")
#include "graphic.hpp"
#include "MotionConstraint.h"
//local SliderThumb = require("SliderThumb")


#include "maths.hpp"


//
// Slider Class
//
class Slider : public virtual GraphicGroup
{
    static const int trackThickness = 4;
    double lowValue;
    double highValue;
    Vec2f position;
    
    Slider(obj)
        :GraphicGroup(obj),
        dragging(false)
    {
        //lowValue = obj.lowValue or 0
        //highValue = obj.highValue or 255
        //position = obj.position or {x = 0, y = 0};


        add(obj.thumb);
        setPosition(obj.position);
    }

    static Slider create(params)
    {
        if not params.startPoint or not params.endPoint then
            return nil, "must specify startPoint and endPoint"
            end

            local thickness = params.thickness or 24
            local startPoint = params.startPoint
            local endPoint = params.endPoint
            local dx = params.endPoint.x - params.startPoint.x;
        local dy = params.endPoint.y - params.startPoint.y;

        local orientation = "vertical"
            if abs(dx) > abs(dy) then
                orientation = "horizontal"
                end


                -- create the thumb
                local thumbParams = params.thumb or {}
        thumbParams.length = thumbParams.length or 60;
        thumbParams.thumbColor = thumbParams.thumbColor or 0x70;
        if orientation == "vertical" then
            thumbParams.frame = { x = 0; y = 0,width = thickness,height = thumbParams.length };
        else
            thumbParams.frame = { x = 0; y = 0,width = thumbParams.length,height = thickness };
        end

            local sliderThumb = SliderThumb:new(thumbParams)


            //Now figure out the frame of the entire slider
            local sliderFrame
            local sliderContraint
            local sliderStart
            local sliderEnd

            if orientation == "vertical" then
                sliderFrame = { x = startPoint.x - thickness / 2, y = startPoint.y, width = thickness, height = abs(dy) }
                sliderConstraint = MotionConstraint:new({
                    minX = 0, maxX = 0,
                    minY = 0, maxY = sliderFrame.height - sliderThumb.frame.height })
                    sliderStart = { x = thickness / 2, y = 0 };
        sliderEnd = { x = thickness / 2, y = sliderFrame.height }
            else
                sliderFrame = { x = startPoint.x, y = startPoint.y - thickness / 2, width = abs(dx), height = thickness }
                sliderConstraint = MotionConstraint:new({
                    minX = 0, maxX = sliderFrame.width - sliderThumb.frame.width,
                    minY = 0, maxY = 0 })
                    sliderStart = { x = 0,y = thickness / 2 };
        sliderEnd = { x = sliderFrame.width, y = thickness / 2 };
        end


            local sliderParams = {
                title = params.title,
                trackColor = params.trackColor or color(0xff,0,0);
                position = {x = 0,y = 0};
                startPoint = sliderStart;
                endPoint = sliderEnd;
                frame = sliderFrame;
                constraint = sliderConstraint;
                thumb = sliderThumb;
        }

        local slider = Slider:new(sliderParams);

        return slider;
    }

    void drawBackground(IGraphics *ctx)
    {
        //print("slider.drawBackground: ", self.frame.x, self.frame.y, self.frame.width, self.frame.height)
        //draw line between endpoints
        ctx->strokeWeight(Slider::trackThickness);
        ctx->stroke(120);
        //ctx : rect(self.frame.x, self.frame.y, self.frame.width, self.frame.height)
        ctx->line(self.startPoint.x, self.startPoint.y, self.endPoint.x, self.endPoint.y);

        //draw a couple of circles at the ends
        ctx->noStroke();
        ctx->fill(10);
        ctx->circle(self.startPoint.x, self.startPoint.y, (Slider::trackThickness / 2) + 2);
        ctx->circle(self.endPoint.x, self.endPoint.y, (Slider::trackThickness / 2) + 2);
    }

    //
    //    Returns a number in range[0..1]
    //
    Vec2f getPosition()
    {
        return position;
        //return map(self.thumb.frame.x, self.constraint.minX, self.constraint.maxX, 0, 1)
    }

    void setPosition(const Vec2f &pos)
    {
        position.x = constrain(pos.x, 0, 1);
        position.y = constrain(pos.y, 0, 1);

        auto locY = map(pos.y, 0, 1, constraint.minY, constraint.maxY);
        auto locX = map(pos.x, 0, 1, constraint.minX, constraint.maxX);


        thumb.moveTo(locX, locY);

        lastLocation = { thumb.frame.x, thumb.frame.y };
    }

                        function Slider.getValue(self)
                        return map(self:getPosition(), 0, 1, self.lowValue, self.highValue);
                    end

                        function Slider.changeThumbLocation(self, change)
                        local movement = self.constraint:tryChange(self.thumb.frame, change)
                        --print("movement: ", movement.dx, movement.dy)

                        self.thumb : moveBy(movement.dx, movement.dy)

                        local position = self.constraint : calcPosition(self.thumb.frame)
                        self.position = position;

                    --tell anyone who's interested that something has changed
                        signalAll(self, self, "changeposition")
                        end

                        function Slider.mouseDown(self, event)
                        self.dragging = true;
                    self.lastLocation = { x = event.x, y = event.y };
                    end

                        function Slider.mouseUp(self, event)
                        self.dragging = false;
                    end

                        function Slider.mouseMove(self, event)
                        --print("Slider.mouseMove: ", event.x, event.y, self.dragging)

                        if self.dragging then
                            local change = {
                                dy = event.y - self.lastLocation.y;
                                dx = event.x - self.lastLocation.x;
                        }
                        self:changeThumbLocation(change)
                            end
                            self.lastLocation = { x = event.x, y = event.y }
                            end

                            return Slider

