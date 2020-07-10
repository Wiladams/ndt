#pragma once

#include <memory>
#include <algorithm>

#include "binstream.hpp"
#include "pstoken.h"
#include "psstack.h"
#include "psarray.h"
#include "psdictionary.h"

using std::shared_ptr;
using std::make_shared;

class PSVM;

class PSScanner
{
	std::shared_ptr<BinStream> fStream;
	//std::shared_ptr<PSVM> fVM;
	PSVM& fVM;


	static void StaticConstructor();

	std::shared_ptr<PSToken> lex_number();
	std::shared_ptr<PSToken> lex_name();
	std::shared_ptr<PSToken> beginLiteralName();

public:
	PSScanner(PSVM& vm, std::shared_ptr<BinStream> bs);

	std::shared_ptr<PSToken> nextToken();
};


class PSVM
{
	PSStack fOperandStack;
	PSDictionaryStack fDictionaryStack;

	int fBuildProcDepth = 0;

public:
	// Operand stack
	PSStack& operandStack() { return fOperandStack; }
	shared_ptr<PSToken> popOperand() { return fOperandStack.pop(); }
	void pushOperand(shared_ptr<PSToken> tok) { fOperandStack.push(tok); }


	// Handling Array
	void beginArray();
	void endArray();


	// Handling Procedure construction
	void beginProc();
	shared_ptr<PSToken> endProc();


	bool isBuildingProc() { return fBuildProcDepth > 0; }

	void execArray(shared_ptr<PSToken> tok);
	void execName(shared_ptr<PSToken> tok);

	void eval(shared_ptr<BinStream> bs);
	void eval(std::string s);

};