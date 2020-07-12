#include "p5.hpp"
#include "pstoken.h"
#include "psscanner.h"
#include "filestream.h"
#include "psvm.h"

#include <iostream>

using namespace p5;

/*
void* operator new(std::size_t n)
{
	std::cout << "[allocating " << n << " bytes]\n";
	return malloc(n);
}
*/

void printStack(PSStack& stk)
{
	std::cout << "==== printStrack (" << stk.length() << ") ====\n";

	size_t len = stk.length();
	for (size_t i = 0; i < len; i++)
		printf("- %s\n", stk.nth(i)->toString().c_str());
	//printf("- %d\n", stk.nth(i)->fType);
}

/*
void preload()
{
	const char* value = "!PS";
	int len = strlen(value);
	PSToken tok(value, len, PSTokenType::COMMENT);
	std::cout << "Token: " <<  tok.toString() << "\n";

	//std::string value("!PS");
	//auto tok = std::make_shared<PSToken>(value, PSTokenType::COMMENT);
	//printf("Token: %s\n", tok->toString().c_str());
}
*/

void setup()
{

	std::shared_ptr<FileStream> fs = std::make_shared<FileStream>("hello.ps");
	
	//printf("fs.isValid(): %d\n", fs->isValid());

	PSVM vm;
	 
	vm.eval(fs);
}