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
    static charset numberChars("0123456789.-+eE");


    constexpr auto SVG_MAX_ATTR = 128;
    constexpr auto SVG_MAX_DASHES = 8;

    // Turn a chunk into a vector of chunks, splitting on the delimiters
    // BUGBUG - should consider the option of empty chunks, especially at the boundaries
    static INLINE std::vector<DataChunk> chunk_split(const DataChunk& inChunk, const charset& delims, bool wantEmpties = false) noexcept
    {
        std::vector<DataChunk> result;
        DataChunk s = inChunk;
        while (s)
        {
            DataChunk token = chunk_token(s, delims);
            //if (size(token) > 0)
            result.push_back(token);
        }

        return result;
    }

}



namespace svg {
    struct SVGDocument : public IDrawable
    {
        std::shared_ptr<mmap> fFileMap{};
        
		// All the drawable nodes within this document
        std::vector<std::shared_ptr<IDrawable>> fShapes{};
        maths::bbox2f fExtent;
        
        SVGDocument(std::string filename)
		{
            fFileMap = ndt::mmap::create_shared(filename);
            if (fFileMap == nullptr)
                return ;
        }
        
        void draw(IGraphics& ctx) override
        {
            for (auto& shape : fShapes)
            {
				shape->draw(ctx);
            }
        }

        // Add a node that can be drawn
		void addNode(std::shared_ptr<SVGObject> node)
		{
			fShapes.push_back(node);
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

                // Skip over these node types we don't know how to process
                if (elem.isComment() || elem.isContent() || elem.isProcessingInstruction())
                {
                    iter++;
                    continue;
                }


                if (elem.isStart() && (elem.name() == "svg"))
                {
                    auto node = SVGRootNode::createFromIterator(iter);
                    if (node != nullptr)
                    {
                        addNode(node);
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
