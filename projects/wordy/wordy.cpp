#include "p5.hpp"
#include "textscan.h"
#include "binstream.hpp"

#include <cstdio>

using namespace p5;

void setup()
{
	createCanvas(800, 600);
	dropFiles();
}

void fileDrop(const FileDropEvent& e)
{
	printf("fileDrop: %d,%d - %D\n", e.x, e.y, e.filenames.size());
	for (size_t i = 0; i < e.filenames.size(); i++) {
		printf("  %s\n", e.filenames[i].c_str());
	}
}