#pragma once

#include "psvm.h"
#include "maths.hpp"

#include <cmath>
#include <unordered_map>
#include <random>
#include <iostream>

// Setup a table to contain the base operators
// These should be copied into a dictionary
// as OPERATOR tokens
std::unordered_map < std::string, PS_Operator > PSBaseOperators
{
	// Stack management
	{"clear", [](PSVM& vm) {vm.operandStack().clear(); }},

	{"cleartomark", [](PSVM& vm) { vm.operandStack().clearToMark(); }},

	{"copy", [](PSVM& vm) {
		auto tok = vm.operandStack().pop();
		auto n = tok->asDouble();
		vm.operandStack().copy((size_t)n);
	}},

	{"count", [](PSVM& vm) {
		auto len = vm.operandStack().length();
		auto tok = make_shared<PSToken>((double)len);
		vm.pushOperand(tok);
	}},

	{"counttomark", [](PSVM& vm) {
		auto n = vm.operandStack().countToMark();
		auto tok = make_shared<PSToken>((double)n);

		vm.pushOperand(tok);
	}},

	{"dup", [](PSVM& vm) {vm.operandStack().dup(); }},

	{"exch", [](PSVM& vm) {vm.operandStack().exch(); }},

	{"index", [](PSVM& vm) {
		auto n = vm.popOperand()->asDouble();
		auto tok = vm.operandStack().nth((size_t)n);
		vm.pushOperand(tok);
	}},

	{"mark", [](PSVM& vm) { vm.operandStack().mark(); }},

	{"pop", [](PSVM& vm) {vm.operandStack().pop(); }},

	{"roll", [](PSVM& vm) {
		auto j = vm.popOperand()->asDouble();
		auto n = vm.popOperand()->asDouble();
		vm.operandStack().roll((int)n, (int)j);
	}},

	//{"top", [](PSVM& vm) {vm.operandStack().top(); }},



	//
	// Arithmetic and Mathematical Operators
	// Pop two arguments off stack, put result back on stack
	{"add", [](PSVM& vm) {
			auto num2 = vm.popOperand()->asDouble();
			auto num1 = vm.popOperand()->asDouble();
			auto tok = make_shared<PSToken>(num1 + num2);

			vm.pushOperand(tok);
	}},

	// atan
	{ "atan", [](PSVM& vm) {
		auto den = vm.popOperand()->asDouble();
		auto num = vm.popOperand()->asDouble();
		auto value = maths::Degrees(std::atan(num / den));
		vm.pushOperand(make_shared<PSToken>(value));
	} },
	
	// sub
	// subtraction
	{"sub", [](PSVM& vm) {
			auto num2 = vm.popOperand()->asDouble();
			auto num1 = vm.popOperand()->asDouble();
			auto tok = make_shared<PSToken>(num1 - num2);

			vm.pushOperand(tok);
	}},

	// mul
	// multiplication
	// pop two, push one
	{"mul", [](PSVM& vm) {
			auto num2 = vm.popOperand()->asDouble();
			auto num1 = vm.popOperand()->asDouble();
			auto tok = make_shared<PSToken>(num1 * num2);

			vm.pushOperand(tok);
	}},


	{"div", [](PSVM& vm) {
			auto num2 = vm.popOperand()->asDouble();
			auto num1 = vm.popOperand()->asDouble();
			auto tok = make_shared<PSToken>(num1 / num2);

			vm.pushOperand(tok);
	}},
	
	// exp
	{"exp", [](PSVM& vm) {
			auto base = vm.popOperand()->asDouble();
			auto exponent = vm.popOperand()->asDouble();
			auto tok = make_shared<PSToken>(std::pow(base ,exponent));

			vm.pushOperand(tok);
	}},

	{"idiv", [](PSVM& vm) {
			auto b = vm.popOperand()->asDouble();
			auto a = vm.popOperand()->asDouble();
			auto q = a / b;
			if (q >= 0) {
				q = floor(q);
			}
			else {
				q = ceil(q);
			}
			auto tok = make_shared<PSToken>(q);

			vm.pushOperand(tok);
	}},

	{"mod", [](PSVM& vm) {
			auto b = vm.popOperand()->asDouble();
			auto a = vm.popOperand()->asDouble();
			auto value = fmod(a, b);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	{ "maximum", [](PSVM& vm) {
			auto b = vm.popOperand()->asDouble();
			auto a = vm.popOperand()->asDouble();
			auto value = std::max(a, b);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	{ "minimum", [](PSVM& vm) {
			auto b = vm.popOperand()->asDouble();
			auto a = vm.popOperand()->asDouble();
			auto value = std::min(a, b);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	// Single argument arithmetic operators
	// abs
	{ "abs", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		auto tok = make_shared<PSToken>(std::abs(a));

		vm.pushOperand(tok);

	} },

	// ceiling
	{ "ceiling", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::ceil(a)));
	} },

	// floor
	{ "floor", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::floor(a)));
	} },

	// neg
	{ "neg", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(-(a)));
	} },
	
	// round
	{ "round", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::round(a)));
	} },
	
	// truncate
	{ "truncate", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::trunc(a)));
	} },

	// sqrt
	{ "sqrt", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::sqrt(a)));
	} },


	
	// ln
	{ "ln", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::log(a)));
	} },
	
	// log
	{ "log", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::log10(a)));
	} },

	// sin
	{ "sin", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::sin(a)));
	} },
	
	// cos
	{ "cos", [](PSVM& vm) {
		auto a = vm.popOperand()->asDouble();
		vm.pushOperand(make_shared<PSToken>(std::sin(a)));
	} },
	


	// rand
	{ "rand", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>((double)vm.randomInt()));
	} },
	
	// srand
	{ "srand", [](PSVM& vm) {
		auto seed = vm.popOperand()->asDouble();
		vm.seedRandomInt((unsigned int)seed);
	} },

	// rrand

	//
	// Array, Packed Array, Dictionary, and string operators
	//
	// get
	{ "get", [](PSVM& vm) {
		auto idx = (size_t)vm.popOperand()->asDouble();
		auto arr = vm.popOperand()->asArray();
		auto value = arr->operator[](idx);
		// BUGBUG - check for range and stick a nil token if nothing
		vm.pushOperand(value);
	} },

			// put
			// getinterval
			// putinterval

	// Dictionary operations
	// userdict
			
	// def
	{ "def", [](PSVM& vm) {
		auto value = vm.popOperand();
		auto key = vm.popOperand();
		auto keyName = key->asString();

		vm.dictionaryStack().def(keyName, value);
	} },
	
	// load
	{ "load", [](PSVM& vm) {
		auto keyName = vm.popOperand()->asString();
		auto value = vm.dictionaryStack().load(keyName);

		if (nullptr == value) {
			// print undefined key
			// BUGBUG
			return;
		}

		vm.pushOperand(value);
	} },

	// store
	{ "store", [](PSVM& vm) {
		auto value = vm.popOperand();
		auto keyName = vm.popOperand()->asString();

		vm.dictionaryStack().store(keyName, value);
	} },

	// where
	{ "where", [](PSVM& vm) {
		auto key = vm.popOperand()->asString();
		auto d = vm.dictionaryStack().where(key);

		if (nullptr == d) {
			vm.pushOperand(make_shared<PSToken>(false));
		} else {
			vm.pushOperand(make_shared<PSToken>(d));
			vm.pushOperand(make_shared<PSToken>(true));
		}
	} },

	// length

	
	// Array creation

	// astore
	{ "astore", [](PSVM& vm) {
		auto arrtok = vm.popOperand();
		auto arr = arrtok->asArray();

		auto size = arr->size();

		for (size_t i = 0; i < size; i++) {
			auto value = vm.popOperand();
			arr->operator[](size - i-1) = value;
		}

		vm.pushOperand(arrtok);
	} },

	// aload
	
	// begin
	// pop a dictionary off the operand stack
	// make it the current dictionary
	// by placing it atop the dictionary stack
	{ "begin", [](PSVM& vm) {
		auto tok = vm.popOperand();
		auto d = tok->asDictionary();

		vm.dictionaryStack().pushDictionary(d);
	} },

	// end
	{ "end", [](PSVM& vm) {
		auto d = vm.dictionaryStack().popDictionary();
	} },

	// [ - beginArray, mark
	// scanner takes care of this one

	
	// ] - endArray
	// scanner takes care of this one

	// array
	// packedarray
	
	// dict
	
	// string
	
	// search
	
	// anchorsearch
	
	//
	// Type, Attribute and conversion operators
	//

	// type
	// print the type of the operand on the top of the stack
	{ "type", [](PSVM& vm) {
		auto a = vm.operandStack().top();
		auto tok = make_shared<PSToken>("");
		switch (a->fType) {
			PSTokenType::LITERAL_ARRAY:
			break;

			PSTokenType::BOOLEAN:
				PSTokenType::DICTIONARY:
				PSTokenType::NUMBER_INT :
				PSTokenType::MARK :
				PSTokenType::LITERAL_NAME :
				PSTokenType::nil :
				PSTokenType::OPERATOR :
				PSTokenType::NUMBER :
				PSTokenType::LITERAL_STRING :
				break;
		}
		tok->setExecutable(true);
		vm.pushOperand(tok);
	} },

	// xcheck
	{ "xcheck", [](PSVM& vm) {
		auto a = vm.operandStack().top();
		if (a->isExecutable()) {
			vm.pushOperand(make_shared<PSToken>(true));
		}
		else {
			vm.pushOperand(make_shared<PSToken>(false));
		}
	} },

	// rcheck
	// wcheck
	// cvlit

	// cvx
	{ "cvx", [](PSVM& vm) {
			auto a = vm.popOperand();
			a->setExecutable(true);
			vm.pushOperand(a);
	} },
	
	// Echo, Print
	{ "print" , [](PSVM& vm) {
			//std::cout << "PRINT OPERATOR" << std::endl;
			auto a = vm.popOperand();
			a->printValue(std::cout);
	} },

	// Print out the contents of the current operand stack
	// without disturbing stack contents
	{ "stack", [](PSVM& vm) {
		for (auto& it : vm.operandStack().fContainer) {
			it->printValue(std::cout);
		}
	} },
	
	// Print detail info on items
	{ "pstack", [](PSVM& vm) {
		for (auto& it : vm.operandStack().fContainer) {
			it->printValue(std::cout) << std::endl;
		}
	} },
	
	//
	// Resource Operators
	//

	//
	// Virtual Memory Operators
	//

	//
	// Miscellaneous Operators
	//

	// bind
	{ "bind", [](PSVM& vm) {
		std::cout << "NYI: bind" << std::endl;
		//vm.bind();
	} },

	// null
	{ "null", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>(PSTokenType::nil));
	} },

	// version
	{ "version", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>(std::string("3.0"), PSTokenType::LITERAL_STRING));
	} },

	// realtime
	// usertime

	// languagelevel
	// product

	// revision
	{ "revision", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>((double)1.0));
	} },
	
	// serialnumber
	{ "serialnumber", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>((double)1.0));
	} },

	// Executive
	


	{ "=" , [](PSVM& vm) {
		auto a = vm.popOperand();
		a->printValue(std::cout) << std::endl;
	} },

	{ "==" , [](PSVM& vm) {
		auto a = vm.popOperand();
		a->printFullValue(std::cout) << std::endl;
	}},

	// save
	{ "save" , [](PSVM& vm) {
		//vm.saveState();
		std::cout << "NYI: save" << std::endl;
	} },

	// restore
	{ "restore" , [](PSVM& vm) {
		//vm.restoreState();
		std::cout << "NYI: restore" << std::endl;
	} },
};
