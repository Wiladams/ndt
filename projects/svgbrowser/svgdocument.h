#pragma once

// References
// https://github.com/lemire/fastbase64

#include "blend2d.h"

#include "maths.hpp"
#include "shaper.h"
#include "geometry.h"

#include "chunkutil.h"
#include "mmap.hpp"
#include "xmlscan.h"
#include "svgshapes.h"


#include <functional>
#include <charconv>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <stack>
#include <memory>


namespace svg
{
    using namespace ndt;
	using namespace ndt_debug;
    using std::string;
    using ndt::DataChunk;


    static charset digitChars("0123456789");
}



namespace svg {
    struct SVGDocument : public IDrawable
    {
        std::shared_ptr<mmap> fFileMap{};
		std::shared_ptr<SVGRootNode> fRootNode{};
        
		// All the drawable nodes within this document
        //std::vector<std::shared_ptr<IDrawable>> fShapes{};
        maths::bbox2f fExtent;
        
        SVGDocument(std::string filename)
		{
            fFileMap = ndt::mmap::create_shared(filename);
            if (fFileMap == nullptr)
                return ;
        }
        
		double width() const { 
			if (fRootNode == nullptr)
				return 100;
			return fRootNode->width();
        }

		double height() const {
			if (fRootNode == nullptr)
				return 100;
			return fRootNode->height();
		}
        
		size_t nodeCount() const {
			if (fRootNode == nullptr)
				return 0;
			return fRootNode->nodeCount();
		}
        
        void draw(IGraphics& ctx) override
        {
            if (nullptr == fRootNode)
                return;
            
            fRootNode->draw(ctx);
        }
        
        void drawProgressive(IGraphics& ctx, double percent, size_t totalNodes, size_t &nodesDrawn)
        {
            if (nullptr == fRootNode)
                return;

            // get the node count of fRootNode
            // then call drawProgressive on fRootNode
            // passing along our progress, and the top level root count
            // so that it can calculate the progress of each node
            size_t drawn = 0;
            fRootNode->drawProgressive(ctx, percent, fRootNode->nodeCount(), drawn);

            //printf("SVGDocument::drawProgressive: %zu of %zu nodes drawn\n", drawn, fRootNode->nodeCount());
        }
        

        // Load the document from an XML Iterator
        // Since this is the top level document, we just want to kick
        // off loading the root node 'svg', and we're done 
        void loadFromIterator(ndt::XmlElementIterator& iter)
        {

            // skip past any elements that come before the 'svg' element
            while (iter)
            {
                const ndt::XmlElement& elem = *iter;

                if (!elem)
                    break;

                printXmlElement(*iter);


                if (elem.isStart() && (elem.name() == "svg"))
                {
                    // There should be only one root node in a document, so we should 
                    // break here, but, curiosity...
                    fRootNode = SVGRootNode::createFromIterator(iter);
                    if (fRootNode != nullptr)
                    {
						printf("SVGDocument node count: %zd\n", fRootNode->nodeCount());
                        //addNode(fRootNode);
                    }
                }

                iter++;
            }
        }
        
        bool load()
        {
			if (fFileMap == nullptr)
				return false;
            
			DataChunk s = fFileMap->getChunk();
			s = chunk_trim(s, wspChars);

			XmlElementIterator iter(s);

			loadFromIterator(iter);

			return true;
        }
        
		static std::shared_ptr<SVGDocument> createFromFilename(const std::string& filename)
		{
			auto doc = std::make_shared<SVGDocument>(filename);
            doc->load();
            
            return doc;
		}

    };
}
