#pragma once

#include "apphost.h"
#include "uievent.h"

/*
	Window manager

	Controls where events are sent

	Brings windows to the forefront and back depending on clicks
*/

class WindowManager
{
public:
	void handleKeyEvent(const KeyEvent& e);
	void handleMouseEvent(const MouseEvent& e);

	void keyPressed(const KeyEvent& e);
	void keyReleased(const KeyEvent& e);
	void keyTyped(const KeyEvent& e);

	void mouseClicked(const MouseEvent& e);
	void mouseDragged(const MouseEvent& e);
	void mouseMoved(const MouseEvent& e);
	void mousePressed(const MouseEvent& e);
	void mouseReleased(const MouseEvent& e);
	void mouseWheel(const MouseEvent& e);
};