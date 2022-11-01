#include "p5.hpp"
#include "deck.h"


using namespace p5;


DeckOfCards gCards;

void setup()
{
	createCanvas(960, 480, "cards");
}


void draw()
{
	background(Pixel(127, 255, 127, 255));

	int row = 0;
	int col = 0;

	int rowHeight = 104;
	int xincrement = 16;
	int yincrement = 6;

	int cardNum = 1;

	for (auto& card : gCards.fCards)
	{
		image(card.fImage, col*xincrement, (row* rowHeight)+col*yincrement);

		if (cardNum % 13 == 0)
		{
			row++;
			col = 0;
		}
		else {
			col++;
		}

		cardNum++;
	}
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;

	case 'S':
		gCards.shuffle();
		break;
	}

}

