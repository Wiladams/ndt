#pragma once

// References
// https://jenkov.com/tutorials/svg/svg-and-css.html
//

#include "chunkutil.h"

#include <string>
#include <vector>
#include <map>
#include <tuple>

namespace ndt
{
	
	// CSS Syntax
	// selector {property:value; property:value; ...}
	// 

	struct CSSSelector
	{
		std::string fName{};
		std::map<std::string, ndt::DataChunk> fProperties{};

		CSSSelector() = default;
		
		CSSSelector(std::string &name, const DataChunk& aChunk)
		{
			fName = name;
			loadFromChunk(aChunk);
		}
		
		const std::string& name() const { return fName; }
		const std::map<std::string, ndt::DataChunk>& properties() const { return fProperties; }
		
		
		explicit operator bool() const { return !fName.empty() && fProperties.size() > 0; }
		
		void loadFromChunk(const DataChunk& inChunk)
		{
			DataChunk s = inChunk;
			
			// get the proper-value combinations, which are separated by ';'
			// then split each property-value pair into two chunks
			while (s)
			{
				DataChunk pcombo = chunk_token(s, ndt::charset(";"));
				DataChunk pname = chunk_token(pcombo, ndt::charset(":"));
				
				// Add the property to the map
				if (pcombo && pname)
				{
					//writeChunk(pname);
					//printf(" ==> ");
					//printChunk(pcombo);
					
					fProperties[std::string(pname.fStart, pname.fEnd)] = pcombo;
				}
			}
		}

		DataChunk getPropertyValue(std::string name)
		{
			auto it = fProperties.find(name);
			if (it != fProperties.end())
				return it->second;
			else
				return DataChunk();
		}
	};

	
	//
	// CSSInlineStyleIterator
	// This iterator is used to iterate over the inline style attributes of an element
	// Each iteration returns a CSS property/value pair as a std::pair
	struct CSSInlineStyleIterator {
		DataChunk fChunk;
		DataChunk fCurrentName{};
		DataChunk fCurrentValue{};

		CSSInlineStyleIterator(const DataChunk& inChunk) : fChunk(inChunk) {}


		bool next()
		{
			fChunk = chunk_skip_wsp(fChunk);
			fCurrentName = {};
			fCurrentValue = {};

			if (fChunk)
			{

				DataChunk nextValue = chunk_token(fChunk, ndt::charset(";"));
				fCurrentName = chunk_trim(chunk_token(nextValue, ndt::charset(":")), wspChars);
				fCurrentValue = chunk_trim(nextValue, wspChars);

				return (bool)fCurrentName && (bool)fCurrentValue;
			}
			else
				return false;
		}
		
		
		CSSInlineStyleIterator& operator++() { next();return *this; }
		
		auto operator*()  { return std::make_pair(fCurrentName, fCurrentValue); }

		explicit operator bool() const { return fCurrentName && fCurrentValue; }
	};
	
	

		
	struct CSSSelectorIterator
	{
		DataChunk fSource{};
		DataChunk fMark{};
		CSSSelector fCurrentItem{};
		
		CSSSelectorIterator(const DataChunk& inChunk)
		{
			fSource = inChunk;
			fMark = inChunk;

			next();
		}

		explicit operator bool() const { return (bool)fCurrentItem; }
		
		bool next()
		{
			fSource = chunk_ltrim(fSource, wspChars);
			
			if (!fSource)
			{
				fCurrentItem = CSSSelector();
				return false;
			}
			
			// Look for the next selector, which should be a string
			// followed by a '{', with optional whitespace in between
			// terminated with a '}'
			DataChunk selectorChunk = chunk_token(fSource, ndt::charset("{"));
			selectorChunk = chunk_trim(selectorChunk, wspChars);
			
			if (selectorChunk)
			{
				// fSource is positioned right after the opening '{', so we can
				// look for the closing '}', and then trim the whitespace
				std::string selectorName(selectorChunk.fStart, selectorChunk.fEnd);
				
				DataChunk content = chunk_token(fSource, ndt::charset("}"));
				if (content)
				{
					fCurrentItem = CSSSelector(selectorName, content);
					
					// We found the end of the block, so we have a valid selector
					return true;
				}
			}
			
			return false;

		}

		CSSSelector& operator*()
		{
			return fCurrentItem;
		}
		
		CSSSelectorIterator& operator++()
		{
			next();
			return *this;
		}
		
		CSSSelectorIterator & operator++(int)
		{
			next();
			return *this;
		}
		
		
	};
}