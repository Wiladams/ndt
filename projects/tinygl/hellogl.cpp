#include "apphost.h"
//#include "framestats.h"
//#include "Recorder.hpp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "tinygl/GL/gl.h"
#include "tinygl/zbuffer.h"
#include "3dMath.h"
#include <stopwatch.hpp>


StopWatch wallclock{};
//using namespace p5;


//FrameStats _stats;
//std::shared_ptr<Recorder> recorder;

int winSizeX = 0;
int winSizeY = 0;
int frameCount = 0;

void drawGL() {
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_TEXTURE_2D);
	// glBindTexture(GL_TEXTURE_2D,tex);
	// time_passed += 0.0166666;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(frameCount, 0, 0, 1);

	glBegin(GL_TRIANGLES);
	glColor3f(0.2, 0.2, 1.0); // BLUE!
	// glColor3f(1.0, 0.2, 0.2); //RED!
	glVertex3f(-0.8, -0.8, 0.2);

	glColor3f(0.2, 1.0, 0.2); // GREEN!
	// glColor3f(1.0, 0.2, 0.2); //RED!
	glVertex3f(0.8, -0.8, 0.2);

	glColor3f(1.0, 0.2, 0.2); // RED!
	glVertex3f(0, 1.2, 0.2);
	glEnd();
	glPopMatrix();
}

void drawGL2() {
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x = 0;
	// Select and setup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mat4 m = perspective(65.0f, (GLfloat)canvasWidth / (GLfloat)canvasHeight, 1.0f, 100.0f);
	glLoadMatrixf(m.d);
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

	//drawGL();
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