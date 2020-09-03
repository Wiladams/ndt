#pragma once

#include <string>
#include <variant>
#include <functional>
#include <iostream>
#include <deque>
#include <memory>

class PSVM;
struct PSToken;

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
	OPERATOR,			// a function which has been bound
	LITERAL_ARRAY,		// []
	PROCEDURE,			// {}
	DICTIONARY,
};


using PS_Operator = std::function< void (PSVM& vm) >;




class PSArray : public std::deque<std::shared_ptr<PSToken> >
{
public:
	PSArray() {}
};

// BUGBUG - The dictionary should probably be full of tokens
// not just operators
class PSDictionary : public std::unordered_map<std::string, std::shared_ptr<PSToken> >
{

public:
	PSDictionary() {}

	PSDictionary(std::unordered_map < std::string, std::function<void(PSVM& vm)> > ops)
	{
		addOperators(ops);
	}

	void addOperators(std::unordered_map < std::string, std::function<void(PSVM& vm)> > ops)
	{
		for (auto& it : ops)
		{
			insert({ it.first, std::make_shared<PSToken>(it.second) });
		}
	}
};

using PSTokenData = std::variant<
	int,
	bool,

	double,
	std::string,
	PS_Operator,
	std::shared_ptr<PSArray>,
	std::shared_ptr<PSDictionary> >;

struct PSToken {
	PSTokenType fType;
	bool fIsExecutable = false;
	PSTokenData fData;

	// Constructing a token
	PSToken(const PSTokenType t) :fData(true), fType(t) {}
	PSToken(const bool value) : fData(value), fType(PSTokenType::BOOLEAN) {}
	PSToken(const double value) : fData(value), fType(PSTokenType::NUMBER) {}
	PSToken(const std::string value, const PSTokenType kind= PSTokenType::LITERAL_STRING) : fData(value), fType(kind) {}
	PSToken(PS_Operator value) : fData(value), fType(PSTokenType::OPERATOR), fIsExecutable(true) {}
	PSToken(std::shared_ptr<PSArray> value) : fData(value), fType(PSTokenType::LITERAL_ARRAY) {}
	PSToken(std::shared_ptr<PSDictionary> value) : fData(value), fType(PSTokenType::DICTIONARY) {}

	void setExecutable(const bool value){fIsExecutable = value;}
	bool isExecutable() { return fIsExecutable; }

	void setType(const PSTokenType value) { fType = value; }

	PSToken& operator =(const bool value) { fData = value; return *this; }
	PSToken& operator =(const double value) { fData = value; return *this; }
	PSToken& operator =(const std::string& value) { fData = value; return *this; }
	PSToken& operator =(PS_Operator value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSArray> value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSDictionary> value) { fData = value; return *this; }

	
	bool asBool() const { return std::get<bool>(fData); }
	double asDouble() const { return std::get<double>(fData); }
	const std::string& asString() const { return std::get<std::string>(fData); }
	PS_Operator asFunction() const { return std::get<PS_Operator>(fData); }
	std::shared_ptr<PSArray> asArray() const { return std::get<std::shared_ptr<PSArray>>(fData); }
	std::shared_ptr<PSDictionary> asDictionary() const { return std::get<std::shared_ptr<PSDictionary>>(fData); }


	std::ostream& printValue(std::ostream& os)
	{
		switch (fType) {
			case PSTokenType::MARK:
				os << "MARK";
			break;
			
			case PSTokenType::NUMBER:
				os << std::get<double>(fData);
			break;

			case PSTokenType::COMMENT:
			case PSTokenType::EXECUTABLE_NAME:
			case PSTokenType::LITERAL_NAME:
			case PSTokenType::LITERAL_STRING:
				os << std::get<std::string>(fData);
			break;


			case PSTokenType::LITERAL_ARRAY: 
			{
				os << "ARRAY";
			}
			break;
		
			case PSTokenType::DICTIONARY:
				os << "DICTIONARY";
			break;

			case PSTokenType::PROCEDURE:
				os << "PROCEDURE";
			break;

			case PSTokenType::OPERATOR:
				os << "OPERATOR";
			break;
		}

		return os;
	}

	std::ostream& printFullValue(std::ostream& os)
	{
		switch (fType) 
		{

			case PSTokenType::LITERAL_ARRAY:
			{
				auto arr = *asArray();
			
				os << "ARRAY: " << std::to_string(arr.size()) << std::endl;
				for (auto& it : arr) {
					it->printValue(os) << std::endl;
				}
			}
			break;

			case PSTokenType::PROCEDURE:
			{
				auto arr = *asArray();

				os << "PROCEDURE: " << std::to_string(arr.size()) << std::endl;
				for (auto& it : arr) {
					it->printFullValue(os) << std::endl;
				}
			}
			break;

			default:
				printValue(os) << std::endl;
			break;

		}

		return os;
	}
};

#include "psstack.h"
#include "psdictionary.h"
