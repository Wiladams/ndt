#include "p5.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

using namespace p5;
namespace fs = std::filesystem;

void setup()
{
	for (auto& p : fs::directory_iterator("\\Fonts")) {
		std::cout << p.path().filename() << "  " << std::endl;
		std::cout << p.path().parent_path() << std::endl;
	}

	halt();
}