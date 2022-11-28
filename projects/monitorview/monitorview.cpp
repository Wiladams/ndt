//
// monitorview
// 
// Turn each monitor into a moveable graphic window
//
#include "studio.hpp"

#include "elements/displayview.hpp"

#include <memory>




std::vector<DisplayMonitor> mons{};


struct GraphicMover
{
	// Window movement
	maths::vec2f fLastMouse{};
	bool fIsMoving = false;

	GraphicMover()
	{
		;
	}

	void operator()(GraphicElement &g, const MouseEvent& e)
	{
		// First check to see if we're moving our window
// around, from dragging in the titleBar area
		switch (e.activity)
		{
		case MOUSEPRESSED:
			fIsMoving = true;
			fLastMouse = { e.x, e.y };

			return;

			break;

		case MOUSERELEASED:
			if (fIsMoving)
				fIsMoving = false;
			break;

		case MOUSEMOVED:
			if (fIsMoving) {
				// move
				auto dx = (e.x - fLastMouse.x);
				auto dy = (e.y - fLastMouse.y);
				//printf("GWindow.mouseEvent(moved): %3.2f %3.2f\n", dx, dy);

				g.moveBy(dx, dy);

				fLastMouse = { e.x, e.y };
				return;
			}
			break;
		}


	}
};


constexpr float desiredWidth = 640;		// default width we want windows to be
										// their height is calculated to maintin aspect ratio
GraphicMover mover;

void setup()
{
	// Get list of all connected monitors
	// 
	auto bbox = DisplayMonitor::monitors(mons);

	// Construct the individual snapshots
	float xoffset = 100;
	float yoffset = 100;
	for (auto& mon : mons)
	{
		auto sz = maths::size(mon.frame());

		float xScale = desiredWidth / sz.x;
		float desiredHeight = sz.y * xScale;

		auto snap = std::make_shared<DisplayView>(mon, maths::bbox2f{ {xoffset,yoffset},{xoffset+desiredWidth,xoffset+desiredHeight} });
		snap->setMouseDispatch(mover);

		addGraphic(snap);

		xoffset += 200;
		yoffset += 200;
	}
}