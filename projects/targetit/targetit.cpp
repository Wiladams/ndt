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
static const int MaxBullets = 1000;
int gBulletsRemaining;
int MaxTargets = 20;
double MaxTrace;
std::vector<GTarget> targets;
std::queue<Bullet> bullets;

bool gRapidFire = false;
GScope gScope;
BLImage backgroundImage;
BLImage foregroundSurface;
ScreenSnapshot *snappy=nullptr;

double Distance(const vec2& v1, const vec2& v2)
{
	auto diff = v2 - v1;
	return diff.length();
	//return sqrt((diff.x * diff.x) + (diff.y * diff.y));
}

void resetBoard()
{
	gBulletsRemaining = MaxBullets;

	// Read the background image
	//backgroundImage.readFromFile("motherboard.jpg");

	// Read the screen
	//snappy = new ScreenSnapshot(0, 0, width, height);
	//snappy->moveNext();
	//snappy->getCurrent().rectMode(RECTMODE::CENTER);
	//snappy->getCurrent().noStroke();
	//snappy->getCurrent().fill(color(0, 0, 0, 0));
	//snappy->getCurrent().blendMode(BL_COMP_OP_SRC_COPY);

}

void keyReleased(const KeyboardEvent&e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void joyMoved(const JoystickEvent& e)
{
	joyX = map(e.x, -1, 1, 0, width - 1, true);
	joyY = map(e.y, -1, 1, height  -1, 0, true);
	gScope.focusOn(joyX, joyY);
}

void triggerPressed()
{
	if (gBulletsRemaining < 1)
		return;

	double len = Distance({ (float)joyX, (float)joyY }, { (float)width / 2, (float)height - 1 });
	// duration anywhere from a minimum of 1/8 second
	// to 1/4 second depending on distance of target from origin
	float duration = map(len, 0, MaxTrace, 0.125, 0.25);
	Bullet b1({ (float)width / 2,(float)height - 1 }, { (float)joyX, (float)joyY }, duration);
	b1.play();
	bullets.push(b1);

	gBulletsRemaining = gBulletsRemaining - 1;
}

void joyPressed(const JoystickEvent& e)
{
	//printf("joyPressed: 0x%x\n", e.buttons);


	if (e.buttons & JOY_BUTTON1) {
		// If the main trigger has been pulled
		// then create a bullet and start its
		// animation
		triggerPressed();
	} else if (e.buttons & JOY_BUTTON2) {
		// switch between rapid fire and single shot
		gRapidFire = !gRapidFire;
	}
	else if (e.buttons & JOY_BUTTON4) {
		// Reset the board
		resetBoard();
	}
}

void drawBullets(std::shared_ptr<IGraphics> ctx)
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
			// use rect, because we can leverage the RECTMODE
			//snappy->getCurrent().clearRect(b.fDestination.x, b.fDestination.y, cellHeight, cellHeight);
			snappy->getCurrent().rect(b.fDestination.x, b.fDestination.y, cellHeight, cellHeight);
		}
	}
}

void drawTargets(std::shared_ptr<IGraphics> ctx) 
{
	for (size_t i = 0; i < targets.size(); i++)
		targets[i].draw(ctx);
}

void drawScore(std::shared_ptr<IGraphics> ctx)
{
	// draw rounded rect in top left corner
	ctx->stroke(0xC0);
	ctx->fill(color(60, 60, 60, 220));
	ctx->rect(8, 8, 300, 64, 6, 6);

	ctx->noStroke();
	ctx->fill(color(0, 220, 15));
	ctx->textSize(36);
	ctx->text("Bullets: ", 12, 58);
	ctx->fill(color(220, 15, 15));

	char buff[32];
	sprintf_s(buff, 32, "%4d", gBulletsRemaining);

	ctx->text(buff, 220, 58);
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
			auto img = snappy->getCurrent().getImage();
			image(img, 0, 0);
		}
	} else {
		background(0xc0);
	}

	if (gRapidFire) {
		JoystickEvent e;
		joy1.getPosition(e);
		if (e.buttons & JOY_BUTTON1 > 0) {
			triggerPressed();
		}
	}

	//drawTargets(gAppSurface);
	drawBullets(gAppSurface);
	
	gScope.draw(gAppSurface);

	drawScore(gAppSurface);
}



void setup()
{
	//createCanvas(600, 600);
	createCanvas(displayWidth, displayHeight);
	layered();
	setCanvasPosition(0, 0);

	// Read the background image
	backgroundImage.readFromFile("motherboard.jpg");

	resetBoard();
	
	vec2f lowerCorner(0, height - 1);
	vec2f upperCorner(width - 1, 0);
	MaxTrace = Distance(lowerCorner, upperCorner);

	frameRate(60);

	joystick();
	joyX = width / 2;
	joyY = height-1;


	for (size_t i = 1; i <= MaxTargets; i++)
		targets.push_back(GTarget(random(0, (double)width - 1), random(0, (double)height - 1), random(40, 90)));

	// setup bullets
	gBulletsRemaining = MaxBullets;

	// Read the screen
	snappy = new ScreenSnapshot(0, 0, width, height,0);
	snappy->next();
	auto srf = snappy->getCurrent();
	srf.rectMode(RECTMODE::CENTER);
	srf.noStroke();
	srf.fill(color(0, 0, 0, 0));
	srf.blendMode(BL_COMP_OP_SRC_COPY);
}
