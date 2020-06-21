#pragma once

#include "binstream.hpp"
#include "pstoken.h"

#include <memory>


class PSVM;

class PSScanner
{
	std::shared_ptr<BinStream> fStream;
	PSVM *fVM;



	static void StaticConstructor();

	std::shared_ptr<PSToken> lex_number();
	std::shared_ptr<PSToken> lex_name();
	std::shared_ptr<PSToken> beginLiteralName();

public:
	PSScanner(PSVM * vm, std::shared_ptr<BinStream> bs);

	std::shared_ptr<PSToken> nextToken();
};