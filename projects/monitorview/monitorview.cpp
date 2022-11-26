//
// monitorview
// 
// Target specific monitor to take a screen snapshot
//
#include "studio.hpp"
#include "elements/screensnapshot.hpp"
#include "elements/gmonitor.h"

#include <memory>

ScreenSnapper snapper;

struct MonitorView : public GraphicElement
{
	DisplayMonitor & fMonitor;
	ScreenSnapper fSnapper;

	MonitorView(DisplayMonitor& mon, maths::bbox2f aframe)
		:GraphicElement(aframe)
		,fMonitor {mon	}
	{
		fSnapper.reset(mon.frame(), mon.getDC());
		setBounds(maths::bbox2f{ {0,0},{frameWidth(),frameHeight()} });
	}

	void draw(IGraphics& ctx) override
	{
		fSnapper.next();
		ctx.scaleImage(fSnapper.getImage(), 0, 0, fSnapper.width(), fSnapper.height(), 0, 0, frameWidth(), frameHeight());
	}
};



void setup()
{
	//createCanvas(800, 600, "monitorview", 4);

	// Get monitor we want to capture
	// 
	std::vector<DisplayMonitor> mons{};
	auto bbox = DisplayMonitor::monitors(mons);

	printf("VBox: %3.0f,%3.0f  %3.0f,%3.0f\n", bbox.min.x, bbox.min.y, bbox.max.x, bbox.max.y);

	// Setup the screen snapshot
	int numMons = mons.size();
	auto& myMon = mons[1];


}