#pragma once

#include "apphost.h"
#include "graphic.hpp"
#include "rangen.h"


namespace shapes {
	static maths::rng_state gRNGState{};

	static float random(float low, float high) noexcept
	{
		auto d = maths::rand1f(gRNGState);

		d = (d * (high - low + 1.0f)) + low;

		return d;
	}

	static Pixel  randomColor()
	{
		uint8_t r = (uint8_t)random(30, 255);
		uint8_t g = (uint8_t)random(30, 255);
		uint8_t b = (uint8_t)random(30, 255);
		uint8_t a = (uint8_t)random(0x7f, 255);

		return { r,g,b, a };
	}

	struct GCircle : public GraphicElement
	{
		static constexpr float RADIUS = 20;

		MouseEventDispatch fMouseDispatch;
		bool fIsDragging = false;

		maths::vec2f fCenter{};
		float fRadius = 20;
		Pixel fColor{};

		GCircle(maths::vec2f center = { RADIUS,RADIUS }, const float radius= RADIUS)
			:GraphicElement(center.x-radius, center.y- radius, radius *2, radius *2)
			,fCenter(center)
			,fRadius(radius)
		{
			fColor = Pixel(255, 0, 0);

			fMouseDispatch.mousePressed = [this](const MouseEvent& e) {mousePressed(e); };
			fMouseDispatch.mouseReleased = [this](const MouseEvent& e) {mouseReleased(e); };
			fMouseDispatch.mouseMoved = [this](const MouseEvent& e) {mouseMoved(e); };

			fIsDragging = false;
		}

		void mouseEvent(const MouseEvent& e)
		{
			//printf("GCircle::mouseEvent (%3.0f, %3.0f)\n", e.x, e.y);
			fMouseDispatch(e);
		}

		void mousePressed(const MouseEvent& e)
		{
			fIsDragging = true;
		}

		void mouseReleased(const MouseEvent& e)
		{
			fIsDragging = false;
		}

		void mouseMoved(const MouseEvent& e)
		{
			if (fIsDragging)
			{
				maths::vec2f ecx{ e.x,e.y };
				auto dxy = ecx - fCenter;
				moveBy(dxy);
				fCenter = ecx;
			}
			else {
				fColor = randomColor();
			}
		}

		void draw(IGraphics& ctx) override
		{
			ctx.fill(fColor);
			ctx.noStroke();
			//ctx.circle(fCenter.x, fCenter.y, fRadius);
			ctx.rect(fCenter.x-fRadius, fCenter.y-fRadius, fRadius*2, fRadius*2);
		}
	};
}
