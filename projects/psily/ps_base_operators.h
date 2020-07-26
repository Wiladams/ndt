#pragma once

#include "psvm.h"

#include <unordered_map>

// Setup a dispatch table for base operators
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

	// Echo, Print
	{ "print" , [](PSVM& vm) {
		//std::cout << "PRINT OPERATOR" << std::endl;
		auto a = vm.operandStack().pop();
		std::cout << *a << std::endl;
	}},

	{ "==" , [](PSVM& vm) {
		//std::cout << "== OPERATOR" << std::endl;
		auto a = vm.operandStack().pop();
		std::cout << *a << std::endl;
	}},
};
