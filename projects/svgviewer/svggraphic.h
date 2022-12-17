#pragma once

#include "graphic.hpp"
#include "datachunk.h"
#include "svgparser.h"


namespace svg
{

	/*
	        char id[64];				// Optional 'id' attr of the shape or its group
        SVGpaint fill;				// Fill paint
        SVGpaint stroke;			// Stroke paint
        float strokeWidth;			// Stroke width (scaled).
        float strokeDashOffset;		// Stroke dash offset (scaled).
        float strokeDashArray[8];			// Stroke dash array (scaled).
        char strokeDashCount;				// Number of dash values in dash array.
        char strokeLineJoin;		// Stroke join type.
        char strokeLineCap;			// Stroke cap type.
        float miterLimit;			// Miter limit
        char fillRule;				// Fill rule, see NSVGfillRule.
        unsigned char flags;		// Logical or of NSVG_FLAGS_* flags
        float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
        SVGpath* paths;			    // Linked list of paths in the image.
        struct SVGshape* next;		// Pointer to next shape, or NULL if last element.

	*/
	struct SVGGraphicShape : public GraphicElement
	{
		std::string fID;
		BLVar fFill;
		BLVar fStroke;
		float fOpacity;
		float fStrokeWidth;
		float fStrokeDashOffset;
		std::vector<float> fStrokeDashArray;
		size_t fStrokeDashCount;
		BLStrokeJoin fStrokeLineJoin;
		BLStrokeCap fStrokeLineCap;
		float fMiterLimit;
		BLFillRule fFillRule;
		uint8_t fFlags;
		
		BLPath fMainPath;
		
		bool initFromSVGShape(SVGshape& ashape)
		{
			fID = ashape.id;
			fOpacity = ashape.opacity;
			fStrokeWidth = ashape.strokeWidth;
			fStrokeDashOffset = ashape.strokeDashOffset;
			fStrokeDashCount = ashape.strokeDashCount;
			for (size_t i = 0; i < fStrokeDashCount; i++)
			{
				fStrokeDashArray.push_back(ashape.strokeDashArray[i]);
			}
			fStrokeLineJoin = (BLStrokeJoin)ashape.strokeLineJoin;
			fStrokeLineCap = (BLStrokeCap)ashape.strokeLineCap;
			fMiterLimit = ashape.miterLimit;
			fFillRule = (BLFillRule)ashape.fillRule;
			fFlags = ashape.flags;
			rectf shapeBounds = rectf(ashape.bounds[0], ashape.bounds[1], ashape.bounds[2]-ashape.bounds[0], ashape.bounds[3]-ashape.bounds[1]);

			// create a path for each of the paths
			SVGpath* path = ashape.paths;
			while (path != nullptr)
			{
				BLPath apath;
				apath.moveTo(path->pts[0], path->pts[1]);
				for (size_t i = 2; i < ((path->npts*2) - 3); i += 6)
				{
					apath.cubicTo(path->pts[i+0], path->pts[i+1], path->pts[i+2], path->pts[i+3], path->pts[i+4], path->pts[i+5]);
				}
				
				if (path->closed)
				{
					apath.close();
				}
				fMainPath.addPath(apath);
				
				path = path->next;
			}
			fMainPath.close();
			
			setBounds(shapeBounds);
			
			return true;
		}

		void drawBackground(IGraphics& ctx)
		{
			// setup the fill and stroke	
		}
		
		void drawSelf(IGraphics& ctx)
		{
			// Draw the path
			ctx.stroke(0);
			ctx.path(fMainPath);
		}
	};
	
	struct SVGGraphic : public GraphicElement
	{
		BLPath fMainPath;
		
		SVGGraphic(const maths::rectf& fr)
			:GraphicElement(fr)
		{

		}

		void drawBackground(IGraphics& ctx) override
		{
			// setup the fill and stroke	
			ctx.stroke(0);
			ctx.noFill();
		}
		
		void draw(IGraphics &ctx) override
		{
			drawBackground(ctx);
			
			// Draw the path
			ctx.path(fMainPath);
		}
			
		bool initFromChunk(const DataChunk& srcChunk)
		{
			SVGParser p;
			svg::parseSVGDocument(srcChunk, p);

			// Now we have a parsed svg
			// use the svgp.image to create a graphic
			// Get the bouns of the image
			maths::rectf bnds{ 0,0,p.imageWidth, p.imageHeight };
			setBounds(bnds);

			fMainPath = p.fMainPath;
			
			/*
			// Go through the shapes, turning each one into an SVGGraphicShape
			SVGshape* shape = p.image->shapes;
			while (shape != nullptr)
			{
				auto g = std::make_shared<SVGGraphicShape>();
				g->initFromSVGShape(*shape);

				addGraphic(g);

				shape = shape->next;
			}
			*/
			return true;
		}
	};
}

