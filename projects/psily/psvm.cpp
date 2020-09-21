#include <cstring>
#include <chrono>
#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>
#include <iostream>
#include <variant>

#include "psvm.h"
#include "textscan.h"
#include "ps_base_operators.h"



using std::shared_ptr;
using std::make_shared;

using namespace ndt;

// These are a convenient way of putting characters together
// in an easily searchable set.
// This does not work for unicode sequences, but rather the
// stuff you'll typically see while tokenizing a programming language
charset escapeChars("/\\\"bfnrtu");
charset delimeterChars("()<>[]{}/%");
charset isWhitespace("\t\n\f\r ");
//charset hexChars("0123456789abcdefABCDEF");
//charset digitChars("0123456789");
charset numBeginChars("+-.0123456789");




// Skip leading whitespaces in a binstream
bool skipspaces(std::shared_ptr<BinStream> bs)
{
	while (!bs->isEOF()) {
		auto c = bs->peekOctet();
		if (!isWhitespace(c))
			break;
		if (isWhitespace(bs->peekOctet()))
		{
			bs->skip(1);
		}
		else {
			return true;
		}
	}

	return !bs->isEOF();
}


//scan identifiers
std::shared_ptr<PSToken> lex_name(PSScanner& scnr, shared_ptr<BinStream> bs)
{
	auto starting = bs->tell();
	auto startPtr = bs->getPositionPointer();


	// read to end of stream, or until delimeter
	while (!bs->isEOF()) {
		auto c = bs->peekOctet();
		if (delimeterChars[c] || isWhitespace(c)) {
			break;
		}

		bs->skip(1);
	}

	auto ending = bs->tell();
	auto len = ending - starting;
	std::string value((char*)startPtr, len);

	auto tok = std::make_shared<PSToken>(value, PSTokenType::EXECUTABLE_NAME);

	if (value == "true" || (value == "false")) {
		tok->setType(PSTokenType::BOOLEAN);
		if (value == "true") {
			*tok = true;
		} else {
			*tok = false;
		}
	}

	return tok;
}


// Dispatch table for individual lexeme processing
// really this could just be a switch statement within the scanner
// but this way is more informative coding wise
std::unordered_map < int, std::function<shared_ptr<PSToken> (PSScanner& scnr, shared_ptr<BinStream> bs)> > lexemeMap
{
	// [ - beginArray
	{'[',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		return scnr.beginArray();
	}},
	
	// ] - endArray
	{']',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		return scnr.endArray();
	}},

	// ( - literal string
	{'(',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		auto starting = bs->tell();
		auto startPtr = bs->getPositionPointer();

		while (!bs->isEOF()) {
			auto c = bs->peekOctet();
			if (c == ')') {
				break;
			}

			// deal with embedded chars
			if (c == '\\') {
				// BUGBUG
			}

			bs->skip(1);
		}

		auto ending = bs->tell();
		auto len = ending - starting;
		std::string value((char*)startPtr, len);

		// Skip over closing delimeter
		bs->skip(1);

		auto tok = std::make_shared<PSToken>(value, PSTokenType::LITERAL_STRING);

		return tok;
	}},
		/*
// <
std::shared_ptr<PSToken> beginHexString(PSScanner&, std::shared_ptr<BinStream> bs)
{
	auto starting = bs->tell();
	auto startPtr = bs->getPositionPointer();

	while (!bs->isEOF()) {
		auto c = bs->peekOctet();
		if (c == '>') {
			break;
		}

		// deal with embedded chars
		if (c == '\\') {
			// BUGBUG
		}

		bs->skip(1);
	}

	auto ending = bs->tell();
	auto len = ending - starting;

	// Skip over closing delimeter
	bs->skip(1);

	// make a string
	// then do substitutions to convert hex numbers
	// into actual bytes
	auto tok = std::make_shared<PSToken>((char*)startPtr, len, PSTokenType::HEXSTRING);

	return tok;
}
*/
	// { - beginProcedure
	{ '{',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		scnr.fBuildProcDepth += 1;
		
		return scnr.markOperandStack();
	}},

	// } - endProcedure
	{ '}',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		auto tok = scnr.endArray();
		tok->setType(PSTokenType::PROCEDURE);
		tok->setExecutable(true);
		scnr.fBuildProcDepth -= 1;

		return tok;
	}},

	// % - begin comment
	{ '%',[](PSScanner& scnr, shared_ptr<BinStream> bs) {
		auto starting = bs->tell();
		auto startPtr = bs->getPositionPointer();

		while ((bs->peekOctet() != '\n') && !bs->isEOF()) {
			bs->skip(1);
		}

		auto ending = bs->tell();
		auto len = ending - starting;
		std::string value((char*)startPtr, len);

		auto tok = std::make_shared<PSToken>(value, PSTokenType::COMMENT);

		return tok;
	} },
	
	// '/' - literal name
	{ '/', [](PSScanner& scnr, shared_ptr<BinStream> bs) {
		auto tok = lex_name(scnr, bs);
		tok->fType = PSTokenType::LITERAL_NAME;

		return tok;
	} },

};



shared_ptr<PSToken> PSScanner::markOperandStack()
{
	vm().operandStack().mark();
	return nullptr;
}

std::shared_ptr<PSToken> PSScanner::beginArray()
{
	return markOperandStack();
}

std::shared_ptr<PSToken> PSScanner::endArray()
{
	auto n = vm().operandStack().countToMark();
	auto arr = make_shared<PSArray>();

	for (size_t i = 0; i < n; i++) {
		auto tok = vm().popOperand();
		//arr->push_back(tok);
		arr->push_front(tok);
	}

	// pop the marker itself
	auto op = vm().popOperand();

	auto scannedTok = make_shared<PSToken>(arr);

	return scannedTok;
}





// scan a number
// something got us here, it was something in numBeginChars
// valid: 8.35928e-09
// numBeginChars["+-.[digit]"]
// If not valid number, return nil
std::shared_ptr<PSToken> lex_number(PSScanner &scnr, shared_ptr<BinStream> bs)
{
	auto starting = bs->tell();
	auto startPtr = bs->getPositionPointer();
	//print("lex_number: ", starting)
	//First, let's look for a valid starting character
	// - , +, ., digit
	if (numBeginChars[bs->peekOctet()])
	{
		//print("  SKIP 1")
		bs->skip(1);
	}
	else {
		//print("INVALID START of NUMBER: ", string.char(bs:peekOctet()))
		return nullptr;
	}

	// scan until a delimeter found
	while (true) {
		if (bs->isEOF())
		{
			break;
		}

		auto c = bs->peekOctet();

		if (isWhitespace(c)) {
			break;
		}

		if (delimeterChars[c]) {
			break;
		}

		bs->skip(1);
	}


	auto ending = bs->tell();
	auto len = ending - starting;
	std::string value((char*)startPtr, len);
	double nValue = std::strtod(value.c_str(), nullptr);

	auto tok = make_shared<PSToken>(nValue);
	
	return tok;
}



PSScanner::PSScanner(PSVM &vm, std::shared_ptr<BinStream> bs)
	:fVM(vm),
	fStream(bs),
	fBuildProcDepth(0)
{
}



// Generate a single token
std::shared_ptr<PSToken> PSScanner::nextToken()
{
	while (!fStream->isEOF()) {
		skipspaces(fStream);
		
		if (fStream->isEOF())
		{
			return nullptr;
		}

		auto c = fStream->readOctet();

		if (lexemeMap[c]) {
			auto tok = lexemeMap[c](*this, fStream);
			if (tok != nullptr) {
				if (isBuildingProc()) {
					vm().pushOperand(tok);
				} else {
					return tok;
				}
			}
			else {
				// do nothing
				// look for next token
			}
		}
		else {
			// handle the case of numbers
			// and names that begin with a number
			if (numBeginChars[c])
			{
				fStream->skip(-1);
				auto sentinel = fStream->tell();
				auto tok = lex_number(*this, fStream);
				if (tok == nullptr) {
					fStream->seek(sentinel);
					tok = lex_name(*this, fStream);
				}

				if (tok != nullptr) {
					if (isBuildingProc()) {
					
					} else {
						return tok;
					}
				}
			}
			else if (isGraph(c))
			{
				fStream->skip(-1);
				auto tok = lex_name(*this, fStream);
				if (isBuildingProc()) 
				{
					vm().pushOperand(tok);
				} else {
					return tok;
				}
			}
			else {
				printf("SCANNER UNKNOWN: %c\n", c);
			}
		}
	}

	// returning a nullptr indicates there are no further
	// tokens to be scanned
	return nullptr;
}




//
// Virtual Machine Definition
//

PSVM::PSVM()
{
	// seed random number generator with system time
	fRandomGen.seed((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));

	// Setup dictionary stack
	// Base operators, graphics, file all go into 
	// initial userdict
	auto d = make_shared<PSDictionary>();
	d->addOperators(PSBaseOperators);


	fDictionaryStack.pushDictionary(d);
}


void PSVM::execArray(shared_ptr<PSToken> tok)
{
	auto arr = tok->asArray();

	for (size_t idx = 0; idx < arr->size(); idx++)
	{
		auto item = arr->at(idx);

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
			item->asFunction()(*this);
			break;


		case PSTokenType::EXECUTABLE_NAME:
			execName(item);
			break;
		}
	}
}

void PSVM::execName(shared_ptr<PSToken> tok)
{
	// lookup the name in the dictionary stack
	auto key = tok->asString();
	auto op = fDictionaryStack.load(key);
	
	// if the name is not found, report error and continue
	if (op == nullptr) {
		std::cout << "UNKNOWN EXECUTABLE NAME: " << key << std::endl;
		return;
	}

	//std::cout << "execName: ";
	// tok->printValue(std::cout) << std::endl;

	// We found an object associated with the name.  
	// Take action based on the type of the found object
	switch (op->fType)
	{
	case PSTokenType::BOOLEAN:
	case PSTokenType::NUMBER:
	case PSTokenType::LITERAL_STRING:
		pushOperand(op);
		break;

	case PSTokenType::OPERATOR:
		op->asFunction()(*this);
		break;

	case PSTokenType::PROCEDURE:
	case PSTokenType::LITERAL_ARRAY:
		if (op->isExecutable()) {
			execArray(op);
		}
		else {
			pushOperand(op);
		}
		break;
	default:
		std::cout << "UKNOWN EXECUTABLE TYPE: " << (int)op->fType << std::endl;
		break;
	}
}

void PSVM::evalStream(std::shared_ptr<BinStream> bs)
{
	PSScanner scnr(*this, bs);

	// Alternate between grabbing a token
	// and executing something
	while (!bs->isEOF()) {
		auto tok = scnr.nextToken();
		if (nullptr == tok)
			break;

		//std::cout << "eval: " << tok->fData << std::endl;
		//std::cout << "eval: ";
		//tok->printValue(std::cout) << std::endl;

		if (tok->fType == PSTokenType::EXECUTABLE_NAME) {
			execName(tok);
		} else if (tok->fType == PSTokenType::COMMENT) {
			// throw comments away, or do document processing
		} else {
			// by default, just place the token on the operand stack
			pushOperand(tok);
		}
	}
}

void PSVM::runFilename(std::string filename)
{
	auto fs = std::make_shared<FileStream>(filename);
	evalStream(fs);
}

