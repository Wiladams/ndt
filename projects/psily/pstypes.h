#pragma once

#include <string>
#include <variant>
#include <functional>
#include <iostream>

class PSArray;
class PSDictionary;
class PSVM;

// Enumerate the kinds of tokens that we will see
// This is used everywhere from the scanner to interpreter and VM
enum class PSTokenType : uint32_t
{
	nil,				// a null
	MARK,				// a noop

	// lexical types
	LITERAL_NAME,		// /name
	EXECUTABLE_NAME,	// name

	LITERAL_STRING,		// (string)
	HEXSTRING,			// <hexstring>
	NUMBER,				// int, float, hex
	NUMBER_INT,			// int
	NUMBER_FLOAT,		// float
	BOOLEAN,			// true, false
	COMMENT,			// % to end of line

	// Structural types
	FUNCTION,			// a function which has been bound
	OPERATOR,			// a function which has been bound
	LITERAL_ARRAY,		// []
	PROCEDURE,			// {}
	DICTIONARY,
};

using PSTokenData = std::variant<
	int, 
	bool,
	//uint64_t,
	//float,
	double,
	std::string,
	std::function<void(PSVM& vm)>,
	std::shared_ptr<PSArray>,
	std::shared_ptr<PSDictionary> >;

struct PSToken {
	PSTokenType fType;
	bool fIsExecutable = false;
	//PSTokenData fData;
	
	std::variant<
		int, 
		bool,
		//uint64_t,
		//float,
		double,
		std::string,
		std::function<void(PSVM& vm)>,
		std::shared_ptr<PSArray>,
		std::shared_ptr<PSDictionary> > fData;
	
	PSToken(const PSToken& other)
		:fType(other.fType),
		fIsExecutable(other.fIsExecutable),
		fData(other.fData)
	{
	}

	PSToken(const bool value) : fData(value), fType(PSTokenType::BOOLEAN) {}
	PSToken(const int value) : fData(value), fType(PSTokenType::NUMBER_INT) {}
	PSToken(const double value) : fData(value), fType(PSTokenType::NUMBER) {}
	PSToken(const std::string value, const PSTokenType kind= PSTokenType::LITERAL_STRING) : fData(value), fType(kind) {}
	PSToken(const std::function<void(PSVM& vm)> value) : fData(value), fType(PSTokenType::OPERATOR) {}
	PSToken(const std::shared_ptr<PSArray> value) : fData(value), fType(PSTokenType::LITERAL_ARRAY) {}
	PSToken(const std::shared_ptr<PSDictionary> value) : fData(value), fType(PSTokenType::DICTIONARY) {}

	void setExecutable(const bool value){fIsExecutable = value;}
	bool isExecutable() { return fIsExecutable; }

	operator const bool() { return std::get<bool>(fData); }
	//operator const int() { return std::get<int>(fData); }
	operator const double() { return std::get<double>(fData); }
	operator const std::string& () { return std::get<std::string>(fData); }
	operator const std::function<void(PSVM& vm)>() { return std::get<std::function<void(PSVM& vm)>>(fData); }
	operator const std::shared_ptr<PSArray>() { return std::get<std::shared_ptr<PSArray>>(fData); }
	operator const std::shared_ptr<PSDictionary>() { return std::get<std::shared_ptr<PSDictionary>>(fData); }

	PSToken& operator =(const bool value) { fData = value; return *this; }
	PSToken& operator =(const double value) { fData = value; return *this; }
	PSToken& operator =(const std::string& value) { fData = value; return *this; }
	PSToken& operator =(std::function<void(PSVM& vm)> value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSArray> value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSDictionary> value) { fData = value; return *this; }

};

// A visitor so we can print out token
// values
/*
struct PSTokenPrinter {
	void operator()(const bool value) { std::cout << std::to_string(value); }
	void operator()(const double value) { std::cout << std::to_string(value); }
	void operator()(const std::string& value) { std::cout << value; }
	void operator()(std::function<void(PSVM& vm)> value) { std::cout << "FUNCTION"; }
	void operator()(std::shared_ptr<PSArray> value) { std::cout << "ARRAY"; }
	void operator()(std::shared_ptr<PSDictionary> value) { std::cout << "DICTIONARY"; }
};
*/

std::ostream& operator << (std::ostream& os, PSTokenData const& v) {
	std::visit([&os](auto const& e) {os << e; }, v);
	return os;
}

#include "psstack.h"
#include "psarray.h"
#include "psdictionary.h"

#include "psmatrix.h"