#pragma once



#include <array>
#include <string>
#include <variant>
#include <functional>
#include <iostream>
#include <deque>
#include <memory>

#include "filestream.h"
#include "maths.hpp"

class PSVM;
struct PSToken;

// Enumerate the kinds of tokens that we will see
// This is used everywhere from the scanner to interpreter and VM
enum struct PSTokenType : uint32_t
{
	NIL,				// a null
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
	FILESTREAM,				// a file stream
};

class PSArray : public std::deque<std::shared_ptr<PSToken> >
{
public:
	PSArray() {}
};

using PS_Operator = std::function< void (PSVM& vm) >;

struct PSPoint {
	double x;
	double y;
};

struct PSString : public std::string
{
	size_t fCapacity;

	PSString(const char* s)
		:std::string(s)
	{
		fCapacity = strlen(s);
	}

	PSString(const size_t s)
		: fCapacity(s)
	{
	}
	/*
	// returning a value based on index
	char operator[](const size_t idx) const
	{
		return fStorage[idx];
	}

	// setting a value based on index
	char& operator[](const size_t idx)
	{
		return fStorage[idx];
	}
	*/
	void put(int idx, int c)
	{
		this->operator[](idx) = c;
	}

	int get(int idx) const
	{
		return this->operator[](idx);
	}

	size_t length()
	{
		return fCapacity;
	}

	PSString getInterval(const int idx, const size_t count)
	{
		int actual = (int)maths::min(float(count), float((int)length() - idx));
		if (actual < 0) {
			return PSString((size_t)0);
		}

		// Create a new array with capacity == count
		PSString arr(count);
		if (actual == 0) {
			return arr;
		}

		for (int nidx = 0; nidx < actual; nidx++)
		{
			arr[nidx] = this->operator[](idx + nidx);
		}

		return arr;
	}

};

/*
	A PSMatrix object with Postscript transformation
	behavior and representation.  It is essentially
	a truncated 3x3 matrix.

	a	b	==>	m00  m01  0
	c	d	==> m10  m11  0
	tx ty	==> m20  m21  1
*/

// BUGBUG - would it be better to use std::variant?
struct PSMatrix {
	union {
		struct {
			double m00;
			double m01;
			double m10;
			double m11;
			double m20;
			double m21;
		};
		double m[6];
	};

	//
	// default constructor
	PSMatrix()
	{
		for (size_t i = 0; i < 6; i++)
		{
			m[i] = 0.0;
		}
	}

	PSMatrix(const double d0, const double d1, const double d2, const double d3, const double d4, const double d5)
	{
		m[0] = d0;
		m[1] = d1;
		m[2] = d2;
		m[3] = d3;
		m[4] = d4;
		m[5] = d5;
	}

	/*
		Copy constructor - clone
	*/
	PSMatrix(const PSMatrix& other)
	{
		for (size_t i = 0; i < 6; i++)
		{
			m[i] = other.m[i];
		}
	}
	/*
	// Create a matrix from a Postscript Array
	// The array contains token values
	static inline PSMatrix createFromArray(const PSArray& arr)
	{
		PSMatrix m{ arr[0]->asDouble(),
			arr[1]->asDouble(),
			arr[2]->asDouble(),
			arr[3]->asDouble(),
			arr[4]->asDouble(),
			arr[5]->asDouble() };

		return m;
	}
	*/
	// Factory constructors
	static inline PSMatrix createIdentity()
	{
		PSMatrix m{ 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

		return m;
	}

	static inline PSMatrix createTranslation(const double tx, const double ty)
	{
		PSMatrix m{ 1.0, 0.0, 0.0, 1.0, tx, ty };

		return m;
	}

	/*
		Rotation counter clockwise about a point
		by an angle specified in degrees

			cos(a)  sin(a)  0
		   -sin(a)  cos(a)  0
			 0       0      1
	*/
	static inline PSMatrix createRotation(const double angle, const double cx = 0, const double cy = 0)
	{
		auto rads = maths::radians(angle);
		auto rcos = maths::cos(rads);
		auto rsin = maths::sin(rads);

		PSMatrix m{ rcos, rsin, -rsin, rcos, cx, cy };
		return m;
	}

	static inline PSMatrix createScaling(const double sx, const double sy)
	{
		PSMatrix m{ sx, 0.0, 0.0, sy, 0.0, 0.0 };
		return m;
	}


	//
	// Instance Methods
	//

	// preMultiplyBy
	// this = other * this
	//
	PSMatrix& preMultiplyBy(const PSMatrix& other)
	{
		auto a = other.m00 * m00 + other.m01 * m10;
		auto b = other.m00 * m01 + other.m01 * m11;

		auto c = other.m10 * m00 + other.m11 * m10;
		auto d = other.m10 * m01 + other.m11 * m11;

		auto tx = other.m20 * m00 + other.m21 * m10 + m20;
		auto ty = other.m20 * m01 + other.m21 * m11 + m21;

		m00 = a;
		m01 = b;
		m10 = c;
		m11 = d;
		m20 = tx;
		m21 = ty;

		return *this;
	}



	inline double determinant()
	{
		return m00 * m11 - m01 * m10;
	}

	// BUGBUG - this should be optional
	PSMatrix inverse()
	{
		double d = determinant();

		// if (d == 0)
		// return nullptr;

		auto t00 = m11;
		auto t01 = m01;
		auto t10 = m10;
		auto t11 = m00;

		t00 = t00 / d;
		t01 = t01 / d;
		t10 = t10 / d;
		t11 = t11 / d;

		auto t20 = -(m20 * t00 + m21 * t10);
		auto t21 = -(m20 * t01 + m21 * t11);

		PSMatrix m1{ t00,t01,t10,t11,t20,t21 };

		return m1;
	}

	PSMatrix& rotate(const double angle, const double cx, const double cy)
	{
		PSMatrix r = PSMatrix::createRotation(angle, cx, cy);
		preMultiplyBy(r);

		return *this;
	}

	PSMatrix& scale(const double sx, const double sy)
	{
		m00 = m00 * sx;
		m01 = m01 * sx;
		m10 = m10 * sy;
		m11 = m11 * sy;

		return *this;
	}

	PSMatrix& translate(const double tx, const double ty)
	{
		double x1 = 0;
		double y1 = 0;
		transformPoint(tx, ty, x1, y1);
		m20 = m20 + x1;
		m21 = m21 + y1;

		return *this;
	}

	//
	// Apply the matrix to transforming points
	// transformPoint
	// transform an incoming point by the transformation matrix
	//
	void transformPoint(const double x, const double y, double& x1, double& y1)
	{
		x1 = m00 * x + m10 * y + m20;
		y1 = m01 * x + m11 * y + m21;
	}

	void dtransform(const double x, const double y, double& x1, double& y1)
	{
		x1 = m00 * x + m10 * y;
		y1 = m01 * x + m11 * y;
	}

	void scalePoint(const double x, const double y, double& x1, double& y1)
	{
		auto m1 = PSMatrix::createScaling(m00, m11);
		m1.transformPoint(x, y, x1, y1);
	}
};






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
	PSString,
	PS_Operator,
	std::shared_ptr<PSArray>,
	std::shared_ptr<PSDictionary>,
	std::shared_ptr<FileStream> > ;

struct PSToken {
	PSTokenData fData; 
	PSTokenType fType;
	bool fIsExecutable = false;


	// Constructing a token
	PSToken(const PSTokenType t) :fData(true), fType(t) {}
	PSToken(const bool value) : fData(value), fType(PSTokenType::BOOLEAN) {}
	PSToken(const int value) : fData(value), fType(PSTokenType::NUMBER_INT) {}
	PSToken(const double value) : fData(value), fType(PSTokenType::NUMBER) {}
	PSToken(const PSString value, const PSTokenType kind = PSTokenType::LITERAL_STRING) : fData(value), fType(kind) {}
	PSToken(PS_Operator value) : fData(value), fType(PSTokenType::OPERATOR), fIsExecutable(true) {}
	PSToken(std::shared_ptr<PSArray> value) : fData(value), fType(PSTokenType::LITERAL_ARRAY) {}
	PSToken(std::shared_ptr<PSDictionary> value) : fData(value), fType(PSTokenType::DICTIONARY) {}
	PSToken(std::shared_ptr<FileStream> value) : fData(value), fType(PSTokenType::FILESTREAM) {}

	void setExecutable(const bool value){fIsExecutable = value;}
	bool isExecutable() { return fIsExecutable; }

	void setType(const PSTokenType value) { fType = value; }

	PSToken& operator =(const bool value) { fData = value; return *this; }
	PSToken& operator =(const double value) { fData = value; return *this; }
	PSToken& operator =(const PSString& value) { fData = value; return *this; }
	PSToken& operator =(PS_Operator value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSArray> value) { fData = value; return *this; }
	PSToken& operator =(std::shared_ptr<PSDictionary> value) { fData = value; return *this; }

	
	bool asBool() const { return std::get<bool>(fData); }
	bool asInt() const { return std::get<int>(fData); }
	double asDouble() const { return std::get<double>(fData); }
	const PSString& asString() const { return std::get<PSString>(fData); }
	PS_Operator asFunction() const { return std::get<PS_Operator>(fData); }
	std::shared_ptr<PSArray> asArray() const { return std::get<std::shared_ptr<PSArray>>(fData); }
	std::shared_ptr<PSDictionary> asDictionary() const { return std::get<std::shared_ptr<PSDictionary>>(fData); }
	std::shared_ptr<FileStream> asFile() const { return std::get<std::shared_ptr<FileStream>>(fData); }

	std::ostream& printValue(std::ostream& os)
	{
		switch (fType) {
			case PSTokenType::MARK:
				os << "MARK";
			break;
			
			case PSTokenType::NUMBER_INT:
				os << std::get<int>(fData);
				break;

			case PSTokenType::NUMBER:
				os << std::get<double>(fData);
			break;

			case PSTokenType::COMMENT:
			case PSTokenType::EXECUTABLE_NAME:
			case PSTokenType::LITERAL_NAME:
			case PSTokenType::LITERAL_STRING:
				os << std::get<PSString>(fData);
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

			case PSTokenType::FILESTREAM:
				os << "FILESTREAM";
			break;

			default:
				os << "UNKNOWN";
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

			case PSTokenType::FILESTREAM:
				os << "FILESTREAM" << std::endl;
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
