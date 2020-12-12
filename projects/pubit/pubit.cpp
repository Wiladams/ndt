/*
    Subscribe to various system level topics
	print out simple messages when data available
*/

#include "apphost.h"


void keyboardSubscriber(const KeyboardEventTopic& p, const KeyboardEvent& e)
{
	std::cout << "keyboardSubscriber: " << e.activity << ", " << e.keyCode << std::endl;
}

void mouseSubscriber(const MouseEventTopic &p, const MouseEvent& e)
{
	std::cout << "mouseSubscriber: " << e.x << ", " << e.y << std::endl;
}

void joystickSubscriber(const JoystickEventTopic& p, const JoystickEvent& e)
{
	std::cout << "joystickSubscriber: " << e.activity << ", " << e.z << std::endl;
}

void onLoad()
{
	joystick();		// turn on joystick messages

	subscribe(mouseSubscriber);
	subscribe(keyboardSubscriber);
	subscribe(joystickSubscriber);
}