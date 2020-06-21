#pragma once

#include <memory>

#include "pstoken.h"
#include "psscanner.h"
#include "psstack.h"
#include "psoperators.h"

using std::shared_ptr;
using std::make_shared;

class PSVM
{
	PSStack fOperandStack;
	int fBuildProcDepth = 0;

public:
	// Operand stack
	PSStack& operandStack() { return fOperandStack; }
	shared_ptr<PSToken> popOperand() { return fOperandStack.pop(); }
	void pushOperand(shared_ptr<PSToken> tok) { fOperandStack.push(tok); }


	void beginProc()
	{
		fOperandStack.mark();
		fBuildProcDepth += 1;
	}

	shared_ptr<PSToken> endProc()
	{
		//endArray();
		auto arr = popOperand();
		fBuildProcDepth -= 1;

		return arr;
	}

	bool isBuildingProc() { return fBuildProcDepth > 0; }


	void eval(shared_ptr<BinStream> bs)
	{
		auto scnr = make_shared<PSScanner>(this, bs);

		//Iterate through tokens
		while (!bs->isEOF()) {
			auto tok = scnr->nextToken();
			if (tok == nullptr)
				break;

			if (tok->fType == PSTokenType::EXECUTABLE_NAME) {
				execName(tok);
			}
			else {
				pushOperand(tok);
			}
		}
	}

	void eval(std::string s)
	{
		auto bs = make_shared<BinStream>(s.c_str(), s.length());
	}
};