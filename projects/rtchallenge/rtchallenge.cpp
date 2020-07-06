#include "p5.hpp"
using namespace p5;

#include "rttypes.h"

template <size_t DIM, typename T>
void printTuple(const RTvec<DIM,T> & a)
{
	printf("(%f, %f, %f, %f)\n", a[0], a[1], a[2], a[3]);
}

/*
void printMatrix(const RTMatrix44& m)
{
	for (int row = 0; row < 4; row++) {
		printTuple(m[row]);
	}
}
*/

void test_addition()
{
	printf("==== test_addition() ====\n");
	auto a1 = RTFloat4(3, -2, 5, 1);
	auto a2 = RTFloat4(-2, 3, 1, 0);
	auto a3 = a1 + a2;
	printTuple(a1);
	printTuple(a2);
	printTuple(a1+a2);
}

void test_subtraction()
{
	printf("==== test_subtraction() ====\n");
	auto p = RTFloat4::Point(3, 2, 1);
	auto v = RTFloat4::Vector(5, 6, 7);
	printf("---- point minus vector ---\n");
	printTuple(p);
	printTuple(v);
	printTuple(p-v);

	printf("---- vector minus vector ----\n");
	auto v1 = RTFloat4::Vector(3, 2, 1);
	auto v2 = RTFloat4::Vector(5, 6, 7);
	auto v3 = v1 - v2;
	printTuple(v1);
	printTuple(v2);
	printTuple(v3);
}

void test_negation()
{
	printf("==== test_negation ====\n");
	RTFloat4 a(1, -2, 3, -4);
	printTuple(a);
	printTuple(-a);
}

/*
void test_matrix()
{
	printf("==== test_matrix ====\n");
	printf("---- transpose ----\n");
	RTMatrix44 A = { {{0,9,3,0},{9,8,0,8},{1,8,5,3},{0,0,5,8}} };

	printMatrix(matrix_transpose(A));

	printf("---- multiply tuple ----\n");
	A = { {{1,2,3,4},{2,4,4,2},{8,6,4,1},{0,0,0,1}} };
	RTFloat4 b{ 1,2,3,1 };
	printTuple(A * b);
}
*/

void preload()
{
	printf("preload\n");

	RTFloat4 p1 = RTFloat4::Point(4.3, -4.2, 3.1);
	RTFloat4 v1 = RTFloat4::Vector(4.3, -4.2, 3.1);

	printf("p1: isPoint: %d\n", p1.isPoint());
	printf("v1: isPoint: %d\n", v1.isPoint());

	test_addition();
	test_subtraction();
	test_negation();
	//test_matrix();
}

void setup()
{

}

void draw()
{

}