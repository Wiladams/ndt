#include "p5.hpp"
#include <cstdio>

void printColor(const Color& c, const char* name = "") {
	printf("%s[%d, %d, %d, %d]\n", name, c.r, c.g, c.b, c.a);
}

template <typename T>
T ** Array2D(int rows, int columns)
{
	T** arr = new T* [rows];
	for (int i = 0; i < rows; i++) {
		arr[i] = new T[columns];
	}

	return arr;
}

const int gWidth = 600;
const int gHeight = 400;

double **Temperature;

void zeroTemperature()
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Temperature[y][x] = 0;
		}
	}
}

void buildImage()
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			double t = map(Temperature[y][x], 0, 1.5, 0, 255);
			set(x, y, color(t, t, 0));
		}
	}
}

class Blob
{
	double cx, cy;
	double r;

public:
	Blob(double x, double y, double radius)
	{
		cx = x;
		cy = y;
		r = radius;
	}

	void draw()
	{
		for (double y = cy - r; y < cy + r; y++) {
			for (double x = cx - r; x < cx + r; x++) {
				double d = dist(x, y, cx, cy);
				if (d > r) 
					continue;
				
				double h = map(d, 0, r, 1, 0);
				Temperature[(int)y][(int)x] += h;
			}
		}
	}
};


void setup()
{
	createCanvas(gWidth, gHeight);

	Temperature = Array2D<double>(height, width);

	Blob* blob0 = new Blob(300, 200, 100);
	Blob* blob1 = new Blob(400, 230, 75);
	
	zeroTemperature();
	blob0->draw();
	blob1->draw();

	loadPixels();
	buildImage();
	updatePixels();

	noLoop();
}