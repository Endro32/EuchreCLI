/*
 * Deck.h
 *
 *  Created on: Oct 22, 2019
 *      Author: webk7
 */

#ifndef DECK_H_
#define DECK_H_

#include <string>
#include <vector>

class Card {
public:
	static const int SPADES = 0; // Integer values for the 4 different suits
	static const int HEARTS = 1;
	static const int CLUBS = 2;
	static const int DIAMONDS = 3;

	static const std::string SPADESYM;
	static const std::string HEARTSYM;
	static const std::string DIAMONDSYM;
	static const std::string CLUBSYM;

	static const int ACE = 1; // Integer representation of the non-numeric cards
	static const int JACK = 11;
	static const int QUEEN = 12;
	static const int KING = 13;

	Card(int suit, int rank); // Constructor
	virtual ~Card(); // Destructor

	int getSuit();
	static std::string getSuitSymbol(int suit);
	std::string getSuitSymbol();
	static std::string getSuitAsString(int suit);
	std::string getSuitAsString();
	int getRank();
	std::string getRankAsString();
	int getStrength(int trump, int currentSuit);
private:
	const int suit;
	const int rank;
};

class Deck {
public:
	Deck();
	virtual ~Deck();

	void shuffle();
	Card *dealCard();		// Returns the top card on the deck and removes it
	Card *flipTop();		// Returns the top card on the deck, but doesn't deal it
	void topToBottom();		// Swap top and bottom cards of deck
	void put_back(Card*);
private:
	int cardsUsed;
	std::vector<Card*> cards;
};

#endif /* DECK_H_ */
