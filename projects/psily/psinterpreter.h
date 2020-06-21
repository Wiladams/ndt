#pragma once

#include "binstream.hpp"
#include "pstoken.h"
#include "psscanner.h"

#include <memory>

class PSVM;



class PSInterpreter
{
	std::shared_ptr<PSVM> fVM;
	std::shared_ptr<BinStream> fStream;
	PSScanner fScanner;

public:
	PSInterpreter(std::shared_ptr<PSVM> vm, std::shared_ptr<BinStream> bs)
		:fStream(bs),
		fScanner(vm, bs)
	{}

	void run()
	{
		while (!fStream->isEOF()) {
			auto tok = fScanner.nextToken();
			if (tok == nullptr)
				break;

			switch (tok->fType) {
			case PSTokenType::BOOLEAN:
			case PSTokenType::NUMBER:
			case PSTokenType::LITERAL_STRING:
			case PSTokenType::HEXSTRING:
			case PSTokenType::LITERAL_NAME:
			case PSTokenType::LITERAL_ARRAY:
			case PSTokenType::PROCEDURE:
				fVM->pushOperand(tok);
				break;
			
			case PSTokenType::EXECUTABLE_NAME:
				fVM->execName(tok);
				break;

			default:
				printf("PSInterpreter, UKNOWN Token Type: %d\n", tok->fType);
			}
		}
	}
};