#include "psscanner.h"
#include "binstream.hpp"

#include <cstring>
#include <bitset>
#include <unordered_map>



// Represent a set of characters as a bitset
struct charset {
	std::bitset<256> bits;

	charset(const char* chars)
	{
		size_t len = strlen(chars);
		for (size_t i = 0; i < len; i++)
			bits.set(chars[i]);
	}

	bool operator [](size_t idx) 
	{
		return bits[idx];
	}
};

// These are a convenient way of putting characters together
// in an easily searchable set.
// This does not work for unicode sequences, but rather the
// stuff you'll typically see while tokenizing a programming language
charset escapeChars("/\\\"bfnrtu");
charset delimeterChars("()<>[]{}/%");
charset whitespaceChars("\t\n\f\r ");
charset hexChars("0123456789abcdefABCDEF");
charset digitChars("0123456789");
charset numBeginChars("+-.0123456789");


static inline bool isGraph(int c)
{
	return (c > 0x20) && (c < 0x7f);
}

// Skip leading whitespaces in a binstream
bool skipspaces(std::shared_ptr<BinStream> bs)
{
	while (true) {
		if (bs->isEOF()) {
			return false;
		}

		if (whitespaceChars[bs->peekOctet()])
		{
			bs->skip(1);
		}
		else {
			return true;
		}
	}
}

// Definition of a function that can parse a particular lexeme
typedef std::shared_ptr<PSToken> (*LexemeFunc)(PSScanner&, std::shared_ptr<BinStream> strm);

std::unordered_map<int, LexemeFunc> lexemeMap;

// [
std::shared_ptr<PSToken> beginArray(PSScanner&, std::shared_ptr<BinStream> strm)
{
	printf("Begin Array\n");
	return nullptr;
}

// ]
std::shared_ptr<PSToken> endArray(PSScanner&, std::shared_ptr<BinStream> strm)
{
	printf("End Array\n");
	return nullptr;
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
	std::string value((char*)startPtr, len);

	// Skip over closing delimeter
	bs->skip(1);

	auto tok = std::make_shared<PSToken>(PSTokenType::LITERAL_STRING, value);

	return tok;
}

// <
std::shared_ptr<PSToken> beginHexString(PSScanner&, std::shared_ptr<BinStream> strm)
{
	return nullptr;
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
	std::string value((char*)startPtr, len);

	auto tok = std::make_shared<PSToken>(PSTokenType::COMMENT, value);

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
		if (delimeterChars[c] || whitespaceChars[c]) {
			break;
		}

		fStream->skip(1);
	}

	auto ending = fStream->tell();
	auto len = ending - starting;
	std::string value((char *)startPtr, len);

	//printf("lex_name: %s\n", name.c_str());

	auto tok = std::make_shared<PSToken>(PSTokenType::EXECUTABLE_NAME, value);

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

		if (whitespaceChars[c]) {
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
	auto tok = std::make_shared<PSToken>(PSTokenType::NUMBER, nValue);
	
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



PSScanner::PSScanner(std::shared_ptr<BinStream> bs)
	:fStream(bs)
{
	StaticConstructor();
}

// Generate a single token
std::shared_ptr<PSToken> PSScanner::nextToken()
{

	skipspaces(fStream);
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
		} else {
			// do nothing
			return nullptr;
		}
	} else {
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
		} else if (isGraph(c)) 
		{
			fStream->skip(-1);
			auto tok = lex_name();
			//if (fVM->isBuildingProc()) {
			//fVM->OperandStack:push(tok);
			//}
			//else
			//{
				return tok;
			//}
		} else {
			printf("SCANNER UNKNOWN: %c\n", c);
		}
	}

	return nullptr;
}
