#pragma once

#include <new>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <memory>
#include <unordered_map>
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
	OPERATOR,			// a function which has been bound
	LITERAL_ARRAY,		// []
	PROCEDURE,			// {}
	DICTIONARY,
};

// PSTokenData
// The union data structure used to hold the actual
// data associated with a token.  Doing it as a union
// 'class' will use more storage, but greatly simplifies
// the storage and usage.
union PSTokenData
{
	bool		asBool;
	int			asInt;
	uint64_t	asLongLong;

	float		asFloat;
	double		asReal;

	//intptr_t	asPointer;

	std::string * asString;
	
	std::function<void (PSVM & vm)> asOperator;
	std::shared_ptr<PSArray> asArray;
	std::shared_ptr<PSDictionary> asDictionary;

	PSTokenData() {}	// Need default constructor
	
	// Create these constructors for completeness
	PSTokenData(const bool value) :asBool(value) {}
	PSTokenData(const int value) :asInt(value) {}
	PSTokenData(const uint64_t value) :asLongLong(value) {}
	PSTokenData(const float value) :asFloat(value) {}
	PSTokenData(const double value) :asReal(value) {}
	//PSTokenData(const intptr_t value) :asPointer(value) {}

	// Create actual constructors for non-trivial types
	PSTokenData(const std::function<void(PSVM& vm)> &op)
		: asOperator(op){}
	
	PSTokenData(std::shared_ptr<PSDictionary> d)
		: asDictionary(d) {}

	PSTokenData(std::shared_ptr<PSArray> a)
		: asArray(a) {}

	~PSTokenData(){}	// Need this destructor
};

// The PSToken holds the data type, the actual data
// and any flags we want to associated with the token
// such as whether it is executable or not
struct PSToken
{
private:
	bool fIsExecutable;

	PSToken(const PSToken& other) = delete;

public:
	PSTokenType fType;

	PSTokenData fData;

	PSToken()
		:fType(PSTokenType::MARK)
		, fIsExecutable(false)
	{
	}

	PSToken(const PSTokenType kind)
		: fType(kind), fIsExecutable(false) {}

	PSToken(const char *data, const int len, PSTokenType t)
		: fType(t)
		, fIsExecutable(false)
	{
		fData.asString = new std::string(data, len);
	}


	PSToken(const std::string &aString, PSTokenType t)
		: fType(t)
		, fIsExecutable(false)
	{
		fData.asString = new std::string(aString);
	}

	
	PSToken(bool aBool, PSTokenType t)
		:fType(t)
		, fIsExecutable(false)
	{
		fData.asBool = aBool;
	}

	PSToken(double aReal)
		:fType(PSTokenType::NUMBER)
		, fIsExecutable(false)
	{
		fData.asReal = aReal;
	}

	PSToken(std::shared_ptr<PSArray> value)
		:fType(PSTokenType::LITERAL_ARRAY)
		, fIsExecutable(false)
		, fData(value)
	{
	}

	PSToken(std::shared_ptr<PSDictionary> d)
		:fType(PSTokenType::DICTIONARY)
		, fIsExecutable(false)
		,fData(d)
	{
	}

	PSToken(std::function<void(PSVM & vm)> op)
		:fType(PSTokenType::OPERATOR)
		,fIsExecutable(true)
		,fData(op)
	{

	}

	// BUGBUG - need to do type specific destruction for complex types
	~PSToken() {}



	void setExecutable(const bool value)
	{
		fIsExecutable = value;
	}

	bool isExecutable() { return fIsExecutable; }

	std::string toString()
	{
		switch (fType) {
		case PSTokenType::LITERAL_ARRAY:
			return std::string("LITERAL_ARRAY");

		case PSTokenType::PROCEDURE:
			return std::string("PROCEDURE");

		case PSTokenType::OPERATOR:
			return std::string("OPERATOR");

		case PSTokenType::LITERAL_NAME:
			return std::string("LITERAL_NAME");

		case PSTokenType::EXECUTABLE_NAME:
			return std::string("EXECUTABLE_NAME: ")+*fData.asString;

		case PSTokenType::LITERAL_STRING:
			return std::string("LITERAL_STRING: ") + *fData.asString;

		case PSTokenType::HEXSTRING:
			return std::string("HEXSTRING");

		case PSTokenType::NUMBER:
			return std::string("NUMBER: ")+std::to_string(fData.asReal);

		case PSTokenType::NUMBER_INT:
			return std::string("NUMBER_INT");

		case PSTokenType::NUMBER_FLOAT:
			return std::string("NUMBER_FLOAT");

		case PSTokenType::BOOLEAN:
			return std::string("BOOLEAN: "+std::to_string(fData.asBool));

		case PSTokenType::COMMENT:
			return std::string("COMMENT: ")+*fData.asString;
		}

		return std::string("UNKNOWN");
	}
};

inline std::ostream& operator<<(std::ostream& out, const PSToken & tok)
{
	//printf("TOKENOUT: %d\n", (int)tok.fType);

	switch (tok.fType) {
	case PSTokenType::LITERAL_ARRAY:
		return out << std::string("LITERAL_ARRAY");

	case PSTokenType::PROCEDURE:
		return out << std::string("PROCEDURE");

	case PSTokenType::OPERATOR:
		return out << std::string("OPERATOR");

	case PSTokenType::LITERAL_NAME:
		return out << *tok.fData.asString;

	case PSTokenType::EXECUTABLE_NAME:
		return out << *tok.fData.asString;

	case PSTokenType::LITERAL_STRING:
		//std::cout << "LITERAL STRING: ";
		return out << *tok.fData.asString;

	case PSTokenType::HEXSTRING:
		return out << *tok.fData.asString;

	case PSTokenType::NUMBER:
		return out << std::to_string(tok.fData.asReal);

	case PSTokenType::NUMBER_INT:
		return out << std::to_string(tok.fData.asInt);

	case PSTokenType::NUMBER_FLOAT:
		return out << std::to_string(tok.fData.asFloat);

	case PSTokenType::BOOLEAN:
		return out << std::to_string(tok.fData.asBool);

	case PSTokenType::COMMENT:
		return out << *tok.fData.asString;
	}
}