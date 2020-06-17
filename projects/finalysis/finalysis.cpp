#include "p5.hpp"
#include "filestream.h"
#include <memory>

// this sample is really just a test of the FileStream
// class.  It will open up a filestream, then create a histogram
// of the bytes, and display those values in a bar chart.

using namespace p5;

std::shared_ptr<FileStream> fStream;
size_t histogram[256];
size_t biggest;

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
	for (size_t i = 0; i < 256; i++) {
		int x = map(i, 0, 256, 0, 256);
		int h = map(histogram[i], 0, biggest, 0, 256);
		rect(x, 256-h, 2, h);
	}
	pop();
}

void setup()
{
	createCanvas(1024, 768);
	fStream = std::make_shared<FileStream>("earthmap2k.jpg");

	printf("fStream, data: %p size: %ld\n", fStream->data(), fStream->size());

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
		biggest = MAX(biggest, histogram[i]);
	}

	printf("BIGGEST: %d\n", biggest);
}