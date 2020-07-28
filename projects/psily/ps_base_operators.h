#pragma once

#include "psvm.h"
#include "maths.hpp"

#include <cmath>
#include <unordered_map>
#include <random>

// Setup a dispatch table for base operators
// These will ultimately be copied into a dictionary
std::unordered_map < std::string, std::function<void(PSVM& vm)> > PSOperators
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
		vm.pushOperand(tok);
	}},

	{"counttomark", [](PSVM& vm) {
		auto n = vm.operandStack().countToMark();
		auto tok = make_shared<PSToken>((double)n);

		vm.pushOperand(tok);
	}},

	{"dup", [](PSVM& vm) {vm.operandStack().dup(); }},

	{"exch", [](PSVM& vm) {vm.operandStack().exch(); }},

		// BUGBUG - use double
	{"index", [](PSVM& vm) {
		auto n = vm.operandStack().pop();
		auto tok = vm.operandStack().nth((size_t)n->fData.asReal);
		vm.pushOperand(tok);
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

			vm.pushOperand(tok);
	}},

	// atan
	{ "atan", [](PSVM& vm) {
		auto den = vm.popOperand()->fData.asReal;
		auto num = vm.popOperand()->fData.asReal;
		auto value = maths::Degrees(std::atan(num / den));
		vm.pushOperand(make_shared<PSToken>(value));
	} },
	
	// sub
	// subtraction
	{"sub", [](PSVM& vm) {
			auto num2 = vm.popOperand();
			auto num1 = vm.popOperand();
			auto tok = make_shared<PSToken>((double)num1->fData.asReal - num2->fData.asReal);

			vm.pushOperand(tok);
	}},

	// mul
	// multiplication
	// pop two, push one
	{"mul", [](PSVM& vm) {
			auto num2 = vm.popOperand();
			auto num1 = vm.popOperand();
			auto tok = make_shared<PSToken>((double)num1->fData.asReal * num2->fData.asReal);

			vm.pushOperand(tok);
	}},


	{"div", [](PSVM& vm) {
			auto num2 = vm.popOperand();
			auto num1 = vm.popOperand();
			auto tok = make_shared<PSToken>((double)num1->fData.asReal / num2->fData.asReal);

			vm.pushOperand(tok);
	}},
	
	// exp
	{"exp", [](PSVM& vm) {
			auto base = vm.popOperand();
			auto exponent = vm.popOperand();
			auto tok = make_shared<PSToken>(std::pow(base->fData.asReal ,exponent->fData.asReal));

			vm.pushOperand(tok);
	}},

	{"idiv", [](PSVM& vm) {
			auto b = vm.popOperand();
			auto a = vm.popOperand();
			auto q = (double)a->fData.asReal / b->fData.asReal;
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
			auto b = vm.popOperand();
			auto a = vm.popOperand();
			auto value = fmod(a->fData.asReal, b->fData.asReal);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	{ "maximum", [](PSVM& vm) {
			auto b = vm.popOperand();
			auto a = vm.popOperand();
			auto value = std::max(a->fData.asReal, b->fData.asReal);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	{ "minimum", [](PSVM& vm) {
			auto b = vm.popOperand();
			auto a = vm.popOperand();
			auto value = std::min(a->fData.asReal, b->fData.asReal);

			auto tok = make_shared<PSToken>(value);

			vm.pushOperand(tok);
	}},

	// Single argument arithmetic operators
	// abs
	{ "abs", [](PSVM& vm) {
		auto a = vm.popOperand();
		auto tok = make_shared<PSToken>(std::abs(a->fData.asReal));

		vm.pushOperand(tok);

	} },

	// ceiling
	{ "ceiling", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::ceil(a->fData.asReal)));
	} },

	// floor
	{ "floor", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::floor(a->fData.asReal)));
	} },

	// neg
	{ "neg", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(-(a->fData.asReal)));
	} },
	
	// round
	{ "round", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::round(a->fData.asReal)));
	} },
	
	// truncate
	{ "truncate", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::trunc(a->fData.asReal)));
	} },

	// sqrt
	{ "sqrt", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::sqrt(a->fData.asReal)));
	} },


	
	// ln
	{ "ln", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::log(a->fData.asReal)));
	} },
	
	// log
	{ "log", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::log10(a->fData.asReal)));
	} },

	// sin
	{ "sin", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::sin(a->fData.asReal)));
	} },
	
	// cos
	{ "cos", [](PSVM& vm) {
		auto a = vm.popOperand();
		vm.pushOperand(make_shared<PSToken>(std::sin(a->fData.asReal)));
	} },
	


	// rand
	{ "rand", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>((double)vm.randomInt()));
	} },
	
	// srand
	{ "srand", [](PSVM& vm) {
		auto seed = vm.popOperand();
		vm.seedRandomInt((unsigned int)seed->fData.asReal);
	} },

	// rrand

	//
	// Array, Packed Array, Dictionary, and string operators
	//
	// get
	{ "get", [](PSVM& vm) {
		auto idx = (size_t)vm.popOperand()->fData.asReal;
		auto arr = *vm.popOperand()->fData.asArray;
		auto value = arr[idx];
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

		vm.dictionaryStack().def(*key->fData.asString, value);
	} },
	
	// load
	{ "load", [](PSVM& vm) {
		auto key = vm.popOperand();
		auto value = vm.dictionaryStack().load(*key->fData.asString);

		if (nullptr == value) {
			// print undefined key
			return;
		}

		vm.pushOperand(value);
	} },

	// store
	{ "store", [](PSVM& vm) {
		auto value = vm.popOperand();
		auto key = vm.popOperand();

		vm.dictionaryStack().store(*key->fData.asString, value);
	} },

	// where
	{ "where", [](PSVM& vm) {
		auto key = vm.popOperand();
		auto d = vm.dictionaryStack().where(*key->fData.asString);

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
	{ "where", [](PSVM& vm) {
		auto arrtok = vm.popOperand();
		auto arr = *arrtok->fData.asArray;
		auto size = arr.size();

		for (size_t i = 0; i < size; i++) {
			auto value = vm.popOperand();
			arr[size - i-1] = value;
		}

		vm.pushOperand(arrtok);
	} },

	// aload
	
	// begin
	// pop a dictionary off the operand stack
	// make it the current dictionary
	// by placing it atop the dictionary stack
	{ "begin", [](PSVM& vm) {
		auto d = vm.popOperand();
		vm.dictionaryStack().pushDictionary(d->fData.asDictionary);
	} },

	// end
	{ "begin", [](PSVM& vm) {
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
	// xcheck
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
			std::cout << *a << std::endl;
	} },

	// Print out the contents of the current operand stack
	// without disturbing stack contents
	{ "stack", [](PSVM& vm) {
		for (auto& it : vm.operandStack().fContainer) {
			std::cout << *it << std::endl;
		}
	} },
	
	// Print detail info on items
	{ "pstack", [](PSVM& vm) {
		for (auto& it : vm.operandStack().fContainer) {
			std::cout << *it << std::endl;
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
		vm.pushOperand(make_shared<PSToken>((double)1));
	} },
	
	// serialnumber
	{ "serialnumber", [](PSVM& vm) {
		vm.pushOperand(make_shared<PSToken>((double)1));
	} },

	// Executive
	


	{ "=" , [](PSVM& vm) {
		auto a = vm.popOperand();
		std::cout << *a << std::endl;
	} },

	{ "==" , [](PSVM& vm) {
		auto a = vm.popOperand();
		std::cout << *a << std::endl;
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
