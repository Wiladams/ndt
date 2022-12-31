#pragma once


#include "graphic.hpp"
#include "Graphics.h"
#include "xmlscan.h"
#include "svgtypes.h"

#include <string>

// Common Shapes
// Line
// Rect
// Circle
// Ellipse
// Polyline
// Polygon
// Path
// Text

// Shape Common Attributes
// id
// lang, xml::lang
// tabindex
// data-*
// BUGBUG- Need nullable types for cascading attributes

namespace svg {
	using namespace ndt;
	
	struct SVGObject
	{
		std::string fName{};    // The tag name of the element
		std::string fId{};      // The id of the element
		//std::map<std::string, SVGAttribute> fAttributes;

		//const std::map<std::string, SVGAttribute> & attributes() const { return fAttributes; }

		const std::string & name() const { return fName; }
		void setName(const std::string &name) { fName = name; }

		const std::string& id() const { return fId; }
		void setId(const std::string &id) { fId = id; }

		virtual void loadSelfFromXml(const XmlElement& elem)
		{
			setName(elem.name());
			auto id = elem.getAttribute("id");
			if (id)
				setId(std::string(id.fStart, id.fEnd));
		}

		virtual void loadFromXmlElement(const ndt::XmlElement& elem)
		{
			// load the common attributes


			// call to loadselffromxml
			// so sub-class can do its own loading
			loadSelfFromXml(elem);
		}
	};
	
	//
	// SVGVisualObject
	// This is any object that will change the state of the rendering context
	// that's everything from paint that needs to be applied, to geometries
	// that need to be drawn, to line widths, text alignment, and the like.
	// Most things, other than basic attribute type, will be a sub-class of this
	struct SVGVisual : public SVGObject, public IDrawable
	{
		// Transform
		bool fHasTransform{ false };
		BLMatrix2D fTransform{};
		
		// Filling 
		bool fHasFill{ false };
		bool fHasExplicitNoFill{ false };
		
		vec4b fFillColor{};
		BLVar fFillPaint{};

		// Stroking
		bool fHasStroke{ false };
		bool fHasExplicitNoStroke{ false };
		
		vec4b fStrokeColor{};
		BLVar fStrokePaint{};

		std::string fStrokeLineCap{};
		std::string fStrokeLineJoin{};
		float fStrokeMiterLimit;
		float fStrokeWidth;
		
		bool hasTransform() const { return fHasTransform; }
		bool hasFill() const { return fHasFill; }
		bool hasExplicitNoFill() const { return fHasExplicitNoFill; }
		
		bool hasStroke() const { return fHasStroke; }
		bool hasExplicitNoStroke() const { return fHasExplicitNoStroke; }

		

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGObject::loadSelfFromXml(elem);
			
			// Transformation matrix
			fTransform = BLMatrix2D::makeIdentity();
			auto tform = elem.getAttribute("transform");
			if (tform)
			{
				fTransform = toTransform(tform);
				fHasTransform = true;
			}
			
			// Load up on all the visuals
			auto fill = elem.getAttribute("fill");
			if (fill)
			{
				if (fill == "none") {
					fHasFill = true;
					fHasExplicitNoFill = true;
				}
				else {
					fHasFill = true;
					fHasExplicitNoFill = false;
					fFillColor = svg::toColor(fill);
					blVarAssignRgba32(&fFillPaint, fFillColor.value);
				}
			}
			else {
				// Fill not specified, so parent fill cascades
				fHasFill = false;
				fHasExplicitNoFill = false;
			}
			
			auto stroke = elem.getAttribute("stroke");
			if (stroke) {
				if (stroke == "none") {
					fHasExplicitNoStroke = true;
					fHasStroke = true;
				}
				else {
					fHasStroke = true;
					fHasExplicitNoStroke = false;
					fStrokeColor = svg::toColor(stroke);
					blVarAssignRgba32(&fStrokePaint, fStrokeColor.value);
				}
			}
			else {
				fHasStroke = false;
				fHasExplicitNoStroke = false;
			}
			
			// load any transforms
			// stroke-width
			auto awidth = elem.getAttribute("stroke-width");
			if (awidth) {
				fStrokeWidth = svg::toNumber(awidth);
			}
			else {
				fStrokeWidth = 1.0f;
			}

			
			// stroke-linecap
			// stroke-linejoin
			// stroke-miterlimit
			// stroke-dasharray
			// stroke-dashoffset
			// fill-rule
			// clip-rule
			// opacity
			// fill-opacity
			// stroke-opacity
			// filter
			// clip-path
			// mask
			
		}
		
		// Contains styling attributes
		void applyAttributes(IGraphics& ctx)
		{
			// Transform
			//double tform[6] = { 1.7656463, 0, 0, 1.7656463, 20, 10 };
						//ctx.transform(tform);
			if (hasTransform())
			ctx.transform(fTransform.m);

			
			// fill
			if (hasFill())
			{
				if (hasExplicitNoFill())
				{
					ctx.noFill();
				}
				else
				{
					ctx.fill(fFillPaint);
				}
			}

			// stroke
			if (hasStroke())
			{
				ctx.strokeWeight(fStrokeWidth);
				
				if (hasExplicitNoStroke())
				{
					ctx.noStroke();
				}
				else
				{
					ctx.stroke(fStrokePaint);
				}
			}
		}
		
		virtual void drawSelf(IGraphics& ctx)
		{
			;

		}
		
		void draw(IGraphics& ctx) override
		{
			ctx.push();
			
			applyAttributes(ctx);

			drawSelf(ctx);

			ctx.pop();
		}
	};
	
	struct SVGShape : public SVGVisual
	{

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGVisual::loadSelfFromXml(elem);
		}
		

		

		static std::shared_ptr<SVGShape> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGShape>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};

	struct SVGPathBasedShape : public SVGShape
	{
		BLPath fPath;
		
		SVGPathBasedShape() : fPath() {}
		
		void drawSelf(IGraphics &ctx) override
		{
			ctx.path(fPath);
		}
	};
	
	struct SVGLine : public SVGPathBasedShape
	{

		BLLine fGeometry{};
		
		SVGLine() { reset(0, 0, 0, 0); }
		SVGLine(float x, float y, float w, float h)
		{
			reset(x, y, w, h);
		}
		
		void reset(float x, float y, float w, float h)
		{
			fPath.clear();
			fGeometry = BLLine(x, y, w, h);
			fPath.addLine(fGeometry);
		}
		
		void loadSelfFromXml(const XmlElement& elem) override 
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fGeometry.x0 = toDimension(elem.getAttribute("x1")).calculatePixels();
			fGeometry.y0 = toDimension(elem.getAttribute("y1")).calculatePixels();
			fGeometry.x1 = toDimension(elem.getAttribute("x2")).calculatePixels();
			fGeometry.y1 = toDimension(elem.getAttribute("y2")).calculatePixels();

			fPath.addLine(fGeometry);
		}

		static std::shared_ptr<SVGLine> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGLine>();
			shape->loadFromXmlElement(elem);

			return shape;
		}

		
	};
	
	struct SVGRect : public SVGPathBasedShape
	{
		BLRoundRect fGeometry;
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fGeometry.x = toDimension(elem.getAttribute("x")).calculatePixels();
			fGeometry.y = toDimension(elem.getAttribute("y")).calculatePixels();
			fGeometry.w = toDimension(elem.getAttribute("width")).calculatePixels();
			fGeometry.h = toDimension(elem.getAttribute("height")).calculatePixels();

			if (elem.getAttribute("rx"))
			{
				fGeometry.rx = toDimension(elem.getAttribute("rx")).calculatePixels();
				fGeometry.ry = toDimension(elem.getAttribute("ry")).calculatePixels();
				fPath.addRoundRect(fGeometry);
			}
			else
			{
				fPath.addRect(fGeometry.x, fGeometry.y, fGeometry.w, fGeometry.h);
			}
		}
		
		static std::shared_ptr<SVGRect> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGRect>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGCircle : public SVGPathBasedShape
	{
		BLCircle fCircle;
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fCircle.cx = toDimension(elem.getAttribute("cx")).calculatePixels();
			fCircle.cy = toDimension(elem.getAttribute("cy")).calculatePixels();
			fCircle.r = toDimension(elem.getAttribute("r")).calculatePixels();

			fPath.addCircle(fCircle);
		}

		static std::shared_ptr<SVGCircle> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGCircle>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGEllipse : public SVGPathBasedShape
	{
		BLEllipse fGeometry;
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fGeometry.cx = toDimension(elem.getAttribute("cx")).calculatePixels();
			fGeometry.cy = toDimension(elem.getAttribute("cy")).calculatePixels();
			fGeometry.rx = toDimension(elem.getAttribute("rx")).calculatePixels();
			fGeometry.ry = toDimension(elem.getAttribute("ry")).calculatePixels();

			fPath.addEllipse(fGeometry);
		}

		static std::shared_ptr<SVGEllipse> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGEllipse>();
			shape->loadFromXmlElement(elem);
			
			return shape;
		}
	};
	
	struct SVGPolyline : public SVGPathBasedShape
	{
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			auto points = elem.getAttribute("points");
			auto pts = toPoints(points);

			fPath.moveTo(pts[0].fX, pts[0].fY);
			for (int i = 1; i < pts.size(); i++)
			{
				fPath.lineTo(pts[i].fX, pts[i].fY);
			}

			
		}

		static std::shared_ptr<SVGPolyline> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGPolyline>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGPolygon : public SVGPathBasedShape
	{

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			auto points = elem.getAttribute("points");
			auto pts = toPoints(points);

			fPath.moveTo(pts[0].fX, pts[0].fY);
			for (int i = 1; i < pts.size(); i++)
			{
				fPath.lineTo(pts[i].fX, pts[i].fY);
			}
			fPath.close();
			
		}

		static std::shared_ptr<SVGPolygon> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGPolygon>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGPath : public SVGPathBasedShape
	{

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			auto d = elem.getAttribute("d");
			auto success = blPathFromCommands(d, fPath);
		}

		static std::shared_ptr<SVGPath> createFromXml(const XmlElement& elem)
		{
			auto path = std::make_shared<SVGPath>();
			path->loadFromXmlElement(elem);
			
			return path;
		}
	};


	std::map<std::string, std::function<std::shared_ptr<SVGShape>(const XmlElement& elem)>> gShapeCreationMap = {
	{"line", [](const XmlElement& elem) { return SVGLine::createFromXml(elem); }},
	{"rect", [](const XmlElement& elem) { return SVGRect::createFromXml(elem); }},
	{"circle", [](const XmlElement& elem) { return SVGCircle::createFromXml(elem); }},
	{"ellipse", [](const XmlElement& elem) { return SVGEllipse::createFromXml(elem); }},
	{"polyline", [](const XmlElement& elem) { return SVGPolyline::createFromXml(elem); }},
	{"polygon", [](const XmlElement& elem) { return SVGPolygon::createFromXml(elem); }},
	{"path", [](const XmlElement& elem) { return SVGPath::createFromXml(elem); }},


	};


	
		
	class SVGGroup : public SVGShape
	{
	protected:
		SVGGroup* fRoot;

		std::vector<std::shared_ptr<SVGVisual>> fNodes;
		std::map<std::string, SVGObject&> fDefinitions;

	public:
		SVGGroup(SVGGroup* root=nullptr)
			: fRoot(root)
		{
		}

		void setRoot(SVGGroup* root)
		{
			fRoot = root;
		}


		void addNode(std::shared_ptr < SVGVisual > node)
		{
			fNodes.push_back(node);
		}


		void drawSelf(IGraphics& ctx) override
		{
			for (auto& node : fNodes) {
				node->draw(ctx);
			}
		}


		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGShape::loadSelfFromXml(elem);
		}

		// Take the XmlElements from the iterator, and turn them into shapes
		virtual void loadFromIterator(ndt::XmlElementIterator& iter)
		{
			constexpr int BUILD_STATE_INITIAL = 0;
			constexpr int BUILD_STATE_OPEN = 1;
			constexpr int BUILD_STATE_CLOSE = 2;

			int buildState = BUILD_STATE_OPEN;
			int elementDepth = 0;

			// First, loadSelfFromXml because we're sitting on our opening element
			loadSelfFromXml(*iter);

			while (iter && (buildState != BUILD_STATE_CLOSE))
			{
				// move iterator forward to get to next item
				iter++;

				const ndt::XmlElement& elem = *iter;

				if (!elem)
					break;

				// Skip over these node types we don't know how to process
				if (elem.isComment() || elem.isContent() || elem.isProcessingInstruction())
				{
					continue;
				}

				switch (buildState)
				{

					
				// We are in an open element.  There are a couple of actions
				// that are valid from here
				// 1) We get another open - add a child, call loadIterator
				// 2) We get a self closing - add a child, call loadXmlElement
				// 3) We get a close - close the current element
				// 4) anything else, just ignore it
				case BUILD_STATE_OPEN:
				{
					if (elem.isSelfClosing()) {
						// this will take care of adding mose self/closing geometry elements
						// of course they can be specified with a closing tag as well
						// but that's less common
						// BUGBUG - revisit
						if (gShapeCreationMap.contains(elem.name()))
						{
							auto shape = gShapeCreationMap[elem.name()](elem);
							addNode(shape);
						}
						else {
							// do nothing, we don't know what the node is
						}
					}
					else if (elem.isStart())
					{
						// Add a child, and call loadIterator
						// the only known types are 
						// gradient
						// g
						if (elem.name() == "g")
						{
							auto child = std::make_shared<SVGGroup>(fRoot);
							addNode(child);
							child->loadFromIterator(iter);
						}
						else {
							auto node = std::make_shared<SVGGroup>(fRoot);
							addNode(node);
							node->loadFromIterator(iter);
						}

					}
					else if (elem.isEnd())
					{
						// Close the current element
						buildState = BUILD_STATE_CLOSE;
					}
					else
					{
						// Ignore anything else
						printf("IGNORING: %s\n", elem.name().c_str());
						//printXmlElement(elem);
					}
				}
				break;
				
				}	// end of switch


			}
		}

		static std::shared_ptr<SVGGroup> createFromIterator(XmlElementIterator& iter)
		{
			auto node = std::make_shared<SVGGroup>();
			node->loadFromIterator(iter);

			return node;
		}
	};

	struct SVGRootNode : public SVGGroup
	{
		float fWidth;
		float fHeight;
		SVGViewbox fViewbox;
		bool fPreserveAspectRatio = false;

		
		SVGRootNode()
			: SVGGroup(nullptr)
		{
		}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGGroup::loadSelfFromXml(elem);
			
			// width
			fWidth = toDimension(elem.getAttribute("width")).calculatePixels(96);
			fHeight = toDimension(elem.getAttribute("height")).calculatePixels(96);

			// viewbox
			fViewbox = toViewbox(elem.getAttribute("viewBox"));

			// preserveAspectRatio

		}

		static std::shared_ptr<SVGRootNode> createFromIterator(XmlElementIterator& iter)
		{
			auto node = std::make_shared<SVGRootNode>();
			node->loadFromIterator(iter);

			return node;
		}

		void drawSelf(IGraphics& ctx) override
		{
			SVGGroup::drawSelf(ctx);
		}

		void draw(IGraphics& ctx) override
		{
			ctx.push();

			// Start with default state
			ctx.noStroke();
			//ctx.noFill();
			ctx.fill(Pixel(0, 0, 0));
			ctx.strokeWeight(1.0);

			applyAttributes(ctx);

			drawSelf(ctx);

			ctx.pop();
		}

	};
	
	
	std::map<std::string, std::function<std::shared_ptr<SVGGroup>(XmlElementIterator& iter)>> gGroupCreationMap = {
		{"g", [](XmlElementIterator& iter) { return SVGGroup::createFromIterator(iter); }},
		{"svg", [](XmlElementIterator& iter) { return SVGRootNode::createFromIterator(iter); }},
	};


	
}
