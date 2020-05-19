#include "p5.hpp"
#include "joystick.h"

using namespace p5;

Joystick joy1(0);

void printJoystick(const Joystick& j)
{
	printf("Joystick: %s\n", j.name());
	printf("     regkey: %s\n", j.regKey());
	printf("        VxD: %s\n", j.oemVxD());
	//printf("MID: 0x%x", j.MID());
	printf("      valid: %d\n", j.isValid());
	printf("    buttons: %d\n", j.numButtons());
	printf("       axes: %d\n", j.numAxes());
	printf("      has Z: %d\n", j.hasZ());
	printf("    has POV: %d\n", j.hasPOV());
	printf("  has 4 DIR: %d\n", j.hasPOV4DIR());
	printf("has POV CTS: %d\n", j.hasPOVCTS());
}

void printJoystickPosition(const JoystickEvent& e)
{
	printf("== POS ==\n");
	printf("  xyz: %3.2f, %3.2f, %3.2f\n", e.x, e.y, e.z);
	printf("  ruv: %3.2f, %3.2f, %3.2f\n", e.r, e.u, e.v);
	printf("  POV: %d\n", e.POV);
	printf("Button: %d\n", e.numButtonsPressed);
	printf("State: 0x%x\n", e.buttons);
}

void setup()
{
	createCanvas(600, 600);
	
	printJoystick(joy1);

	frameRate(4);
}

void draw()
{
	background(0xc0);


	if (!joy1.isValid())
		return;

	// get the joystick status/position
	JoystickEvent je;
	joy1.getPosition(je);

	printJoystickPosition(je);
}