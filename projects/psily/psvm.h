#pragma once

#include <memory>
#include <algorithm>

#include "pstoken.h"
#include "psscanner.h"
#include "psstack.h"
#include "psoperators.h"
#include "psarray.h"

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


	// Handling Array
	void beginArray()
	{
		fOperandStack.mark();
	}

	void endArray()
	{
		auto n = fOperandStack.countToMark();
		std::shared_ptr<PSArray> arr = make_shared<PSArray>();

		for (size_t i = 0; i < n; i++) {
			auto tok = popOperand();
			arr->push_back(tok);
		}

		// pop the marker itself
		popOperand();

		pushOperand(make_shared<PSToken>(PSTokenType::LITERAL_ARRAY, arr));
	}

	// Handling Procedure construction
	void beginProc()
	{
		fOperandStack.mark();
		fBuildProcDepth += 1;
	}

	shared_ptr<PSToken> endProc()
	{
		endArray();
		auto arr = popOperand();
		arr->fFlags = arr->fFlags | PSTokenFlags::EXECUTABLE;
		fBuildProcDepth -= 1;

		return arr;
	}

	bool isBuildingProc() { return fBuildProcDepth > 0; }


	/*
		Given an executable array, go through and start executing
			the elements of that array.

			The elements within the array are the same as they were
			when the procedure was constructed.All elements are
			token objects.
	*/
	void execArray(shared_ptr<PSToken> tok)
	{
		for (size_t idx = 0; idx < tok->fData.asArray->size(); idx++ )
		{
			auto item = tok->fData.asArray->at(idx);

			switch (item->fType) {
				case PSTokenType::BOOLEAN:
				case PSTokenType::NUMBER:
				case PSTokenType::LITERAL_ARRAY:
				case PSTokenType::LITERAL_STRING:
				case PSTokenType::LITERAL_NAME:
				case PSTokenType::HEXSTRING:
				case PSTokenType::PROCEDURE:
					pushOperand(item);
					break;

				case PSTokenType::OPERATOR:
					item->fData.asOperator(*this);
					break;


				case PSTokenType::EXECUTABLE_NAME:
					execName(item);
					break;
			}
		}
	}




	void execName(shared_ptr<PSToken> tok)
	{
		auto op = fDictionaryStack.load(tok->asString);

		if (op == nullptr) {
			printf("UNKNOWN EXECUTABLE NAME: %s\n", tok->asString.cs_str());
			return;
		}

		switch (op->fType)
		{
		case PSTokenType::BOOLEAN:
		case PSTokenType::NUMBER:
		case PSTokenType::LITERAL_STRING:
			pushOperand(op);
			break;

		case PSTokenType::OPERATOR:
			op->fData.asOperator(*this);
			break;

		case PSTokenType::PROCEDURE:
			if (op->fFlags & PSFlags::EXECUTABLE) {
				execArray(op->fData.asArray);
			}
			else{
				pushOperand(op);
			}
			break;
		default:
			printf("UKNOWN EXECUTABLE TYPE: %d\n", op->fType);
			break;
		}
	}

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