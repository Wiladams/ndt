#include "p5.hpp"
#include "pstoken.h"
#include "psscanner.h"
#include "filestream.h"


#include "psstack.h"

using namespace p5;

void draw()
{

}

void printStream(std::shared_ptr<BinStream> bs)
{
	while (!bs->isEOF()) {
		printf("%c", bs->readOctet());
	}
}

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
	PSScanner scanner(fs);

	PSStack pstack;

	pstack.push(std::make_shared<PSToken>(PSTokenType::NUMBER, (double)1));
	pstack.push(std::make_shared<PSToken>(PSTokenType::NUMBER, (double)2));
	pstack.push(std::make_shared<PSToken>(PSTokenType::NUMBER, (double)3));
	pstack.exch();
	pstack.dup();
	//pstack.mark();

	//printf("pstack.countToMark(): %d\n", pstack.countToMark());
	printStack(pstack);

	
	
}