#pragma once

#include "psvm.h"
#include "maths.hpp"
#include "p5.hpp"

#include <cmath>
#include <unordered_map>
#include <random>
#include <iostream>

/*
newpath
100 200 moveto
200 250 lineto
100 300 lineto
2 setlinewidth
stroke
*/

// Setup a table to contain the base operators
// These should be copied into a dictionary
// as OPERATOR tokens
std::unordered_map < std::string, PS_Operator > PSBaseOperators
{
	// Stack management
	{"newpath", [](PSVM& vm) {vm.operandStack().clear(); }},
	{"moveto", [](PSVM& vm) {vm.operandStack().clear(); }},
	{"lineto", [](PSVM& vm) {vm.operandStack().clear(); }},
	{"setlinewidth", [](PSVM& vm) {vm.operandStack().clear(); }},
	{"stroke", [](PSVM& vm) {vm.operandStack().clear(); }},
};
