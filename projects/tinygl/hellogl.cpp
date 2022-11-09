#include "apphost.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "tinygl/GL/gl.h"
#include "tinygl/zbuffer.h"
//#include "3dMath.h"
#include "maths.hpp"

#include <stopwatch.hpp>

using namespace maths;

StopWatch wallclock{};


int winSizeX = 0;
int winSizeY = 0;
int frameCount = 0;



void drawGL2() {
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x = 0;
	// Select and setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mat4f m = perspective_mat(65.0f, (GLfloat)canvasWidth / (GLfloat)canvasHeight, 1.0f, 100.0f);
	glLoadMatrixf(begin(m));

	//	inline mat4 perspective(f_ fov, f_ aspect, f_ nearest, f_ farthest)
	//mat4 m = perspective(65.0f, (GLfloat)canvasWidth / (GLfloat)canvasHeight, 1.0f, 100.0f);
	//glLoadMatrixf(m.d);
	// OR...
	// GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
	// glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
	
	// Select and setup the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 0, -1.0f);

	// Draw a rotating colorful triangle
	glTranslatef(0.0f, 14.0f, 0.0f);
	glRotatef(0.3f * (GLfloat)x + (GLfloat)wallclock.seconds() * 100.0f, 0.0f, 0.0f, 1.0f);
	
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 0.01f, 0.01f, 0.0f);
	glVertex3f(-5.0f, 0.0f, -4.0f);
	glColor4f(0.01f, 1.0f, 0.01f, 0.0f);
	glVertex3f(5.0f, 0.0f, -4.0f);
	glColor4f(0.01f, 0.01f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 6.0f);
	glEnd();
}

void onLoop()
{
	frameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawGL2();

	glDrawText((unsigned char*)"Hello World!\nFrom TinyGL", 0, 0, 0x00FFFFFF);

	screenRefresh();
}

void initScene() 
{
	// glLightfv( GL_LIGHT0, GL_AMBIENT, white);
	// glLightfv( GL_LIGHT0, GL_SPECULAR, white);
	// glEnable( GL_CULL_FACE );
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// glEnable( GL_LIGHT0 );
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glTextSize(GL_TEXT_SIZE24x24);

	glEnable(GL_NORMALIZE);
}

void onLoad()
{
	createAppWindow(800, 600, "tinygl - hellogl");
	//layered();
	//frameRate(30);
	//recorder = std::make_shared<Recorder>(gAppSurface, "hellogl");

	winSizeX = canvasWidth;
	winSizeY = canvasHeight;

	ZBuffer* frameBuffer = NULL;
	frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_RGBA, canvasPixelData);


	glInit(frameBuffer);

	// initialize GL:
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, winSizeX, winSizeY);
	glEnable(GL_DEPTH_TEST);
	// GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// glTranslatef( 0.0, 0.0, -45.0 );

	initScene();
}