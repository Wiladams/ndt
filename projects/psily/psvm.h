#pragma once



#include "binstream.hpp"
#include "pstoken.h"
#include "psstack.h"
#include "psarray.h"
#include "psdictionary.h"

#include <memory>
#include <algorithm>
#include <random>

using std::shared_ptr;
using std::make_shared;

class PSVM;

class PSScanner
{
	std::shared_ptr<BinStream> fStream;
	PSVM& fVM;


	static void StaticConstructor();

	std::shared_ptr<PSToken> lex_number();
	std::shared_ptr<PSToken> lex_name();
	std::shared_ptr<PSToken> beginLiteralName();

	bool skipspaces();

public:
	PSScanner(PSVM& vm, std::shared_ptr<BinStream> bs);

	std::shared_ptr<PSToken> nextToken();
	
	PSVM & vm() { return fVM; }
};


class PSVM
{
	PSStack fOperandStack;
	PSDictionaryStack fDictionaryStack;
	std::mt19937 fRandomGen;

	int fBuildProcDepth = 0;

public:
	PSVM();

	// Random number generation
	inline unsigned int randomInt() { return fRandomGen(); }
	inline void seedRandomInt(unsigned int seed) { fRandomGen.seed(seed); }

	// Operand stack
	PSStack& operandStack() { return fOperandStack; }
	shared_ptr<PSToken> popOperand() { return fOperandStack.pop(); }
	void pushOperand(shared_ptr<PSToken> tok) { fOperandStack.push(tok); }

	// Dictionary Stack
	PSDictionaryStack& dictionaryStack() { return fDictionaryStack; }

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