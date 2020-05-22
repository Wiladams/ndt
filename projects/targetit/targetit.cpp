#include "p5.hpp"
#include "scope.h"
#include "target.h"
#include "joystick.h"
#include "bullet.h"
#include "screensnapshot.hpp"

#include <queue>

using namespace p5;

Joystick joy1;
int joyX;
int joyY;

static const int cellHeight = 120;
int MaxTargets = 20;
double MaxTrace;
std::vector<GTarget> targets;
std::queue<Bullet> bullets;

GScope gScope;
BLImage backgroundImage;
BLImage foregroundSurface;
ScreenSnapshot *snappy=nullptr;

double Distance(const Vec2f& v1, const Vec2f& v2)
{
	Vec2f diff = v2 - v1;
	return sqrt((diff.x * diff.x) + (diff.y * diff.y));
}

void keyReleased(const KeyEvent &e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void joyMoved(const JoystickEvent& e)
{
	joyX = map(e.x, -1, 1, 0, width - 1, true);
	joyY = map(e.y, 0, 1, height - cellHeight-1, 0, true);
	gScope.focusOn(joyX, joyY);
}

void joyPressed(const JoystickEvent& e)
{
	//printf("joyPressed: %x\n", e.buttons);

	// If the main trigger has been pulled
	// then create a bullet and start its
	// animation
	if (e.buttons & JOY_BUTTON1) {
		double len = Distance({ (float)joyX, (float)joyY }, { (float)width / 2, (float)height - 1 });
		// duration anywhere from a minimum of 1/8 second
		// to 1/4 second depending on distance of target from origin
		float duration = map(len, 0, MaxTrace, 0.125, 0.25);
		Bullet b1({ (float)width / 2,(float)height - 1 }, { (float)joyX, (float)joyY }, duration);
		b1.play();
		bullets.push(b1);
	}
}

void drawBullets(IGraphics* ctx)
{
	size_t nBullets = bullets.size();
	//printf("draw, bullets: %d\n", nBullets);

	for (size_t i = 0; i < nBullets; i++) {
		Bullet b = bullets.front();
		bullets.pop();
		
		b.draw(ctx);

		if (!b.isFinished()) {
			bullets.push(b);
		}
		else
		{
			
			// puch a hole in the overlay image
			//snappy->getCurrent().blendMode(BL_COMP_OP_SRC_COPY);
			//snappy->getCurrent().fill(color(0, 10));
			//snappy->getCurrent().rect(b.fDestination.x, b.fDestination.y, 100, 100);
			snappy->getCurrent().clearRect(b.fDestination.x, b.fDestination.y, cellHeight, cellHeight);
		}
	}
}

void drawTargets(IGraphics* ctx) {
	for (size_t i = 0; i < targets.size(); i++)
		targets[i].draw(ctx);
}

void draw()
{
	if (isLayered()) 
	{
		scaleImage(backgroundImage,
			0, 0, backgroundImage.width(), backgroundImage.height(),
			0, 0, width, height);

		// draw screen image
		if (snappy != nullptr) {
			//printf("CAPTURED");
			auto img = snappy->getCurrent().getBlend2dImage();
			image(img, 0, 0);
		}
	} else {
		background(0xc0);
	}


	//drawTargets(gAppSurface);
	drawBullets(gAppSurface);
	
	gScope.draw(gAppSurface);
}

void setup()
{
	//createCanvas(600, 600);
	createCanvas(displayWidth, displayHeight);
	layered();
	setWindowPosition(0, 0);


	// Read the background image
	backgroundImage.readFromFile("motherboard.jpg");
	
	Vec2f lowerCorner(0, height - 1);
	Vec2f upperCorner(width - 1, 0);
	MaxTrace = Distance(lowerCorner, upperCorner);

	frameRate(60);

	joystick();
	joyX = width / 2;
	joyY = height-1;


	for (size_t i = 1; i <= MaxTargets; i++)
		targets.push_back(GTarget(random(0, width - 1), random(0, height - 1), random(40, 90)));

	// Read the screen
	snappy = new ScreenSnapshot(0, 0, width, height);
	snappy->moveNext();
	snappy->getCurrent().rectMode(RECTMODE::CENTER);
}
