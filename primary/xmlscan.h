#pragma once

#include "chunkutil.h"

#include <string>
#include <map>

//
// This file represents a very small, fast, simple XML scanner
// The purpose is to break a chunk of XML down into component parts, that higher
// level code can then use to do whatever it wants.
// 
// You can construct an iterator, and use that to scan through the XML
// using a 'pull model'.
// 
// One key aspect of the design is that it operates on a span of memory.  It does
// no deal with files, or streams, or anything high level like that, just a chunk.
// It does not alter the chunk, just reads bytes from it, and returns chunks in 
// responses.
//
// The fundamental unit is the XmlElement, which encapsulates a single XML element
// and its attributes.
//
// The element contains individual members for
//  kind - content, self-closing, start-tag, end-tag, comment, processing-instruction
//  name - the name of the element, if opening or closing tag
//  attributes - a map of attribute names to attribute values.  Values are still in raw form
//  data - the raw data of the element.  The starting name has been removed, to be turned into the name
// 
// The XmlElementIterator is used to iterate over the elements in a chunk of memory.
//
// References:
// https://dvcs.w3.org/hg/microxml/raw-file/tip/spec/microxml.html#:~:text=MicroXML%20is%20a%20Unicode-based%20textual%20format%20for%20general-purpose,in%20this%20format%20is%20called%20a%20MicroXML%20document.
// https://www.w3.org/TR/REC-xml/
//

namespace ndt {


    //static ndt::charset wspChars(" \r\n\t\f\v");


    enum XML_ELEMENT_TYPE {
        XML_ELEMENT_TYPE_INVALID = 0
		, XML_ELEMENT_TYPE_XMLDECL
        , XML_ELEMENT_TYPE_CONTENT
        , XML_ELEMENT_TYPE_SELF_CLOSING
        , XML_ELEMENT_TYPE_START_TAG
        , XML_ELEMENT_TYPE_END_TAG
        , XML_ELEMENT_TYPE_COMMENT
        , XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION
        , XML_ELEMENT_TYPE_CDATA
        , XML_ELEMENT_TYPE_DOCTYPE
    };

	struct XmlName {
        ndt::DataChunk fNamespace{};
        ndt::DataChunk fName{};

        XmlName() = default;
        
        XmlName(const DataChunk& inChunk)
        {
            reset(inChunk);
        }

        XmlName(const XmlName &other):fNamespace(other.fNamespace), fName(other.fName){}
        
        XmlName& operator =(const XmlName& rhs)
        {
            fNamespace = rhs.fNamespace;
            fName = rhs.fName;
            return *this;
        }
        
        XmlName& reset(const DataChunk& inChunk)
        {
            fName = inChunk;
            fNamespace = chunk_token(fName, ':');
            if (chunk_size(fName)<1)
            {
                fName = fNamespace;
                fNamespace = {};
            }
            return *this;
        }
        
		DataChunk name() const { return fName; }
		DataChunk ns() const { return fNamespace; }
	};
    
    // Representation of an xml element
    // The xml scanner will generate these
    struct XmlElement
    {
    private:
        int fElementKind{ XML_ELEMENT_TYPE_INVALID };
        DataChunk fData{};

        XmlName fXmlName{};
        std::string fName{};
        std::map<std::string, DataChunk> fAttributes{};

    public:
        XmlElement() {}
        XmlElement(int kind, const DataChunk& data, bool autoScanAttr = false)
            :fElementKind(kind)
            , fData(data)
        {
            reset(kind, data, autoScanAttr);
        }

		void reset(int kind, const DataChunk& data, bool autoScanAttr = false)
		{
            clear();

            fElementKind = kind;
            fData = data;

            if ((fElementKind == XML_ELEMENT_TYPE_START_TAG) ||
                (fElementKind == XML_ELEMENT_TYPE_SELF_CLOSING) ||
                (fElementKind == XML_ELEMENT_TYPE_END_TAG))
            {
                scanTagName();

                if (autoScanAttr) {
                    if (fElementKind != XML_ELEMENT_TYPE_END_TAG)
                        scanAttributes();
                }
            }
		}
        
		// Clear this element to a default state
        void clear() {
			fElementKind = XML_ELEMENT_TYPE_INVALID;
			fData = {};
			fName.clear();
			fAttributes.clear();
		}
        
        // determines whether the element is currently empty
        bool empty() const { return fElementKind == XML_ELEMENT_TYPE_INVALID; }

        explicit operator bool() const { return !empty(); }

        // Returning information about the element
        const std::map<std::string, DataChunk>& attributes() const { return fAttributes; }
        
        const std::string& name() const { return fName; }
		void setName(const std::string& name) { fName = name; }
        
        int kind() const { return fElementKind; }
		void setKind(int kind) { fElementKind = kind; }
        
        const DataChunk& data() const { return fData; }

		// Convenience for what kind of tag it is
        bool isStart() const { return (fElementKind == XML_ELEMENT_TYPE_START_TAG); }
		bool isSelfClosing() const { return fElementKind == XML_ELEMENT_TYPE_SELF_CLOSING; }
		bool isEnd() const { return fElementKind == XML_ELEMENT_TYPE_END_TAG; }
		bool isComment() const { return fElementKind == XML_ELEMENT_TYPE_COMMENT; }
		bool isProcessingInstruction() const { return fElementKind == XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION; }
        bool isContent() const { return fElementKind == XML_ELEMENT_TYPE_CONTENT; }
		bool isCData() const { return fElementKind == XML_ELEMENT_TYPE_CDATA; }
		bool isDoctype() const { return fElementKind == XML_ELEMENT_TYPE_DOCTYPE; }

        
        void addAttribute(std::string& name, const DataChunk& valueChunk)
        {
            fAttributes[name] = valueChunk;
        }

        DataChunk getAttribute(const std::string &name) const
		{
			auto it = fAttributes.find(name);
			if (it != fAttributes.end())
				return it->second;
			else
                return DataChunk{};
		}
        
    private:
        //
    // Parse an XML element
    // We should be sitting on the first character of the element tag after the '<'
    // There are several things that need to happen here
    // 1) Scan the element name
    // 2) Scan the attributes, creating key/value pairs
    // 3) Figure out if this is a self closing element

    // 
    // We do NOT scan the content of the element here, that happens
    // outside this routine.  We only deal with what comes up the the closing '>'
    //
        void setTagName(const DataChunk& inChunk)
        {
            fXmlName.reset(inChunk);
			fName = std::string(fXmlName.name().fStart, fXmlName.name().fEnd);
        }
        
        void scanTagName()
        {
            DataChunk s = fData;
            bool start = false;
            bool end = false;

            // If the chunk is empty, just return
            if (!s)
                return;

            // Check if the tag is end tag
            if (*s == '/')
            {
                s++;
                end = true;
            }
            else {
                start = true;
            }

            // Get tag name
            DataChunk tagName = s;
            tagName.fEnd = s.fStart;

            while (s && !wspChars[*s])
                s++;

            tagName.fEnd = s.fStart;
            setTagName(tagName);


            fData = s;
        }

        public:
        //
// scanAttributes
// Scans the fData member looking for attribute key/value pairs
// It will add to the member fAttributes these pairs, without further processing.
// This should be called after scanTagName(), because we want to be positioned
// on the first key/value pair. 
//
        int scanAttributes()
        {

            int nattr = 0;
            bool start = false;
            bool end = false;
            uint8_t quote{};
            DataChunk s = fData;


            // Get the attribute key/value pairs for the element
            while (s && !end)
            {
                uint8_t* beginattrValue = nullptr;
                uint8_t* endattrValue = nullptr;


                // Skip white space before the attrib name
                s = chunk_ltrim(s, wspChars);

                if (!s)
                    break;

                if (*s == '/') {
                    end = true;
                    break;
                }

                // Find end of the attrib name.
                static charset equalChars("=");
                auto attrNameChunk = chunk_token(s, equalChars);
                attrNameChunk = chunk_trim(attrNameChunk, wspChars);

                std::string attrName = std::string(attrNameChunk.fStart, attrNameChunk.fEnd);

                //printf("     ATTR : '%s'", attrName.c_str());


                // Skip stuff past '=' until the beginning of the value.
                while (s && (*s != '\"') && (*s != '\''))
                    s++;

                if (!s)
                    break;

                // capture the quote character
                quote = *s;

                // Store value and find the end of it.
                s++;
                beginattrValue = (uint8_t*)s.fStart;

                // Skip until end of the value.
                while (s && *s != quote)
                    s++;

                if (s)
                {
                    endattrValue = (uint8_t*)s.fStart;
                    s++;
                }

                // Store only well formed attributes
                DataChunk attrValue = { beginattrValue, endattrValue };
                //printf("    VALUE :");
                //printChunk(attrValue);

                addAttribute(attrName, attrValue);

                nattr++;
            }

            return nattr;
        }
    };


    


    // XmlElementIterator
    // scans XML generating a sequence of XmlElements
    // This is a forward only non-writeable iterator
    // 
	// Usage:
    //   XmlElementIterator iter = XmlElementIterator(xmlChunk);
    //   XmlElement elem;
    //   do {
	//      elem = iter.next();
    //      if (elem)
	//          processElement(elem);
	//   } while (elem);
    //
    //
    struct XmlElementIterator {
    private:
        // XML Iterator States
        enum XML_ITERATOR_STATE {
            XML_ITERATOR_STATE_CONTENT = 0
            , XML_ITERATOR_STATE_START_TAG

        };
        
        // What state the iterator is in
        int fState{ XML_ITERATOR_STATE_CONTENT };
        ndt::DataChunk fSource{};
        ndt::DataChunk mark{};

        XmlElement fCurrentElement{};
        
    public:
        XmlElementIterator(const ndt::DataChunk& inChunk)
        {
            fSource = inChunk;
            mark = inChunk;

            fState = XML_ITERATOR_STATE_CONTENT;
            
            next();
        }

		explicit operator bool() { return !fCurrentElement.empty(); }
        
        // These operators make it operate like an iterator
        const XmlElement& operator*() const { return fCurrentElement; }
        const XmlElement* operator->() const { return &fCurrentElement; }

        XmlElementIterator& operator++() { next(); return *this; }
        XmlElementIterator& operator++(int) { next(); return *this; }
        
        // Reset the iterator to a known state with data
        void reset(const ndt::DataChunk& inChunk, int st)
        {
            fSource = inChunk;
            mark = inChunk;

            fState = st;
        }

        DataChunk readTag()
        {
            DataChunk elementChunk = fSource;
            elementChunk.fEnd = fSource.fStart;
            
            while (fSource && *fSource != '>')
                fSource++;

            elementChunk.fEnd = fSource.fStart;
            elementChunk = chunk_rtrim(elementChunk, wspChars);
            
            // Get past the '>' if it was there
            fSource++;
            
            return elementChunk;
        }
        
        //
        // !DOCTYPE
        // Will either contain a '[]' section
        // or it will just have '>' at the end
        //
        DataChunk readDoctype()
        {
            DataChunk elementChunk = fSource;
            elementChunk.fEnd = fSource.fStart;
            //auto endingTag = chunk_find_cstr(fSource, "]>");
            
            if (chunk_find_char(fSource, '['))
            {
                // Read until we see ]>
                while (fSource && (*fSource != ']'))
                    fSource++;

                // We want the closing ']' as part of the element
                if (*fSource == ']')
                    fSource++;
            }
            else {
                // skip ahead to '>'
                while (fSource && (*fSource != '>'))
                    fSource++;
            }

            if (*fSource == '>')
            {
                elementChunk.fEnd = fSource.fStart;
                elementChunk = chunk_rtrim(elementChunk, wspChars);

                fSource++;
            }

			return elementChunk;
        }
        
        // Simple routine to scan XML content
        // the input 's' is a chunk representing the xml to 
        // be scanned.
        // The input chunk will be altered in the process so it
        // can be used in a subsequent call to continue scanning where
        // it left off.
        bool next()
        {
            while (fSource)
            {
                switch (fState)
                {
                case XML_ITERATOR_STATE_CONTENT: {

                    if (*fSource == '<')
                    {
                        // Change state to beginning of start tag
                        // for next turn through iteration
                        fState = XML_ITERATOR_STATE_START_TAG;

                        if (fSource != mark)
                        {
                            // Encapsulate the content in a chunk
                            ndt::DataChunk content = { mark.fStart, fSource.fStart };

                            // collapse whitespace
							// if the content is all whitespace
                            // don't return anything
							content = chunk_trim(content, wspChars);
                            if (content)
                            {
                                // Set the state for next iteration
                                fSource++;
                                mark = fSource;
                                fCurrentElement.reset(XML_ELEMENT_TYPE_CONTENT, content);
                                
                                return true;
                            }
                        }

                        fSource++;
                        mark = fSource;
                    }
                    else {
                        fSource++;
                    }

                }
                break;

                case XML_ITERATOR_STATE_START_TAG: {
                    // Create a chunk that encapsulates the element tag 
                    // up to, but not including, the '>' character
                    DataChunk elementChunk = fSource;
                    elementChunk.fEnd = fSource.fStart;
                    int kind = XML_ELEMENT_TYPE_START_TAG;
                    
                    if (chunk_starts_with_cstr(fSource, "?xml"))
                    {
						kind = XML_ELEMENT_TYPE_XMLDECL;
                        elementChunk = readTag();
                    } 
                    else if (chunk_starts_with_cstr(fSource, "?"))
                    {
                        kind = XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION;
                        elementChunk = readTag();
                    }
                    else if (chunk_starts_with_cstr(fSource, "!DOCTYPE"))
                    {
                        kind = XML_ELEMENT_TYPE_DOCTYPE;
                        elementChunk = readDoctype();
                    }
                    else if (chunk_starts_with_cstr(fSource, "!--"))
                    {
						kind = XML_ELEMENT_TYPE_COMMENT;
                        elementChunk = readTag();
                    }
                    else if (chunk_starts_with_cstr(fSource, "![CDATA["))
                    {
                        kind = XML_ELEMENT_TYPE_CDATA;
                        elementChunk = readTag();
                    }
					else if (chunk_starts_with_cstr(fSource, "/"))
					{
						kind = XML_ELEMENT_TYPE_END_TAG;
						elementChunk = readTag();
					}
					else {
						elementChunk = readTag();
                        if (chunk_ends_with_char(elementChunk, '/'))
                            kind = XML_ELEMENT_TYPE_SELF_CLOSING;
					}
                    
                    fState = XML_ITERATOR_STATE_CONTENT;

                    mark = fSource;

					fCurrentElement.reset(kind, elementChunk, true);

                    return true;
                }
                break;

                default:
                    fSource++;
                    break;

                }
            }

            fCurrentElement.clear();
            return false;
        } // end of next()
    };
}


namespace ndt_debug {
	using namespace ndt;
    
    std::map<int, std::string> elemTypeNames = {
     {ndt::XML_ELEMENT_TYPE_INVALID, "INVALID"}
    ,{ndt::XML_ELEMENT_TYPE_CONTENT, "CONTENT"}
    ,{ndt::XML_ELEMENT_TYPE_SELF_CLOSING, "SELF_CLOSING"}
    ,{ndt::XML_ELEMENT_TYPE_START_TAG, "START_TAG"}
    ,{ndt::XML_ELEMENT_TYPE_END_TAG, "END_TAG"}
    ,{ndt::XML_ELEMENT_TYPE_COMMENT, "COMMENT"}
    ,{ndt::XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION, "PROCESSING_INSTRUCTION"}
    ,{ndt::XML_ELEMENT_TYPE_CDATA, "CDATA"}
	,{ndt::XML_ELEMENT_TYPE_XMLDECL, "XMLDECL"}
	,{ndt::XML_ELEMENT_TYPE_DOCTYPE, "DOCTYPE"}
	};

    void printXmlElement(const ndt::XmlElement& elem)
    {
        if (elem.kind() == XML_ELEMENT_TYPE_INVALID)
            return;

        switch (elem.kind())
        {
        case ndt::XML_ELEMENT_TYPE_CONTENT:
        case ndt::XML_ELEMENT_TYPE_COMMENT:
        case ndt::XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION:
            printf("%s: \n", elemTypeNames[elem.kind()].c_str());
            printChunk(elem.data());
            break;

        case ndt::XML_ELEMENT_TYPE_START_TAG:
            printf("START_TAG: [%s]\n", elem.name().c_str());
            break;

        case ndt::XML_ELEMENT_TYPE_SELF_CLOSING:
            printf("SELF_CLOSING: [%s]\n", elem.name().c_str());
            break;

        case ndt::XML_ELEMENT_TYPE_END_TAG:
            printf("END_TAG: [%s]\n", elem.name().c_str());
            break;

        default:
            printf("NYI: %s\n", elemTypeNames[elem.kind()].c_str());
            printChunk(elem.data());
            break;
        }

        for (auto& attr : elem.attributes())
        {
            printf("    %s: ", attr.first.c_str());
            printChunk(attr.second);
        }
    }
}