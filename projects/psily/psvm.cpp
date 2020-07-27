#include "psvm.h"
#include "textscan.h"
#include "ps_base_operators.h"

#include <cstring>

#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>
#include <iostream>

using std::shared_ptr;
using std::make_shared;


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


static inline bool isGraph(int c)
{
	return (c > 0x20) && (c < 0x7f);
}

// Skip leading whitespaces in a binstream
bool PSScanner::skipspaces()
{
	while (!fStream->isEOF()) {
		auto c = fStream->peekOctet();
		if (!isWhitespace(c))
			break;
		if (isWhitespace(fStream->peekOctet()))
		{
			fStream->skip(1);
		}
		else {
			return true;
		}
	}

	return !fStream->isEOF();
}

// Definition of a function that can parse a particular lexeme
typedef std::shared_ptr<PSToken> (*LexemeFunc)(PSScanner&, std::shared_ptr<BinStream> strm);

std::unordered_map<int, LexemeFunc> lexemeMap;

// [
std::shared_ptr<PSToken> beginArray(PSScanner& scnr, std::shared_ptr<BinStream> strm)
{
	scnr.vm().beginArray();
	return nullptr;
}

// ]
std::shared_ptr<PSToken> endArray(PSScanner& scnr, std::shared_ptr<BinStream> strm)
{
	scnr.vm().endArray();
	auto arr = scnr.vm().popOperand();

	return arr;
}

// (
std::shared_ptr<PSToken> beginLiteralString(PSScanner&, std::shared_ptr<BinStream> bs)
{
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
	//std::string value((char*)startPtr, len);

	// Skip over closing delimeter
	bs->skip(1);

	auto tok = std::make_shared<PSToken>((char*)startPtr, len, PSTokenType::LITERAL_STRING);

	return tok;
}

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

// {
std::shared_ptr<PSToken> beginProcedure(PSScanner&, std::shared_ptr<BinStream> strm)
{
	return nullptr;
}

// }
std::shared_ptr<PSToken> endProcedure(PSScanner&, std::shared_ptr<BinStream> strm)
{
	return nullptr;
}

std::shared_ptr<PSToken> beginComment(PSScanner&, std::shared_ptr<BinStream> bs)
{
	auto starting = bs->tell();
	auto startPtr = bs->getPositionPointer();

	while ((bs->peekOctet() != '\n') && !bs->isEOF()) {
		bs->skip(1);
	}
	auto ending = bs->tell();
	auto len = ending - starting;
	//std::string value((char*)startPtr, len);
	//printf("beginComment: %s\n", value.c_str());

	auto tok = std::make_shared<PSToken>((char*)startPtr, len, PSTokenType::COMMENT);

	return tok;
}



//scan identifiers
std::shared_ptr<PSToken> PSScanner::lex_name()
{
	//printf("lex_name\n");
	auto starting = fStream->tell();
	auto startPtr = fStream->getPositionPointer();
	//name_buff:reset()

	// read to end of stream, or until delimeter
	while (!fStream->isEOF()) {
		auto c = fStream->peekOctet();
		if (delimeterChars[c] || isWhitespace(c)) {
			break;
		}

		fStream->skip(1);
	}

	auto ending = fStream->tell();
	auto len = ending - starting;
	std::string value((char *)startPtr, len);

	//printf("lex_name: %s\n", name.c_str());

	auto tok = std::make_shared<PSToken>((char*)startPtr, len, PSTokenType::EXECUTABLE_NAME);

	if (value == "true" || (value == "false")) {
		if (value == "true") {
			tok->fData.asBool = true;
		}
		else {
			tok->fData.asBool = false;
		}

		tok->fType = PSTokenType::BOOLEAN;
	}

	return tok;
}

// '/'
std::shared_ptr<PSToken> PSScanner::beginLiteralName()
{
	auto tok = lex_name();
	tok->fType = PSTokenType::LITERAL_NAME;

	return tok;
}

//scan a number
// something got us here, it was something in numBeginChars
//valid: 8.35928e-09
//numBeginChars["+-.[digit]"]
//If not valid number, return nil
std::shared_ptr<PSToken> PSScanner::lex_number()
{
	auto starting = fStream->tell();
	auto startPtr = fStream->getPositionPointer();
	//print("lex_number: ", starting)
	//First, let's look for a valid starting character
	// - , +, ., digit
	if (numBeginChars[fStream->peekOctet()])
	{
		//print("  SKIP 1")
		fStream->skip(1);
	}
	else {
		//print("INVALID START of NUMBER: ", string.char(bs:peekOctet()))
		return nullptr;
	}

	// scan until a delimeter found
	while (true) {
		if (fStream->isEOF())
		{
			break;
		}

		auto c = fStream->peekOctet();

		if (isWhitespace(c)) {
			break;
		}

		if (delimeterChars[c]) {
			break;
		}

		fStream->skip(1);
	}


	auto ending = fStream->tell();
	auto len = ending - starting;
	std::string value((char*)startPtr, len);
	double nValue = std::strtod(value.c_str(), nullptr);

	//print("LEX_NUMBER: ", starting, len, str, value)
	auto tok = make_shared<PSToken>(nValue);
	
	return tok;
}

void PSScanner::StaticConstructor()
{
	lexemeMap['['] = beginArray;
	lexemeMap[']'] = endArray;
	lexemeMap['('] = beginLiteralString;
	lexemeMap['<'] = beginHexString;
	lexemeMap['{'] = beginProcedure;
	lexemeMap['}'] = endProcedure;
	lexemeMap['%'] = beginComment;
}



PSScanner::PSScanner(PSVM &vm, std::shared_ptr<BinStream> bs)
	:fVM(vm),
	fStream(bs)
{
	StaticConstructor();
}

// Generate a single token
std::shared_ptr<PSToken> PSScanner::nextToken()
{
	while (!fStream->isEOF()) {
		skipspaces();
		
		if (fStream->isEOF())
		{
			return nullptr;
		}

		auto c = fStream->readOctet();

		if (lexemeMap[c]) {
			auto tok = lexemeMap[c](*this, fStream);
			if (tok != nullptr) {
				//if (this->fVM->isBuildingProc()) {
				//	this->fVM->OperandStack:push(tok);
				//} else {
				return tok;
				//}
			}
			else {
				// do nothing
				//return nullptr;
			}
		}
		else {
			// handle the case of numbers
			// and names that begin with a number
			if (numBeginChars[c])
			{
				fStream->skip(-1);
				auto sentinel = fStream->tell();
				auto tok = lex_number();
				if (tok == nullptr) {
					fStream->seek(sentinel);
					tok = lex_name();
				}

				if (tok != nullptr) {
					//if (fVM->isBuildingProc()) {
					//
					//}
					//else {
					return tok;
					//}
				}
			}
			else if (isGraph(c))
			{
				fStream->skip(-1);
				auto tok = lex_name();
				//if (fVM->isBuildingProc()) {
				//fVM->pushOperand(tok);
				//}
				//else
				//{
				return tok;
				//}
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
void PSVM::beginArray()
{
	fOperandStack.mark();
}

void PSVM::endArray()
{
	auto n = fOperandStack.countToMark();
	std::shared_ptr<PSArray> arr = make_shared<PSArray>();

	for (size_t i = 0; i < n; i++) {
		auto tok = popOperand();
		arr->push_back(tok);
	}

	// pop the marker itself
	popOperand();

	pushOperand(make_shared<PSToken>(arr));
}

void PSVM::beginProc()
{
	fOperandStack.mark();
	fBuildProcDepth += 1;
}

shared_ptr<PSToken> PSVM::endProc()
{
	endArray();
	auto arr = popOperand();
	arr->setExecutable(true);
	fBuildProcDepth -= 1;

	return arr;
}

void PSVM::execArray(shared_ptr<PSToken> tok)
{
	for (size_t idx = 0; idx < tok->fData.asArray->size(); idx++)
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

void PSVM::execName(shared_ptr<PSToken> tok)
{
	auto op = fDictionaryStack.load(*tok->fData.asString);

	if (op == nullptr) {
		printf("UNKNOWN EXECUTABLE NAME: %s\n", (*tok->fData.asString).c_str());
		return;
	}

	//std::cout << "execName: " << *tok->fData.asString << "  opType: " << std::to_string((int)op->fType) << std::endl;

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
		if (op->isExecutable()) {
			execArray(op);
		}
		else {
			pushOperand(op);
		}
		break;
	default:
		printf("UKNOWN EXECUTABLE TYPE: %d\n", op->fType);
		break;
	}
}

void PSVM::eval(std::shared_ptr<BinStream> bs)
{
	PSScanner scnr(*this, bs);

	//Iterate through tokens
	while (!bs->isEOF()) {
		auto tok = scnr.nextToken();
		if (nullptr == tok)
			break;

		std::cout << "eval: " << tok->toString() << std::endl;
		if (tok->fType == PSTokenType::EXECUTABLE_NAME) {
			execName(tok);
		}
		else if (tok->fType == PSTokenType::COMMENT) {
			// throw comments away, or do document processing
		}
		else {
			pushOperand(tok);
		}
	}
}

void PSVM::eval(std::string s)
{
	//auto bs = make_shared<BinStream>(s.c_str(), s.length());
	//eval(bs);
}

PSVM::PSVM()
{
	auto d = make_shared<PSDictionary>(PSOperators);


	fDictionaryStack.pushDictionary(d);
}