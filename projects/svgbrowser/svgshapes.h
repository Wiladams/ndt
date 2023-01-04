#pragma once


#include "graphic.hpp"
#include "Graphics.h"
#include "xmlscan.h"
#include "svgtypes.h"
#include "cssscanner.h"

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
	

	// stroke-dasharray
	// stroke-dashoffset
	// fill-rule
	// clip-rule
	// opacity
	// filter
	// clip-path
	// mask
	
	static std::map<std::string, std::function<std::shared_ptr<SVGVisualProperty> (const std::string& , const XmlElement& )>> gSVGPropertyCreation = {
//		,{"color", [](const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml("stroke", elem); } }
		{"fill", [](const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml("fill", elem ); } }
		,{"font-size", [](const std::string& name, const XmlElement& elem) {return SVGFontSize::createFromXml("font-size", elem); } }
		,{"stop-color", [](const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml("stop-color", elem); } }
		,{"stroke", [](const std::string& name, const XmlElement& elem ) {return SVGPaint::createFromXml("stroke", elem); } }
		,{"stroke-linejoin", [](const std::string& name, const XmlElement& elem) {return SVGStrokeLineJoin::createFromXml("stroke-linejoin", elem); } }
		,{"stroke-linecap", [](const std::string& name, const XmlElement& elem ) {return SVGStrokeLineCap::createFromXml("stroke-linecap", elem); } }
		,{"stroke-miterlimit", [](const std::string& name, const XmlElement& elem ) {return SVGStrokeMiterLimit::createFromXml("stroke-miterlimit", elem); } }
		,{"stroke-width", [](const std::string& name, const XmlElement& elem ) {return SVGStrokeWidth::createFromXml("stroke-width", elem); } }
		//,{"text-align", [](const std::string& name, const XmlElement& elem) {return SVGTextAlign::createFromXml("text-align", elem); } }
		,{"text-anchor", [](const std::string& name, const XmlElement& elem) {return SVGTextAnchor::createFromXml("text-anchor", elem); } }
		,{"transform", [](const std::string& name, const XmlElement& elem) {return SVGTransform::createFromXml("transform", elem); } }
		,{"gradientTransform", [](const std::string& name, const XmlElement& elem) {return SVGTransform::createFromXml("gradientTransform", elem); } }

	};

	//
	// SVGVisualObject
	// This is any object that will change the state of the rendering context
	// that's everything from paint that needs to be applied, to geometries
	// that need to be drawn, to line widths, text alignment, and the like.
	// Most things, other than basic attribute type, will be a sub-class of this
	struct SVGGroup;
	struct IMapSVGNodes
	{
		virtual std::shared_ptr<SVGVisual> findNodeById(const std::string& name) = 0;
		virtual void addDefinition(const std::string& name, std::shared_ptr<SVGVisual> obj) = 0;

		virtual void setInDefinitions(bool indefs) = 0;
		virtual bool inDefinitions() const = 0;

	};
	
	struct SVGVisualNode : public SVGVisual
	{
		IMapSVGNodes* fRoot{ nullptr };
		std::string fId{};      // The id of the element

		std::map<std::string, std::shared_ptr<SVGVisualProperty>> fVisualProperties{};

		
		
		SVGVisualNode() = default;
		SVGVisualNode(IMapSVGNodes* root)
			: fRoot(root)
		{
		}
		


		virtual void setRoot(IMapSVGNodes* root) { fRoot = root; }

		
		const std::string& id() const { return fId; }
		void setId(const std::string& id) { fId = id; }
		
		void loadVisualProperties(const XmlElement& elem)
		{
			// Run Through the property creation routines, generating
			// properties for the ones we find in the XmlElement
			for (auto& propconv : gSVGPropertyCreation)
			{
				// get the named attribute
				auto attrName = propconv.first;

				// We have a property and value, convert to SVGVisibleProperty
				// and add it to our map of visual properties
				auto prop = propconv.second(attrName, elem);
				if (prop->isSet())
					fVisualProperties[attrName] = prop;

			}
		}

		void setCommonVisualProperties(const XmlElement &elem)
		{
			// load the common stuff that doesn't require
			// any additional processing
			loadVisualProperties(elem);

			// Handle the style attribute separately by turning
			// it into a standalone XmlElement, and then loading
			// that like a normal element, by running through the properties again
			// It's ok if there were already styles in separate attributes of the
			// original elem, because anything in the 'style' attribute is supposed
			// to override whatever was there.
			auto styleChunk = elem.getAttribute("style");

			if (styleChunk) {
				// Create an XML Element to hang the style properties on as attributes
				XmlElement styleElement{};

				// use CSSInlineIterator to iterate through the key value pairs
				// creating a visual attribute, using the gSVGPropertyCreation map
				CSSInlineStyleIterator iter(styleChunk);

				while (iter.next())
				{
					std::string name = std::string((*iter).first.fStart, (*iter).first.fEnd);
					if (!name.empty() && (*iter).second)
					{
						styleElement.addAttribute(name, (*iter).second);
					}
				}

				loadVisualProperties(styleElement);
			}

			// Deal with any more attributes that need special handling
		}

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGVisual::loadSelfFromXml(elem);
			
			auto id = elem.getAttribute("id");
			if (id)
				setId(std::string(id.fStart, id.fEnd));

			
			setCommonVisualProperties(elem);
		}
		
		// Contains styling attributes
		void applyAttributes(IGraphics& ctx)
		{
			for (auto& prop : fVisualProperties) {
				prop.second->draw(ctx);
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
	
	struct SVGShape : public SVGVisualNode
	{
		SVGShape() :SVGVisualNode(nullptr) {}
		SVGShape(IMapSVGNodes* root) :SVGVisualNode(root) {}

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGVisualNode::loadSelfFromXml(elem);
		}
		
		static std::shared_ptr<SVGShape> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGShape>();
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};

	struct SVGTemplateNode : public SVGShape
	{
		std::shared_ptr<SVGVisual> fWrappedNode{};
		std::string fWrappedID{};

		double fX = 0;
		double fY = 0;

		SVGTemplateNode() {}
		~SVGTemplateNode() {}

		void setRoot(IMapSVGNodes* root) override
		{
			SVGShape::setRoot(root);

			// Use the root to lookup the wrapped node
			if (root != nullptr && !fWrappedID.empty())
			{
				fWrappedNode = root->findNodeById(fWrappedID);
			}

		}

		void drawSelf(IGraphics& ctx) override
		{
			if (fWrappedNode == nullptr)
				return;

			
			// Apply locally generated attributes
			ctx.translate(fX, fY);
			
			// Draw the wrapped graphic
			fWrappedNode->draw(ctx);
		}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			fWrappedNode = nullptr;

			SVGShape::loadSelfFromXml(elem);

			// look for the href, or xlink:href attribute
			auto href = elem.getAttribute("href");
			if (!href)
			{
				href = chunk_trim(elem.getAttribute("xlink:href"), wspChars);
			}

			if (!href)
				return;

			fX = toDimension(elem.getAttribute("x")).calculatePixels();
			fY = toDimension(elem.getAttribute("y")).calculatePixels();

			// Use the href to lookup the node in the tree
			// have to wait for setRoot() to be called before we can do the lookup
			if (*href == '#')
			{
				href++;
			}
			fWrappedID = toString(href);
		}




		static std::shared_ptr<SVGTemplateNode> createFromXml(const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGTemplateNode>();
			shape->loadFromXmlElement(elem);

			return shape;
		}


	};
	
	struct SVGPathBasedShape : public SVGShape
	{
		BLPath fPath{};
		
		SVGPathBasedShape() :SVGShape() {}

		
		void drawSelf(IGraphics &ctx) override
		{
			ctx.path(fPath);
		}
	};
	
	struct SVGLine : public SVGPathBasedShape
	{

		BLLine fGeometry{};
		
		SVGLine() 
			:SVGPathBasedShape() 
		{ 
			reset(0, 0, 0, 0); 
		}
		
		SVGLine(float x, float y, float w, float h)
			:SVGPathBasedShape()
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
		BLRoundRect fGeometry{};
		
		SVGRect() : SVGPathBasedShape() {}
		
		
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
		BLCircle fCircle{};
		
		SVGCircle() : SVGPathBasedShape() {}
		
		
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
		
		SVGEllipse() : SVGPathBasedShape() {}
		
		
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
		SVGPolyline() : SVGPathBasedShape() {}
		
		
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
		SVGPolygon() : SVGPathBasedShape() {}
		
		
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
		SVGPath() : SVGPathBasedShape() {}

		
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

	struct SVGGradientStop : public SVGShape
	{
		double fOffset = 0.0f;
		
		SVGGradientStop() {}

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGVisualNode::loadSelfFromXml(elem);

			fOffset = toDimension(elem.getAttribute("offset")).calculatePixels(96);

		}
		
		static std::shared_ptr<SVGGradientStop> createFromXml(const XmlElement& elem)
		{
			auto stop = std::make_shared<SVGGradientStop>();
			stop->loadFromXmlElement(elem);

			return stop;
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
	{"use", [](const XmlElement& elem) { return SVGTemplateNode::createFromXml(elem); }},
	{"stop", [](const XmlElement& elem) { return SVGGradientStop::createFromXml(elem); }},

	};


	struct SVGCompoundNode : public SVGShape
	{
		static constexpr int BUILD_STATE_INITIAL = 0;
		static constexpr int BUILD_STATE_OPEN = 1;
		static constexpr int BUILD_STATE_CLOSE = 2;
		
		
		std::vector<std::shared_ptr<SVGVisual>> fNodes{};
		std::map<std::string, std::shared_ptr<SVGVisual>> fDefinitions;
		int buildState = BUILD_STATE_OPEN;
		
		SVGCompoundNode() : SVGShape() {}
		SVGCompoundNode(IMapSVGNodes* root) :SVGShape(root) {}
		
		virtual void addNode(std::shared_ptr < SVGVisualNode > node)
		{
			if (node == nullptr)
				return;

			node->setRoot(fRoot);

			fNodes.push_back(node);
		}
		
		virtual void loadSelfClosingNode(const XmlElement& elem)
		{
			auto it = gShapeCreationMap.find(elem.name());
			if (it != gShapeCreationMap.end())
			{
				auto node = it->second(elem);
				addNode(node);
			}
		}
		
		virtual void loadContentNode(const XmlElement& elem)
		{
			// Do something with content nodes	
		}
		
		virtual void loadCompoundNode(XmlElementIterator& iter)
		{
			auto node = std::make_shared<SVGCompoundNode>(fRoot);
			node->loadFromIterator(iter);
			addNode(node);
		}
		
		virtual void loadFromIterator(ndt::XmlElementIterator& iter)
		{
			// First, loadSelfFromXml because we're sitting on our opening element
			// and we need to gather our own attributes
			loadSelfFromXml(*iter);

			buildState = BUILD_STATE_OPEN;
			
			while (iter && (buildState != BUILD_STATE_CLOSE))
			{
				// move iterator forward to get to next item
				iter++;

				const ndt::XmlElement& elem = *iter;

				// BUGBUG - debug
				//ndt_debug::printXmlElement(elem);

				if (!elem)
					break;

				// Skip over these node types we don't know how to process


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
						loadSelfClosingNode(elem);
					}
					else if (elem.isStart())
					{
						loadCompoundNode(iter);
					}
					else if (elem.isEnd())
					{
						// Close the current element
						buildState = BUILD_STATE_CLOSE;
					}
					else if (elem.isContent())
					{
						loadContentNode(elem);
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
		
		static std::shared_ptr<SVGCompoundNode> createFromIterator(XmlElementIterator& iter)
		{
			auto node = std::make_shared<SVGCompoundNode>();
			node->loadFromIterator(iter);

			return node;
		}
	};

	struct SVGTextNode :public SVGCompoundNode
	{
		double x;
		double y;
		std::string fText;
		
		SVGTextNode() :SVGCompoundNode() {}
		SVGTextNode(IMapSVGNodes* root) :SVGCompoundNode(root) {}

		void drawSelf(IGraphics& ctx) override
		{
			ctx.textFont("Consolas");	// BUGBUG - hardcoded, should go away when property supported

			ctx.text(fText.c_str(), x, y);
		}
		
		virtual void loadSelfFromXml(const XmlElement& elem)
		{
			SVGCompoundNode::loadSelfFromXml(elem);

			x = toDimension(elem.getAttribute("x")).calculatePixels(96);
			y = toDimension(elem.getAttribute("y")).calculatePixels(96);
			//fFontSize = toDimension(elem.getAttribute("font-size")).calculatePixels(96);
		}

		void loadContentNode(const XmlElement& elem) override
		{
			// Do something with content nodes	
			fText = toString(elem.data());
		}
	};
	
	struct SVGGradient : public SVGCompoundNode
	{
		BLMatrix2D fGradientTransform;
		BLGradient fGradient;
		BLVar fGradientVar;
		
		SVGGradient() :SVGCompoundNode() {}
		SVGGradient(IMapSVGNodes* root) :SVGCompoundNode(root) {}
		
		operator BLVar & const ()
		{
			return fGradientVar;
		}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGVisualNode::loadSelfFromXml(elem);
			
			double x1 = toDimension(elem.getAttribute("x1")).calculatePixels(96);
			double x2 = toDimension(elem.getAttribute("x2")).calculatePixels(96);
			double y1 = toDimension(elem.getAttribute("y1")).calculatePixels(96);
			double y2 = toDimension(elem.getAttribute("y2")).calculatePixels(96);
			
			// gradientTransform should have been read in as commonAttribute
			// gradientUnits needs to be read in here
			BLLinearGradientValues gradientValues = { x1, y1, x2, y2 };
			fGradient = BLGradient(gradientValues);
			fGradientVar = fGradient;
		}

		void addNode(std::shared_ptr < SVGVisualNode > node) override
		{
			// The only nodes that should be added here are stoops
			// so we can read them, and turn them into BLGradientStops
			if (node == nullptr)
				return;
			
			//if (elem.isComment() || elem.isContent() || elem.isProcessingInstruction())
			//{
			//	continue;
			//}
			
			double offset = 0;
			BLRgba32 c{};
			
			fGradient.addStop(offset, c);

			
		}

		void drawSelf(IGraphics& ctx) override
		{
			
		}
	};
	
	
	class SVGGroup : public SVGCompoundNode, public IMapSVGNodes
	{
	protected:
		
		bool fInDefinitions{ false };
		std::map<std::string, std::shared_ptr<SVGVisual>> fDefinitions;

	public:
		SVGGroup() :SVGCompoundNode(nullptr) {}
		SVGGroup(IMapSVGNodes* root): SVGCompoundNode(root){}


		bool inDefinitions() const override { return fInDefinitions; }
		void setInDefinitions(bool indefs) override { fInDefinitions = indefs; };

		std::shared_ptr<SVGVisual> findNodeById(const std::string& name)
		{
			if (fRoot == this)
				return fDefinitions[name];
			else if (fRoot)
				return fRoot->findNodeById(name);
			
			return nullptr;
		}
		
		void addNode(std::shared_ptr < SVGVisualNode > node) override
		{
			if (node == nullptr)
				return;
			
			node->setRoot(fRoot);

			// Whether we're in definition mode or not,
			// if the visual has an ID, then add the node
			// to our definitions map.
			if (!node->id().empty())
				addDefinition(node->id(), node);

			// If we're not in definitions mode, then 
			// also add the node to the visual nodes list
			if (!inDefinitions())
			{
				fNodes.push_back(node);
			}

		}

		void addDefinition(const std::string& name, std::shared_ptr<SVGVisual> obj)
		{
			if (fRoot == this)
				fDefinitions[name] = obj;
			else if (fRoot != nullptr)
				fRoot->addDefinition(name, obj);
			
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

		void loadSelfClosingNode(const XmlElement& elem) override
		{
			auto it = gShapeCreationMap.find(elem.name());
			if (it != gShapeCreationMap.end())
			{
				auto node = it->second(elem);
				addNode(node);
			}
		}
		
		void loadCompoundNode(XmlElementIterator& iter) override
		{
			const ndt::XmlElement& elem = *iter;
			
			//if (elem.isComment() || elem.isContent() || elem.isProcessingInstruction())
			//{
			//	return;
			//}
			
			// Add a child, and call loadIterator
			// the only known types are 
			// gradient
			// g
			if (elem.name() == "g")
			{
				auto node = std::make_shared<SVGGroup>(fRoot);
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "defs")
			{
				setInDefinitions(true);
				auto node = std::make_shared<SVGGroup>(fRoot);
				node->loadFromIterator(iter);
				addNode(node);
				setInDefinitions(false);
			}
			else if (elem.name() == "text")
			{
				auto node = std::make_shared<SVGTextNode>(fRoot);
				node->loadFromIterator(iter);
				addNode(node);
			}
			else {
				auto node = std::make_shared<SVGGroup>(fRoot);
				node->loadFromIterator(iter);
				addNode(node);
			}
		}
		/*
		// Take the XmlElements from the iterator, and turn them into shapes
		void loadFromIterator(ndt::XmlElementIterator& iter) override
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

				// BUGBUG - debug
				//ndt_debug::printXmlElement(elem);

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
						loadSelfClosingNode(elem);
					}
					else if (elem.isStart())
					{
						loadCompoundNode(iter);
					}
					else if (elem.isEnd())
					{
						//if (elem.name() == "defs")
						//{
						//	setInDefinitions(false);
						//}

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
		*/
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

		SVGRootNode() :SVGGroup(nullptr) { setRoot(this); }
		SVGRootNode(IMapSVGNodes *root)
			: SVGGroup(root)
		{
			setRoot(this);
		}
		

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGGroup::loadSelfFromXml(elem);
			
			// width
			fWidth = toDimension(elem.getAttribute("width")).calculatePixels(96);
			fHeight = toDimension(elem.getAttribute("height")).calculatePixels(96);

			// viewbox
			fViewbox = SVGViewbox::createFromXml(elem, "viewBox");

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
			ctx.fill(Pixel(0, 0, 0));
			ctx.strokeWeight(1.0);
			ctx.textSize(16);
			
			// Apply attributes that have been gathered
			// in the case of the root node, it's mostly the viewport
			applyAttributes(ctx);

			// Draw the children
			drawSelf(ctx);

			ctx.pop();
		}

	};
	
	
	std::map<std::string, std::function<std::shared_ptr<SVGGroup>(XmlElementIterator& iter)>> gGroupCreationMap = {
		{"g", [](XmlElementIterator& iter) { return SVGGroup::createFromIterator(iter); }},
		{"svg", [](XmlElementIterator& iter) { return SVGRootNode::createFromIterator(iter); }},
		{"defs", [](XmlElementIterator& iter) { return SVGRootNode::createFromIterator(iter); }},
	};


	
}
