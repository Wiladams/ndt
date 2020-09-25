#include "p5.hpp"

#include "filestream.h"
#include "psvm.h"

#include <iostream>


using namespace p5;


void setup()
{

	//auto fs = std::make_shared<FileStream>("testy\\hello.ps");
	//auto fs = std::make_shared<FileStream>("testy\\case1.ps");
	//auto fs = std::make_shared<FileStream>("testy\\case2.ps");
	//auto fs = std::make_shared<FileStream>("testy\\def1.ps");
	//auto fs = std::make_shared<FileStream>("testy\\pstack1.ps");
	//auto fs = std::make_shared<FileStream>("testy\\pbourke1.ps");
	//auto fs = std::make_shared<FileStream>("testy\\typeprocs.ps");
	//auto fs = std::make_shared<FileStream>("testy\\version.ps");
	auto fs = std::make_shared<FileStream>("testy\\test_string.ps");

	//printf("fs.isValid(): %d\n", fs->isValid());

	PSVM vm;
	vm.evalStream(fs);
	
	noLoop();
}