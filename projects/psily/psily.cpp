#include "p5.hpp"
#include "pstoken.h"
#include "psscanner.h"
#include "filestream.h"
#include "psvm.h"


using namespace p5;


void printStack(PSStack& stk)
{
	printf("=== printStack (%zd) ====\n", stk.length());
	size_t len = stk.length();
	for (size_t i = 0; i < len; i++)
		printf("- %s\n", stk.nth(i)->toString().c_str());
	//printf("- %d\n", stk.nth(i)->fType);
}

void setup()
{

	std::shared_ptr<BinStream> fs = std::make_shared<FileStream>("hello.ps");
	
	PSVM vm;
	 
	vm.eval(fs);
}