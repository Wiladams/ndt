#include "psvm.h"
#include "textscan.h"

#include <cstring>

#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>

using std::shared_ptr;
using std::make_shared;


// Setup a dispatch table for base operators
//class PSDictionary : public std::unordered_map<std::string, std::shared_ptr<PSToken> >

//std::unordered_map < std::string, std::shared_ptr<PSToken> > PSOperators
std::unordered_map < std::string, std::function<void(PSVM & vm)> > PSOperators
{
	// Stack management
	{"clear", [](PSVM& vm) {vm.operandStack().clear(); }},
	
	{"cleartomark", [](PSVM& vm) { vm.operandStack().clearToMark(); }},

	{"copy", [](PSVM& vm) {
		auto tok = vm.operandStack().pop();
		auto n = tok->fData.asInt;
		vm.operandStack().copy(n);
	}},

	{"count", [](PSVM& vm) {
		auto len = vm.operandStack().length();
		auto tok = make_shared<PSToken>((double)len);
		vm.operandStack().push(tok);
	}},

	{"counttomark", [](PSVM& vm) {
		auto n = vm.operandStack().countToMark();
		auto tok = make_shared<PSToken>((double)n);

		vm.operandStack().push(tok);
	}},

	{"dup", [](PSVM& vm) {vm.operandStack().dup(); }},

	{"exch", [](PSVM& vm) {vm.operandStack().exch(); }},

	{"index", [](PSVM& vm) {
		auto n = vm.operandStack().pop();
		auto value = vm.operandStack().nth(n->fData.asInt);
		vm.operandStack().push(value);
	}},

	{"mark", [](PSVM& vm) { vm.operandStack().mark(); }},

	{"pop", [](PSVM& vm) {vm.operandStack().pop(); }},

	{"roll", [](PSVM& vm) {
		auto j = vm.operandStack().pop();
		auto n = vm.operandStack().pop();
		vm.operandStack().roll(n->fData.asInt, j->fData.asInt);
	}},

	//{"top", [](PSVM& vm) {vm.operandStack().top(); }},
	


	//
	// Arithmetic and Mathematical Operators
	// Pop two arguments off stack, put result back on stack
	{"add", [](PSVM& vm) {
		auto num2 = vm.operandStack().pop();
		auto num1 = vm.operandStack().pop();
		auto tok = make_shared<PSToken>((double)num1->fData.asReal + num2->fData.asReal);

		vm.operandStack().push(tok);
	}},

	{"sub", [](PSVM& vm) {
		auto num2 = vm.operandStack().pop();
		auto num1 = vm.operandStack().pop();
		auto tok = make_shared<PSToken>((double)num1->fData.asReal - num2->fData.asReal);

		vm.operandStack().push(tok);
	}},
	
	{"mul", [](PSVM& vm) {
		auto num2 = vm.operandStack().pop();
		auto num1 = vm.operandStack().pop();
		auto tok = make_shared<PSToken>((double)num1->fData.asReal * num2->fData.asReal);

		vm.operandStack().push(tok);
	}},
	
	
	{"div", [](PSVM& vm) {
		auto num2 = vm.operandStack().pop();
		auto num1 = vm.operandStack().pop();
		auto tok = make_shared<PSToken>((double)num1->fData.asReal / num2->fData.asReal);

		vm.operandStack().push(tok);
	}},

	{"idiv", [](PSVM& vm) {
		auto b = vm.operandStack().pop();
		auto a = vm.operandStack().pop();
		auto q = (double)a->fData.asReal / b->fData.asReal;
		if (q >= 0) {
			q = floor(q);
		}
		else {
			q = ceil(q);
		}
		auto tok = make_shared<PSToken>(q);

		vm.operandStack().push(tok);
	}},

	{"mod", [](PSVM& vm) {
		auto b = vm.operandStack().pop();
		auto a = vm.operandStack().pop();
		auto value = fmod(a->fData.asReal, b->fData.asReal);

		auto tok = make_shared<PSToken>(value);

		vm.operandStack().push(tok);
	}},

	{ "maximum", [](PSVM& vm) {
		auto b = vm.operandStack().pop();
		auto a = vm.operandStack().pop();
		auto value = std::max(a->fData.asReal, b->fData.asReal);

		auto tok = make_shared<PSToken>(value);

		vm.operandStack().push(tok);
	}},

	{ "minimum", [](PSVM& vm) {
		auto b = vm.operandStack().pop();
		auto a = vm.operandStack().pop();
		auto value = std::min(a->fData.asReal, b->fData.asReal);

		auto tok = make_shared<PSToken>(value);

		vm.operandStack().push(tok);
	}},

};




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
	//std::string value((char*)startPtr, len);

	// Skip over closing delimeter
	bs->skip(1);

	auto tok = std::make_shared<PSToken>((char*)startPtr, len, PSTokenType::LITERAL_STRING);

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
	arr->isExecutable = true;
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
		if (op->isExecutable) {
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
		if (tok == nullptr)
			break;

		printf("token: %s\n", tok->toString().c_str());

		if (tok->fType == PSTokenType::EXECUTABLE_NAME) {
			execName(tok);
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
	for (std::pair<std::string, std::function<void(PSVM& vm)> > elem : PSOperators) {
		fDictionaryStack.def(elem.first, make_shared<PSToken>(elem.second));
	}

}