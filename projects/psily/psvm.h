#pragma once



#include "binstream.hpp"
#include "pstypes.h"
#include "p5.hpp"


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

public:
	int fBuildProcDepth = 0;

public:
	PSScanner(PSVM& vm, std::shared_ptr<BinStream> bs);

	PSVM& vm() { return fVM; }

	void incrementProcDepth() { fBuildProcDepth += 1; }
	void decrementProcDepth() { fBuildProcDepth -= 1; }
	bool isBuildingProc() { return fBuildProcDepth > 0; }

	shared_ptr<PSToken> beginArray();
	shared_ptr<PSToken> endArray();
	shared_ptr<PSToken> markOperandStack();
	shared_ptr<PSToken> nextToken();
};


class PSVM
{
	PSStack fOperandStack;
	PSDictionaryStack fDictionaryStack;
	std::mt19937 fRandomGen;
	Surface fSurface;

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

	// Graphics things
	Surface& surface() { return fSurface; }

	// Executing things
	void execArray(shared_ptr<PSToken> tok);
	void execName(shared_ptr<PSToken> tok);

	// Evaluating commands
	void evalStream(shared_ptr<BinStream> bs);
	void runFilename(std::string filename);

};