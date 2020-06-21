#pragma once

#include <cstdint>
#include <string>
#include <cstdlib>
#include <memory>
#include <unordered_map>

// Enumerate the kinds of tokens that we will see
// This is used everywhere from the scanner to interpreter and VM
enum class PSTokenType : uint32_t
{
	MARK,				// a noop

	// lexical types
	LITERAL_NAME,		// /name
	EXECUTABLE_NAME,	// name

	LITERAL_STRING,				// (string)
	HEXSTRING,			// <hexstring>
	NUMBER,				// int, float, hex
	NUMBER_INT,			// int
	NUMBER_FLOAT,		// float
	BOOLEAN,			// true, false
	COMMENT,			// % to end of line

	// Structural types

	LITERAL_ARRAY,		// []
	PROCEDURE,			// {}
	OPERATOR,			// a function which has been bound
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

	intptr_t	asPointer;

	std::string asString;

	PSTokenData()		// Need a default constructor
	{
		asInt = 0;
	}

	~PSTokenData(){}	// Need this destructor
};

// The PSToken holds the data type, the actual data
// and any flags we want to associated with the token
// such as whether it is executable or not
struct PSToken
{
	PSTokenType fType;
	uint32_t fFlags;
	PSTokenData fData;
	
	PSToken()
		:fType(PSTokenType::MARK)
	{
	}

	PSToken(PSTokenType t, std::string &aString)
		: fType(t)
	{
		fData.asString = aString;
	}
	
	PSToken(PSTokenType t, bool aBool)
		:fType(t)
	{
		fData.asBool = aBool;
	}

	PSToken(PSTokenType t, double aReal)
		:fType(t)
	{
		fData.asReal = aReal;
	}

	~PSToken() {}

	operator int() { return fData.asInt; }

	std::string toString()
	{
		switch (fType) {
		case PSTokenType::LITERAL_ARRAY:
			return "LITERAL_ARRAY";

		case PSTokenType::PROCEDURE:
			return "PROCEDURE";

		case PSTokenType::OPERATOR:
			return "OPERATOR";

		case PSTokenType::LITERAL_NAME:
			return "LITERAL_NAME";

		case PSTokenType::EXECUTABLE_NAME:
			return std::string("EXECUTABLE_NAME: ")+fData.asString;

		case PSTokenType::LITERAL_STRING:
			return std::string("LITERAL_STRING: ") + fData.asString;

		case PSTokenType::HEXSTRING:
			return "HEXSTRING";

		case PSTokenType::NUMBER:
			return std::string("NUMBER: ")+std::to_string(fData.asReal);

		case PSTokenType::NUMBER_INT:
			return "NUMBER_INT";

		case PSTokenType::NUMBER_FLOAT:
			return "NUMBER_FLOAT";

		case PSTokenType::BOOLEAN:
			return std::string("BOOLEAN: "+std::to_string(fData.asBool));

		case PSTokenType::COMMENT:
			return std::string("COMMENT: ")+fData.asString;
		}

		return "UNKNOWN";
	}
};