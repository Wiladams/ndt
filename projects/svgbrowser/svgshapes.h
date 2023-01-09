#pragma once


#include "graphic.hpp"
#include "Graphics.h"
#include "xmlscan.h"
#include "svgtypes.h"
#include "cssscanner.h"
#include "base64.h"
#include "stb_image.h"

#include <string>
#include <array>

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
	
	static std::map<std::string, std::function<std::shared_ptr<SVGVisualProperty> (IMapSVGNodes *root, const std::string& , const XmlElement& )>> gSVGPropertyCreation = {
//		,{"color", [](IMapSVGNodes *root, const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml(root, "stroke", elem); } }
		{"fill", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml(root, "fill", elem ); } }
		,{"fill-rule", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGFillRule::createFromXml(root, "fill-rule", elem); } }
		,{"font-size", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGFontSize::createFromXml(root, "font-size", elem); } }
		//,{"stop-color", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGPaint::createFromXml(root, "stop-color", elem); } }
		,{"opacity", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGOpacity::createFromXml(root, "opacity", elem); } }
		,{"stroke", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem ) {return SVGPaint::createFromXml(root, "stroke", elem); } }
		,{"stroke-linejoin", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGStrokeLineJoin::createFromXml(root, "stroke-linejoin", elem); } }
		,{"stroke-linecap", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem ) {return SVGStrokeLineCap::createFromXml(root, "stroke-linecap", elem); } }
		,{"stroke-miterlimit", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem ) {return SVGStrokeMiterLimit::createFromXml(root, "stroke-miterlimit", elem); } }
		,{"stroke-width", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem ) {return SVGStrokeWidth::createFromXml(root, "stroke-width", elem); } }
		//,{"text-align", [](IMapSVGNodes *root, const std::string& name, const XmlElement& elem) {return SVGTextAlign::createFromXml(root, "text-align", elem); } }
		,{"text-anchor", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGTextAnchor::createFromXml(root, "text-anchor", elem); } }
		,{"transform", [](IMapSVGNodes* root, const std::string& name, const XmlElement& elem) {return SVGTransform::createFromXml(root, "transform", elem); } }
		//,{"gradientTransform", [](IMapSVGNodes *root, const std::string& name, const XmlElement& elem) {return SVGTransform::createFromXml(root, "gradientTransform", elem); } }

	};


	//
	// SVGVisualObject
	// This is any object that will change the state of the rendering context
	// that's everything from paint that needs to be applied, to geometries
	// that need to be drawn, to line widths, text alignment, and the like.
	// Most things, other than basic attribute type, will be a sub-class of this
	struct SVGVisualNode : public SVGObject
	{

		std::string fId{};      // The id of the element
		std::map<std::string, std::shared_ptr<SVGVisualProperty>> fVisualProperties{};

		SVGVisualNode() = default;
		SVGVisualNode(IMapSVGNodes* root)
			: SVGObject(root)
		{
			setRoot(root);
		}
		SVGVisualNode(const SVGVisualNode& other) :SVGObject(other)
		{
			fId = other.fId;
			fVisualProperties = other.fVisualProperties;
		}


		SVGVisualNode & operator=(const SVGVisualNode& rhs)
		{
			fId = rhs.fId;
			fVisualProperties = rhs.fVisualProperties;
			
			return *this;
		}
		
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
				auto prop = propconv.second(root(), attrName, elem);
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
			SVGObject::loadSelfFromXml(elem);
			
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
		SVGShape(const SVGShape& other) :SVGVisualNode(other) {}
		
		SVGShape& operator=(const SVGShape& rhs)
		{
			SVGVisualNode::operator=(rhs);
			return *this;
		}
		
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
		std::shared_ptr<SVGObject> fWrappedNode{};
		std::string fWrappedID{};

		double fX = 0;
		double fY = 0;

		SVGTemplateNode() {}
		SVGTemplateNode(IMapSVGNodes* root) : SVGShape(root) {}
		~SVGTemplateNode() {}

		const BLVar& getVariant() override
		{
			return fWrappedNode->getVariant();
		}
		
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




		static std::shared_ptr<SVGTemplateNode> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGTemplateNode>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}


	};
	
	struct SVGPathBasedShape : public SVGShape
	{
		BLPath fPath{};
		
		SVGPathBasedShape() :SVGShape() {}
		SVGPathBasedShape(IMapSVGNodes* iMap) :SVGShape(iMap) {}
		
		
		void drawSelf(IGraphics &ctx) override
		{
			ctx.path(fPath);
		}
	};
	
	struct SVGLine : public SVGPathBasedShape
	{
		BLLine fGeometry{};
		
		SVGLine() :SVGPathBasedShape(){ reset(0, 0, 0, 0); }
		SVGLine(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		//SVGLine(float x, float y, float w, float h)
		//	:SVGPathBasedShape()
		//{
		//	reset(x, y, w, h);
		//}
		
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

		static std::shared_ptr<SVGLine> createFromXml(IMapSVGNodes *iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGLine>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}

		
	};
	
	struct SVGRect : public SVGPathBasedShape
	{
		BLRoundRect fGeometry{};
		
		SVGRect() : SVGPathBasedShape() {}
		SVGRect(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
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
		
		static std::shared_ptr<SVGRect> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGRect>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGCircle : public SVGPathBasedShape
	{
		BLCircle fCircle{};
		
		SVGCircle() : SVGPathBasedShape() {}
		SVGCircle(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fCircle.cx = toDimension(elem.getAttribute("cx")).calculatePixels();
			fCircle.cy = toDimension(elem.getAttribute("cy")).calculatePixels();
			fCircle.r = toDimension(elem.getAttribute("r")).calculatePixels();

			fPath.addCircle(fCircle);
		}

		static std::shared_ptr<SVGCircle> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGCircle>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGEllipse : public SVGPathBasedShape
	{
		BLEllipse fGeometry;
		
		SVGEllipse() : SVGPathBasedShape() {}
		SVGEllipse(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			fGeometry.cx = toDimension(elem.getAttribute("cx")).calculatePixels();
			fGeometry.cy = toDimension(elem.getAttribute("cy")).calculatePixels();
			fGeometry.rx = toDimension(elem.getAttribute("rx")).calculatePixels();
			fGeometry.ry = toDimension(elem.getAttribute("ry")).calculatePixels();

			fPath.addEllipse(fGeometry);
		}

		static std::shared_ptr<SVGEllipse> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGEllipse>(iMap);
			shape->loadFromXmlElement(elem);
			
			return shape;
		}
	};
	
	struct SVGPolyline : public SVGPathBasedShape
	{
		SVGPolyline() : SVGPathBasedShape() {}
		SVGPolyline(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		
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

		static std::shared_ptr<SVGPolyline> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGPolyline>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGPolygon : public SVGPathBasedShape
	{
		SVGPolygon() : SVGPathBasedShape() {}
		SVGPolygon(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		
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

		static std::shared_ptr<SVGPolygon> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto shape = std::make_shared<SVGPolygon>(iMap);
			shape->loadFromXmlElement(elem);

			return shape;
		}
	};
	
	struct SVGPath : public SVGPathBasedShape
	{
		SVGPath() : SVGPathBasedShape() {}
		SVGPath(IMapSVGNodes* iMap) :SVGPathBasedShape(iMap) {}
		
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGPathBasedShape::loadSelfFromXml(elem);
			
			auto d = elem.getAttribute("d");
			auto success = blPathFromCommands(d, fPath);
		}

		static std::shared_ptr<SVGPath> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto path = std::make_shared<SVGPath>(iMap);
			path->loadFromXmlElement(elem);
			
			return path;
		}
	};

	//
	// Turn a base64 encoded inlined image into a BLImage
	// We are handed the attribute, typically coming from a 
	// href of an <image> tage, or as a lookup for a 
	// fill, or stroke pain attribute.
	//
	bool parseImage(const DataChunk& inChunk, BLImage& img)
	{
		bool success{ false };
		DataChunk value = inChunk;

		// figure out what kind of encoding we're dealing with
		// value starts with: 'data:image/png;base64,<base64 encoded image>
		//
		DataChunk data = chunk_token(value, ":");
		auto mime = chunk_token(value, ";");
		auto encoding = chunk_token(value, ",");


		//DataChunk inChunk = value;
		if (encoding == "base64")
		{
			if ((mime == "image/gif"))
			{
				// decode a .gif image
				// gif image decoder

			}
			else if ((mime == "image/png") || (mime == "image/jpeg"))
			{
				// allocate some memory to decode into
				uint8_t* outBuff{ new uint8_t[chunk_size(value)]{} };
				DataChunk outChunk = chunk_from_data_size(outBuff, chunk_size(value));

				auto outData = base64::b64tobin(value, outChunk);

				if (outData)
				{
					BLResult res = img.readFromData(outData.fStart, chunk_size(outData));
					success = (res == BL_SUCCESS);
					printf("parseImage, readFromData: %d  %dX%d\n", res, img.size().w, img.size().h);
				}

				delete[] outBuff;

			}
		}

		return success;
	}

	struct SVGImageNode : public SVGShape
	{
		BLImage fImage{};
		//BLPattern fPattern{};
		double fWidth{};
		double fHeight{};
		double fX = 0;
		double fY = 0;

		
		SVGImageNode(IMapSVGNodes* root) : SVGShape(root) {}
		SVGImageNode(const SVGImageNode& other) :SVGShape(other) { fImage = other.fImage; fWidth = other.fWidth; fHeight = other.fHeight; }
		SVGImageNode& operator=(const SVGImageNode& rhs)
		{
			//SVGShape::operator=(rhs);
			fImage = rhs.fImage;
			fWidth = rhs.fWidth;
			fHeight = rhs.fHeight;

			return *this;
		}

		const BLVar& getVariant() override
		{
			if (fVar.isNull())
			{
				blVarAssignWeak(&fVar, &fImage);
			}

			return fVar;
		}
		
		void drawSelf(IGraphics& ctx)
		{
			if (fImage.empty())
				return;

			ctx.scaleImage(fImage, 0, 0, fImage.size().w, fImage.size().h, fX, fY, fWidth, fHeight);
		}

		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGShape::loadSelfFromXml(elem);

			// Specify the size at which things are displayed
			fWidth = toDimension(elem.getAttribute("width")).calculatePixels(96);
			fHeight = toDimension(elem.getAttribute("height")).calculatePixels(96);

			fX = toDimension(elem.getAttribute("x")).calculatePixels(96);
			fY = toDimension(elem.getAttribute("y")).calculatePixels(96);
			
			// docode and load the image
			//fImage.create(fWidth, fHeight, BL_FORMAT_PRGB32);
			//fCtx.begin(fImage);
			//fCtx.setFillStyle(BLRgba32(255, 255, 0, 255));
			//fCtx.fillAll();
			//fCtx.end();

			//printf("SVGImageNode: %3.0f %3.0f\n", fWidth, fHeight);
			auto href = elem.getAttribute("xlink:href");
			if (!href)
				href = elem.getAttribute("href");

			if (!href)
				return;

			parseImage(href, fImage);
		}

		static std::shared_ptr<SVGImageNode> createFromXml(IMapSVGNodes* iMap, const XmlElement& elem)
		{
			auto node = std::make_shared<SVGImageNode>(iMap);
			node->loadFromXmlElement(elem);

			return node;
		}

	};

	
	
	std::map<std::string, std::function<std::shared_ptr<SVGShape>(IMapSVGNodes* root, const XmlElement& elem)>> gShapeCreationMap = {
	{"line", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGLine::createFromXml(root, elem); }},
	{"rect", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGRect::createFromXml(root, elem); }},
	{"circle", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGCircle::createFromXml(root,elem); }},
	{"ellipse", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGEllipse::createFromXml(root,elem); }},
	{"image", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGImageNode::createFromXml(root,elem); }},
	{"polyline", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGPolyline::createFromXml(root,elem); }},
	{"polygon", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGPolygon::createFromXml(root,elem); }},
	{"path", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGPath::createFromXml(root,elem); }},
	{"use", [](IMapSVGNodes* root, const XmlElement& elem) { return SVGTemplateNode::createFromXml(root,elem); }},

	};


	struct SVGCompoundNode : public SVGShape
	{
		static constexpr int BUILD_STATE_INITIAL = 0;
		static constexpr int BUILD_STATE_OPEN = 1;
		static constexpr int BUILD_STATE_CLOSE = 2;
		
		
		std::vector<std::shared_ptr<SVGObject>> fNodes{};
		std::map<std::string, std::shared_ptr<SVGObject>> fDefinitions;
		int buildState = BUILD_STATE_OPEN;
		
		SVGCompoundNode() : SVGShape() {}
		SVGCompoundNode(IMapSVGNodes* root) :SVGShape(root) {}
		
		const BLVar& getVariant() override
		{
			// if our variant is null
			// traverse down our fNodes, until we find
			// something that reports other than null
			// and return that.
			if (fVar.isNull())
			{
				for (auto& node : fNodes)
				{
					auto& var = node->getVariant();
					if (!var.isNull())
					{
						return node->getVariant();
					}
				}
			}
			
			return fVar;
		}
		
		
		void drawSelf(IGraphics& ctx)
		{
			for (auto& node : fNodes) {
				node->draw(ctx);
			}
		}
		
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
				auto node = it->second(root(), elem);
				addNode(node);
			}
		}
		
		virtual void loadContentNode(const XmlElement& elem)
		{
			// Do something with content nodes	
		}
		
		virtual void loadCDataNode(const XmlElement& elem)
		{
			// Do something with CDATA nodes
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
			loadFromXmlElement(*iter);
			//loadSelfFromXml(*iter);

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
					else if (elem.isCData())
					{
						loadCDataNode(elem);
					}
					else
					{
						// Ignore anything else
						printf("IGNORING: %s\n", elem.name().c_str());
						ndt_debug::printXmlElement(elem);
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
		double x{};
		double y{};
		double dy = 0;

		std::string fText;
		
		SVGTextNode() :SVGCompoundNode() {}
		SVGTextNode(IMapSVGNodes* root) :SVGCompoundNode(root) {}

		void drawSelf(IGraphics& ctx) override
		{
			ctx.textFont("Calibri");	// BUGBUG - hardcoded, should go away when property supported
			ctx.text(fText.c_str(), x, y+dy);

			SVGCompoundNode::drawSelf(ctx);
		}
		
		virtual void loadSelfFromXml(const XmlElement& elem)
		{
			SVGCompoundNode::loadSelfFromXml(elem);

			x = toDimension(elem.getAttribute("x")).calculatePixels(96);
			y = toDimension(elem.getAttribute("y")).calculatePixels(96);
			dy = toDimension(elem.getAttribute("dy")).calculatePixels(96);
			//fFontSize = toDimension(elem.getAttribute("font-size")).calculatePixels(96);
		}

		void loadContentNode(const XmlElement& elem) override
		{
			// Do something with content nodes	
			fText = toString(elem.data());
		}

		void loadCompoundNode(XmlElementIterator& iter) override
		{
			// Most likely a <tspan>
			if ((*iter).name() == "tspan")
			{
				auto node = std::make_shared<SVGTextNode>(fRoot);
				node->loadFromIterator(iter);
				addNode(node);
			}
			else
			{
				SVGCompoundNode::loadCompoundNode(iter);
			}

		}
		
	};
	
	//============================================================
	//	SVGStyleNode
	// Content node is a CDATA section that contains a style sheet
	//============================================================
	struct SVGStyleNode :public SVGCompoundNode
	{
		double x;
		double y;
		std::string fText;

		SVGStyleNode() :SVGCompoundNode() {}
		SVGStyleNode(IMapSVGNodes* root) :SVGCompoundNode(root) {}


		virtual void loadSelfFromXml(const XmlElement& elem)
		{
			SVGCompoundNode::loadSelfFromXml(elem);
		}

		void loadCDataNode(const XmlElement& elem) override
		{
			// Do something with CDATA nodes	
			// parse a style sheet
			ndt_debug::printXmlElement(elem);
		}
	};
	
	//============================================================
	// SVGPatternNode
	//============================================================
	struct SVGPatternNode :public SVGCompoundNode
	{
		BLPattern fPattern{};
		BLMatrix2D fTransform{};
		double fWidth{};
		double fHeight{};


		SVGPatternNode(IMapSVGNodes* root) :SVGCompoundNode(root) {}

		const BLVar& getVariant() override
		{
			// This should be called
			if (fVar.isNull())
			{
				fVar = fPattern;
			}
			
			return fVar;
		}
		
		void drawSelf(IGraphics& ctx) override
		{
			// This should not be called
			// a pattern is used as a fill for other visuals
			SVGCompoundNode::drawSelf(ctx);
		}

		virtual void loadSelfFromXml(const XmlElement& elem)
		{
			SVGCompoundNode::loadSelfFromXml(elem);

			fPattern.setExtendMode(BL_EXTEND_MODE_PAD);
			fWidth = toDimension(elem.getAttribute("width")).calculatePixels(1,0,96);
			fHeight = toDimension(elem.getAttribute("height")).calculatePixels(1,0,96);

			if (elem.getAttribute("patternTransform"))
			{
				auto  tform = SVGTransform::createFromXml(root(), "patternTransform", elem);
				fTransform = tform->fTransform;
				fPattern.setMatrix(fTransform);
			}
		}

		// Here we get called while loading child nodes
		// look for <use> here
		void loadSelfClosingNode(const XmlElement& elem) override
		{
			printf("SVGPatternNode::loadSelfClosingNode\n");
			ndt_debug::printXmlElement(elem);

			// if it's a 'use' node, then get the href field
			// and use that to find the referenced node
			if (elem.name() == "use")
			{
				auto href = elem.getAttribute("href");
				if (!href)
					href = elem.getAttribute("xlink:href");
				
				if (href)
				{
					auto refNode = root()->findNodeByHref(href);
					if (refNode)
					{
						// Now we have the referenced node
						// Get the varient from it, and if it's type is image
						// then assign it to our pattern.
						const BLVar & avar = refNode->getVariant();

						//auto atype = blVarGetType(&avar);
						//printf("SVGPatternNode: %d\n", atype);

						
						if (avar.type() == BL_OBJECT_TYPE_IMAGE)
						{
							fPattern.setImage(avar.as<BLImage>());
						}
					}
				}
			}
		}

	};
	
	//============================================================
	//	SVGGradient
	//============================================================
	struct SVGGradient : public SVGCompoundNode
	{
		BLMatrix2D fGradientTransform{};
		BLGradient fGradient{};
		BLVar fGradientVar{};

		SVGGradient(IMapSVGNodes* root) :SVGCompoundNode(root) 
		{
			fGradient.setExtendMode(BL_EXTEND_MODE_PAD);
		}
		SVGGradient(const SVGGradient& other) = delete;
		SVGGradient operator=(const SVGGradient& other) = delete;
		
		const BLVar& getVariant() override
		{
			//auto nStops = fGradient.size();
			//printf("STOPS: %d\n", nStops);
			
			blVarAssignWeak(&fGradientVar, &fGradient);
			return fGradientVar;
		}
		
		// Load a URL Reference
		void loadFromUrl(const DataChunk& inChunk)
		{
			DataChunk str = inChunk;

			auto id = chunk_trim(str, wspChars);
			
			// The first character could be '.' or '#'
			// so we need to skip past that
			if (*id == '.' || *id == '#')
				id++;

			if (!id)
				return;

			// lookup the thing we're referencing
			std::string idStr = toString(id);

			if (fRoot != nullptr)
			{
				auto node = fRoot->findNodeById(idStr);

				// pull out the color value
				if (node != nullptr)
				{
					const BLVar& aVar = node->getVariant();


					if (aVar.isGradient())
						fGradient = aVar.as<BLGradient>();
				}
			}
		}
		
		// This is called to load specific attributes
		void loadSelfFromXml(const XmlElement& elem) override
		{
			//printf("LOAD FROM SELF\n");
			//ndt_debug::printXmlElement(elem);
			
			// look for an href template
			DataChunk href = elem.getAttribute("href");
			if (!href)
				href = elem.getAttribute("xlink:href");

			if (href)
				loadFromUrl(href);
			

			
			SVGCompoundNode::loadSelfFromXml(elem);

		}
		
		
		virtual void loadSelfClosingNode(const XmlElement& elem)
		{
			//printf("SVGGradientNode::loadSelfClosingNode()\n");
			//ndt_debug::printXmlElement(elem);
			
			SVGDimension dim{};
			dim.loadSelfFromChunk(elem.getAttribute("offset"));
			auto offset = dim.calculatePixels(1, 0);


			std::shared_ptr<SVGPaint> c = nullptr;
			
			// If the node specifies a stop color, then use that
			if (elem.getAttribute("stop-color"))
			{
				// the color could either be wrappep up in a 'style' attribute
				// or directly as 'stop-color' and 'stop-opacity'
				c = SVGPaint::createFromXml(root(), "stop-color", elem);
			}
			else if (elem.getAttribute("style"))
			{
				// Otherwise, look for a style attribute
				// If found, parse it, then look for a stop-color in there
				DataChunk style = elem.getAttribute("style");
				if (style)
				{
					XmlElement styleElement;
					// Turn the style into an XmlElement so we can get at the attributes
					parseStyleAttribute(style, styleElement);
					c = SVGPaint::createFromXml(root(), "stop-color", styleElement);
				}
			}


			if (c != nullptr)
			{
				// Convert the variant color to a BLRgba32
				BLVar aVar = c->getVariant();
				uint32_t colorValue = 0;
				auto res = blVarToRgba32(&aVar, &colorValue);
				BLRgba32 acolor(colorValue);

				fGradient.addStop(offset, acolor);
			}
		}
		
	};
	
	struct SVGLinearGradient : public SVGGradient
	{
		SVGLinearGradient(IMapSVGNodes* root) :SVGGradient(root) 
		{
			fGradient.setType(BL_GRADIENT_TYPE_LINEAR);
		}
		
		void loadSelfFromXml(const XmlElement& elem) override
		{
			// load those first, because there might be an 'href' attribute
			// and we want that to happen before we set our own stuff.
			SVGGradient::loadSelfFromXml(elem);
			
			double x1 = toDimension(elem.getAttribute("x1")).calculatePixels();
			double y1 = toDimension(elem.getAttribute("y1")).calculatePixels();
			double x2 = toDimension(elem.getAttribute("x2")).calculatePixels();
			double y2 = toDimension(elem.getAttribute("y2")).calculatePixels();
			

			// gradientUnits needs to be read in here
			BLLinearGradientValues gradientValues = { x1, y1, x2, y2 };
			fGradient.setValues(gradientValues);


			// Get the transform
			if (elem.getAttribute("gradientTransform"))
			{
				auto  tform = SVGTransform::createFromXml(root(), "gradientTransform", elem);
				fGradient.setMatrix(tform->getTransform());
			}
		}


	};
	
	struct SVGRadialGradient : public SVGGradient
	{

		SVGRadialGradient(IMapSVGNodes* root) :SVGGradient(root) 
		{
			fGradient.setType(BL_GRADIENT_TYPE_RADIAL);
		}


		void loadSelfFromXml(const XmlElement& elem) override
		{
			//printf("SVGRadialGradient::loadSelfFromXml\n");
			//ndt_debug::printXmlElement(elem);
			
			SVGGradient::loadSelfFromXml(elem);

			//BLRadialGradientValues gradientValues(0.5,0.5,0.5,0.5,0);
			BLRadialGradientValues gradientValues{};
			
			if (elem.getAttribute("cx") && elem.getAttribute("cy") && elem.getAttribute("r"))
			{
				double cx = toDimension(elem.getAttribute("cx")).calculatePixels(96);
				double cy = toDimension(elem.getAttribute("cy")).calculatePixels(96);
				double r = toDimension(elem.getAttribute("r")).calculatePixels(96);

				gradientValues.x0 = cx;
				gradientValues.y0 = cy;
				gradientValues.x1 = cx;
				gradientValues.y1 = cy;
				gradientValues.r0 = r;
			}


			if (elem.getAttribute("fx"))
				gradientValues.x1 = toDimension(elem.getAttribute("fx")).calculatePixels(96);
				
			if (elem.getAttribute("fy"))
				gradientValues.y1 = toDimension(elem.getAttribute("fy")).calculatePixels(96);
			
			fGradient.setValues(gradientValues);
			
			// gradientUnits needs to be read in here



			if (elem.getAttribute("gradientTransform"))
			{
				auto  tform = SVGTransform::createFromXml(root(), "gradientTransform", elem);
				fGradient.setMatrix(tform->getTransform());
			}

		}

	};




	
	
	struct SVGGroup : public SVGCompoundNode, public IMapSVGNodes
	{
	protected:
		
		bool fInDefinitions{ false };
		std::map<std::string, std::shared_ptr<SVGObject>> fDefinitions;

	public:
		SVGGroup() :SVGCompoundNode(nullptr) {}
		SVGGroup(IMapSVGNodes* root): SVGCompoundNode(root){}


		bool inDefinitions() const override { return fInDefinitions; }
		void setInDefinitions(bool indefs) override { fInDefinitions = indefs; };

		std::shared_ptr<SVGObject> findNodeById(const std::string& name) override
		{
			if (fRoot == this)
				return fDefinitions[name];
			else if (fRoot)
				return fRoot->findNodeById(name);
			
			return nullptr;
		}
		
		// Load a URL Reference
		std::shared_ptr<SVGObject> findNodeByHref(const DataChunk& inChunk) override
		{
			DataChunk str = inChunk;

			auto id = chunk_trim(str, wspChars);

			// The first character could be '.' or '#'
			// so we need to skip past that
			if (*id == '.' || *id == '#')
				id++;

			if (!id)
				return nullptr;

			// lookup the thing we're referencing
			std::string idStr = toString(id);

			return findNodeById(idStr);
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
			if (!inDefinitions() && (node->name() != "symbol"))
			{
				fNodes.push_back(node);
			}

		}

		void addDefinition(const std::string& name, std::shared_ptr<SVGObject> obj)
		{
			if (fRoot == this)
				fDefinitions[name] = obj;
			else if (fRoot != nullptr)
				fRoot->addDefinition(name, obj);
			
		}


		void loadSelfFromXml(const XmlElement& elem) override
		{
			SVGShape::loadSelfFromXml(elem);
		}

		void loadSelfClosingNode(const XmlElement& elem) override
		{
			if (elem.name() == "linearGradient")
			{
				auto node = std::make_shared<SVGLinearGradient>(root());
				node->loadSelfFromXml(elem);
				addNode(node);
			}
			else if (elem.name() == "radialGradient")
			{
				auto node = std::make_shared<SVGRadialGradient>(root());
				node->loadSelfFromXml(elem);
				addNode(node);
			}
			
			auto it = gShapeCreationMap.find(elem.name());
			if (it != gShapeCreationMap.end())
			{
				auto node = it->second(root(), elem);
				addNode(node);
			}
		}
		
		void loadCompoundNode(XmlElementIterator& iter) override
		{
			const ndt::XmlElement& elem = *iter;
			

			
			// Add a child, and call loadIterator
			if (elem.name() == "g")
			{
				auto node = std::make_shared<SVGGroup>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "defs")
			{
				setInDefinitions(true);
				auto node = std::make_shared<SVGGroup>(root());
				node->loadFromIterator(iter);
				addNode(node);
				setInDefinitions(false);
			}
			else if (elem.name() == "linearGradient")
			{
				auto node = std::make_shared<SVGLinearGradient>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "pattern")
			{
				auto node = std::make_shared<SVGPatternNode>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "radialGradient")
			{
				auto node = std::make_shared<SVGRadialGradient>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "text")
			{
				auto node = std::make_shared<SVGTextNode>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "symbol")
			{
				auto  node = std::make_shared<SVGGroup>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else if (elem.name() == "style")
			{
				auto node = std::make_shared<SVGStyleNode>(root());
				node->loadFromIterator(iter);
				addNode(node);
			}
			else {
				//printf("loadCompoundNode: UNKNOWN: %s\n", elem.name().c_str());
				auto node = std::make_shared<SVGGroup>(root());
				node->loadFromIterator(iter);
				addNode(node);
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
		float fWidth{};
		float fHeight{};
		SVGViewbox fViewbox{};
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
			fViewbox = SVGViewbox::createFromXml(root(), elem, "viewBox");

			// preserveAspectRatio

		}

		static std::shared_ptr<SVGRootNode> createFromIterator(XmlElementIterator& iter)
		{
			auto node = std::make_shared<SVGRootNode>();
			node->loadFromIterator(iter);

			return node;
		}

		void draw(IGraphics& ctx) override
		{
			ctx.push();

			//ctx.scale(0.3, 0.3);
			
			// Start with default state
			ctx.strokeJoin(SVG_JOIN_ROUND);
			ctx.ellipseMode(ELLIPSEMODE::CENTER);
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
		{"symbol", [](XmlElementIterator& iter) { return SVGGroup::createFromIterator(iter); }},
	};


	
}
