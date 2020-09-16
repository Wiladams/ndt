/*
	Test out the standard joystick messaging
	this is limited by the Windows messages that
	target joysticks.  This does not use DirectInput,
	which would give a wider variety of inputs.
*/
#include "p5.hpp"
#include "graphic.hpp"


using namespace p5;

int joyX;
int joyY;


void printJoystick(const Joystick& j)
{
	printf("Joystick: %s\n", j.name());
	printf("     regkey: %s\n", j.regKey());
	printf("        VxD: %s\n", j.oemVxD());
	//printf("MID: 0x%x", j.MID());
	printf("      valid: %d\n", j.isValid());
	printf("    buttons: %zd\n", j.numButtons());
	printf("       axes: %zd\n", j.numAxes());
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

void joyPressed(const JoystickEvent& e)
{
	printf("joyPressed: 0x%x  %3.2f, %3.2f\n", e.buttons, e.x, e.y);
}

void joyReleased(const JoystickEvent& e)
{
	printf("joyReleased: 0x%x  %3.2f, %3.2f\n", e.buttons, e.x, e.y);
}

void joyMoved(const JoystickEvent& e)
{
	printf("joyMoved: (%d) %3.2f, %3.2f\n", e.ID, e.x, e.y);
	joyX = (int)map(e.x, -1, 1, 0, width - 1);
	joyY = (int)map(e.y, -1, 1, height-1, 0);
}

void joyMovedZ(const JoystickEvent& e)
{
	printf("joyMovedZ: %3.2f\n", e.z);
}

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void setup()
{
	createCanvas(400, 400);

	joystick();		// turn on joystick messages

	frameRate(30);
}

void draw()
{
	background(0xc0);
	
	stroke(0);
	fill(255);
	circle(joyX, joyY, 20);
}