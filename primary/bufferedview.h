#pragma once

#include "graphic.hpp"
#include "Surface.h"

struct BufferedView : public GraphicGroup
{
	// For drawing and composing
	User32PixelMap fPixelMap{};
	Surface fSurface{};
	bool fNeedsRedraw{ true };
	bool fSmartCache{ false };

	BufferedView(float x, float y, float w, float h)
		: GraphicGroup(x, y, w, h)

	{
		fPixelMap.init(w, h);
		fSurface.attachPixelArray(fPixelMap);

		// Set default drawing attributes
		fSurface.fill(Pixel(0, 0, 0));
		fSurface.stroke(Pixel(0, 0, 0));
		fSurface.strokeWeight(1.0);
	}

	const bool smartCache() const { return fSmartCache; }
	void smartCache(bool b) { fSmartCache = b; }

	const bool needsRedraw() const { return fNeedsRedraw; }
	void needsRedraw(bool needsIt) { fNeedsRedraw = needsIt; }

	
	void compose(IGraphics& ctx)
	{
		// make sure all drawing has completed
		fSurface.flush();

		// copy our internal pixelmap to the real context
		ctx.image(fSurface.getImage(), (int)frameX(), (int)frameY());
	}

	void applyViewport(IGraphics &ctx)
	{	
		// Apply user specified transform
		// Do the scaling around the indicated center point
		//ctx.translate(fCenterTransform.x, fCenterTransform.y);
		ctx.scale(fScale.x, fScale.y);
		//ctx.translate(-fCenterTransform.x, -fCenterTransform.y);

		// Now do the actual translation
		fSurface.translate(fTranslation.x, fTranslation.y);

	}
	
	void draw(IGraphics& ctx) override
	{

		if (!smartCache() || needsRedraw())
		{
			drawBackground(fSurface);
			
			fSurface.push();

			applyViewport(fSurface);
			
			drawSelf(fSurface);

			fSurface.pop();

			drawForeground(fSurface);

			needsRedraw(false);
		}

		compose(ctx);
	}
};
