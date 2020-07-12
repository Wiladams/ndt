#include "p5.hpp"
#include "filestream.h"
#include <memory>
#include <filesystem>		// Requires C++-17

// this sample is really just a test of the FileStream
// class.  It will open up a filestream, then create a histogram
// of the bytes, and display those values in a bar chart.

using namespace p5;


int histogram[256];
int biggest;


namespace fs = std::filesystem;

/*
void listFiles()
{
	for (auto& p : fs::directory_iterator("c:\\windows\\fonts")) {
		if (p.path().extension() == ".ttf" || p.path().extension() == ".TTF")
			std::cout << p.path() << "  " << p.path().filename() << "  " <<  p.path().extension() << '\n';
	}
}
*/

void createHistogram(std::string filename)
{
	memset(histogram, 0, sizeof(histogram));
	biggest = 0;

	std::shared_ptr<FileStream> fStream = std::make_shared<FileStream>(filename.c_str());

	//printf("fStream, data: %p size: %ld\n", fStream->data(), fStream->size());

	// populate the histogram
	size_t num = 0;
	if (fStream->isValid()) {
		while (!fStream->isEOF()) {
			num++;
			uint8_t c = fStream->readOctet();
			histogram[c] += 1;
		}
	}
	else {
		printf("fStream - NOT VALID\n");
	}

	// Run through the histogram to see what 
	// the biggest value is
	biggest = 0;
	for (size_t i = 0; i < 256; i++) {
		biggest = maths::Max(biggest, histogram[i]);
	}
}
void draw()
{
	background(245, 246, 247);

	push();
	translate(4, 4);
	// draw a little frame
	stroke(0);
	noFill();
	rect(0, 0, 256, 256);

	// Display histogram
	noStroke();
	fill(0xc0);
	for (int i = 0; i < 256; i++) {
		int x = (int)map(i, 0, 256, 0, 256);
		int h = (int)map(histogram[i], 0, biggest, 0, 256);
		rect(x, (double)256-h, 2, h);
	}
	pop();
}

void setup()
{
	createCanvas(1024, 768);
	dropFiles();
}

void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	createHistogram(e.filenames[0]);
}
