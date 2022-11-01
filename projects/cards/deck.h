#pragma once

#include "spritesheet.h"
#include "sampler.hpp"

#include <vector>

struct PlayingCard
{
	// symbolic names for the suits
	typedef enum {
		FirstSuit=1,
		Clubs = 1,
		Spades = 2,
		Hearts = 3,
		Diamonds = 4,
		LastSuit = 4,
	} suits;

	// symbolic names for ranks
	typedef enum {
		FirstRank = 1,
		ace = 1,
		two = 2,
		three = 3,
		four = 4,
		five = 5,
		six = 6,
		seven = 7,
		eight = 8,
		nine = 9,
		ten = 10,
		jack = 11,
		queen = 12,
		king = 13,
		LastRank = 13
	} ranks;

	int fRank{};
	int fSuit{};
	BLImage fImage{};

	PlayingCard(const int rank, const int suit)
		:fRank(rank)
		, fSuit(suit)
	{
	}
};

struct DeckOfCards
{
	static constexpr int cardWidth = 73;
	static constexpr int cardHeight = 98;

	SpriteSheet fSpriteSheet{};
	std::vector<PlayingCard> fCards{};
	bool isValid=false;
	maths::rng_state RNG;

	DeckOfCards()
	{
		RNG = maths::make_rng(52);

		// Create a sprite sheet
		if (!fSpriteSheet.loadImage("cards.png"))
			printf("Error loading sprite sheet\n");

		isValid = true;

		// Create a vector of cards
		createCards();
	}

	void createCards()
	{
		for (int suit = PlayingCard::FirstSuit; suit <= PlayingCard::LastSuit; suit++)
		{
			for (int rank = PlayingCard::FirstRank; rank <= PlayingCard::LastRank; rank++)
			{
				unsigned int x = (rank - 1) * cardWidth;
				unsigned int y = (suit - 1) * cardHeight;

				fCards.push_back(PlayingCard(rank, suit));
				fSpriteSheet.getSprite(x, y, cardWidth, cardHeight, fCards[fCards.size()-1].fImage);
			}
		}
	}

	// Shuffle the deck of cards
	void shuffle()
	{
		maths::shuffle(fCards, RNG);
	}

};

