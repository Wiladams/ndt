#include "p5.hpp"
using namespace p5;

void setup()
{
	createCanvas(800, 600, "sinwave");
}

typedef float (*CFUNC)(float u);

void draw()
{
	background(225);


	// 0 - 2PI
	double minx = 0;
	double maxx = 720;
	double maxy = 200;
    double lastx = minx;
	double frequency = 3;

	// Cosine curve
	double lastcosy = map(cos(0), -1, 1, maxy - 1, 0);
	for (int x = (int)lastx+1; x <= maxx; x++) {
		double angx = map(x, minx, maxx, 0, frequency*2 * maths::Pi);

		double cosx = cos(angx);
		double cosy = map(cosx, -1, 1, (double)maxy - 1, 0);
		stroke(0, 0, 255);
		line(lastx, lastcosy, x, cosy);
		lastcosy = cosy;

		lastx = x;
	}

	// midway line
	stroke(0, 255, 0);
	line(minx, maxy / 2, (double)maxx - 1, maxy / 2);
	
	// Sine curve
	lastx = 0;
	double lastsiny = map(sin(0), -1, 1, (double)maxy - 1, 0);
	
	for (int x = (int)lastx+1; x < (int)maxx - 1; x++) {
		double angx = map(x, 0, (double)maxx - 1, 0, frequency * 2 * maths::Pi2);

		double sinx = sin(angx-(maths::PiOver2));	// shift phase to be opposite with cosx
		double siny = map(sinx, -1, 1, (double)maxy - 1, 0);
		stroke(255, 0, 0);
		line(lastx, lastsiny, x, siny);
		lastsiny = siny;

		lastx = x;
	}

	flush();
}