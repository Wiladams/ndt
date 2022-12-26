#include "p5.hpp"

//#include "svgscanner.h"
#include "xmlscan.h"
#include "mmap.hpp"


using namespace p5;
using namespace ndt;


std::shared_ptr<ndt::mmap> fmap{};

void setup()
{
	//createCanvas(1024, 768);
	fullscreen();

	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	windowLayout(layout);
}

void draw()
{

	// Clear to background, whatever it is
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();

	noStroke();
	fill(0x7f);
	rect(0, 0, canvasWidth, 48);

	// Put some instructional text up there
	fill(0);
	textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
	textSize(36);
	textFont("Consolas");
	text("DROP FILES HERE", displayWidth / 2.0, 24);

	flush();
}

// halt the entire application once a user
// presses the 'escape' key
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

// A file drop may contain the names
// of several files at once
std::map<int, std::string> elemTypeNames = {
	{ndt::XML_ELEMENT_TYPE_INVALID, "INVALID"}
	,{ndt::XML_ELEMENT_TYPE_CONTENT, "CONTENT"}
	,{ndt::XML_ELEMENT_TYPE_SELF_CLOSING, "SELF_CLOSING"}
	,{ndt::XML_ELEMENT_TYPE_START_TAG, "START_TAG"}
	,{ndt::XML_ELEMENT_TYPE_END_TAG, "END_TAG"}
	,{ndt::XML_ELEMENT_TYPE_COMMENT, "COMMENT"}
	,{ndt::XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION, "PROCESSING_INSTRUCTION"}
};

void printXmlElement(const ndt::XmlElement &elem)
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


void testXmlScanner(const ndt::DataChunk& aChunk)
{
	XmlElementIterator iter(aChunk);
	
	while (iter)
	{
		printXmlElement(*iter);
		
		iter++;
	} 
}

void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	for (int i = 0; i < e.filenames.size(); i++)
	{
		fmap = ndt::mmap::create_shared(e.filenames[i]);
		if (fmap != nullptr)
		{
			auto win = window(0, 0, 256, 256);
			win->setBackgroundColor(Pixel(255, 255, 255, 255));
			win->setTitle(e.filenames[i]);
			
			testXmlScanner(fmap->getChunk());
			//auto doc = svg::SVGDocument::scan(fmap->getChunk());
			//win->addGraphic(svg::SVGScanner::fromFile(fmap));
		}
		else
			halt();


		break;
	}
}
