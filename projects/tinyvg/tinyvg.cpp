#include "p5.hpp"
#include "filestream.h"
#include "tvgparse.h"

using namespace p5;
using namespace tinyvg;

// Get a stream on the intended image
FileStream fs_tiger("tiger.tvg");
FileStream fs_shield("shield.tvg");
//FileStream fs_comic("comic.tvg");
FileStream fs_chart("chart.tvg");
//FileStream fs_icon("app-icon.tvg");
//FileStream fs_flowchart("flowchart.tvg");
FileStream fs_everything("everything.tvg");

BinStream* mainStream = nullptr;
tvgparser* mainParser = nullptr;

void drawCommand(tvg_command_t& cmd)
{
	switch (cmd.command) {
		case Commands::DrawLinePath:
		case Commands::DrawLines:
		case Commands::DrawLineLoop:
		case Commands::DrawLineStrip:
		{
			noFill();
			stroke(cmd.lineStyle.style);
			strokeWeight(cmd.lineWidth);
			path(cmd.segment);
		}
		break;

		case Commands::FillPath:
		case Commands::FillPolygon:
		case Commands::FillRectangles:
		{
			noStroke();
			fill(cmd.fillStyle.style);
			path(cmd.segment);
		}

		case Commands::OutlineFillPath:
		case Commands::OutlineFillRectangles:
		case Commands::OutlineFillPolygon:
		{
			strokeWeight(cmd.lineWidth < 0.5 ? 0.5 : cmd.lineWidth);
			fill(cmd.fillStyle.style);
			stroke(cmd.lineStyle.style);

			path(cmd.segment);
		}
		break;


		default:
		{
			printf("UNKNOWN COMMAND: %d\n", cmd.command);
		}
	}
}

// Draw the entire thing in one shot
void drawStream(BinStream& bs)
{
	tvgparser p(bs);

	tvg_command_t cmd;
	while (p.next(cmd))
	{
		//printf("Another path\n");
		drawCommand(cmd);

		cmd.reset();
	}
}

// draw one command per frame
void draw()
{
	if (nullptr == mainParser)
		return;

	if (mainStream->isEOF())
	{
		noLoop();
		//mainStream->seek(0);
		//mainParser->init();
		//background(0xffdddddd);
	}

	tvg_command_t cmd;
	
	if (mainParser->next(cmd))
		drawCommand(cmd);
}

void setup()
{
	createCanvas(800, 1024, "TinyVG Viewer");
	frameRate(3);

	background(0xffdddddd);

	//mainStream = &fs_tiger;
	//mainStream = &fs_shield;
	//mainStream = &fs_chart;
	mainStream = &fs_everything;

	mainParser = new tvgparser(*mainStream);

}
