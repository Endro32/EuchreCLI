/*
 * Deck.cc
 *
 *  Created on: Oct 22, 2019
 *      Author: webk7
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include "Deck.h"

// Card Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
 * int suit
 * int rank
 */

const std::string Card::SPADESYM = "♠";
const std::string Card::HEARTSYM = "♥";
const std::string Card::DIAMONDSYM = "♦";
const std::string Card::CLUBSYM = "♣";

Card::Card(int s, int r) : suit(s), rank(r) { // Constructor
	if (suit < 0 || suit > 3 || (rank != 1 && rank < 9) || rank > 13) {
		std::cout << "An internal error has occurred";
		// TODO throw an exception
	}
}

Card::~Card() {  // Destructor
}

/**
 * Returns the integer value for the suit of this card
 */
int Card::getSuit() {
	return suit;
}

/**
 * Returns the symbol for the given suit as a string
 */
std::string Card::getSuitSymbol(int s) {
	switch (s) {
	case Card::SPADES:		return Card::SPADESYM;
	case Card::HEARTS:		return Card::HEARTSYM;
	case Card::DIAMONDS:	return Card::DIAMONDSYM;
	case Card::CLUBS:		return Card::CLUBSYM;
	default:				return "";
	}
}

/**
 * Returns the symbol for the suit of this card
 */
std::string Card::getSuitSymbol() {
	return getSuitSymbol(suit);
}

/**
 * Returns the name of a suit as a string
 */
std::string Card::getSuitAsString(int s) {
	switch (s) {
	case Card::SPADES:		return "Spades";
	case Card::HEARTS:		return "Hearts";
	case Card::DIAMONDS:	return "Diamonds";
	case Card::CLUBS:		return "Clubs";
	default:				return "Fail";
	}
}

/**
 * Returns the name of this card's suit as a string
 */
std::string Card::getSuitAsString() {
	return getSuitAsString(suit);
}

int Card::getRank() {
	return rank;
}

/**
 * Returns the name of this card's rank as a string
 */
std::string Card::getRankAsString() {
	switch (rank) {
	case Card::ACE:			return "Ace";
	case 9:					return "9";
	case 10:				return "10";
	case Card::JACK:		return "Jack";
	case Card::QUEEN:		return "Queen";
	case Card::KING:		return "King";
	default:				return "Fail";
	}
}

/**
 * Returns a strength value between 0 and 13 for this card
 * 1-6 denotes the rankings of the currently played suit
 * 7-13 denotes the rankings of the trump cards
 * 0 denotes a card from a suit other than the currently played suit or trump
 */
int Card::getStrength(int trump, int curSuit) {
	if (suit == trump) { 		// Return values for cards of trump suit
		switch (rank) {
		case 9:				return 7;
		case 10:			return 8;
		case Card::QUEEN:	return 9;
		case Card::KING:	return 10;
		case Card::ACE:		return 11;
		case Card::JACK:	return 13;
		}
	}

	int leftBower = -1;				// Determine which suite the left bower is in
	switch (trump) {
	case SPADES:
		leftBower = CLUBS;
		break;
	case HEARTS:
		leftBower = DIAMONDS;
		break;
	case CLUBS:
		leftBower = SPADES;
		break;
	case DIAMONDS:
		leftBower = HEARTS;
		break;
	}							// Now check if this card is the left bower
	if (suit == leftBower && rank == Card::JACK)
		return 12;				// Return value for the left bower

	if (suit == curSuit) {		// If card is from the current suit
		if (rank == Card::ACE)	// Return value for the ace of current suit
			return 6;
		else					// Return value for all other cards of current suit
			return rank - 8;
	}
	else {						// Return value for all other cards
		return 0;
	}
}

int Card::getStrength(int trump) {
	if (suit == trump) { 		// Return values for cards of trump suit
		switch (rank) {
		case 9:				return 7;
		case 10:			return 8;
		case Card::QUEEN:	return 9;
		case Card::KING:	return 10;
		case Card::ACE:		return 11;
		case Card::JACK:	return 13;
		}
	}

	int leftBower = -1;				// Determine which suite the left bower is in
	switch (trump) {
	case SPADES:
		leftBower = CLUBS;
		break;
	case HEARTS:
		leftBower = DIAMONDS;
		break;
	case CLUBS:
		leftBower = SPADES;
		break;
	case DIAMONDS:
		leftBower = HEARTS;
		break;
	}							// Now check if this card is the left bower
	if (suit == leftBower && rank == Card::JACK)
		return 12;				// Return value for the left bower

	if (rank == Card::ACE)		// Return value for the ace of current suit
		return 6;
	else						// Return value for all other cards of current suit
		return rank - 8;
}

// Deck Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Creates a deck of all the Euchre cards
 */
Deck::Deck() {
	for (int s = 0; s <= 3; s++) {				// For each suit
		cards.push_back(new Card(s, 1));			// Create ace
		for (int r = 9; r <= 13; r++) {			// For each other card rank
			cards.push_back(new Card(s, r));		// Create card
		}
	}

	cardsUsed = 0;
}

/**
 * Frees the memory of each card object stored in the array
 */
Deck::~Deck() {
	int i = 0;									// Array index
	for (int s = 0; s <= 3; s++) {				// For each suit
		delete cards[i++];						// Delete ace
		for (int r = 9; r <= 13; r++) {			// For each other card rank
			delete cards[i++];					// Delete card
		}
	}
}

void Deck::shuffle() {
	srand((unsigned) time(0)); 					// Set the random seed
	for (int i = 0; i < 24; i++) {				// For each card in the deck
		int random = (rand() % 24);				// Get next random number
		Card *temp = cards[i];
		cards[i] = cards[random];
		cards[random] = temp;
	}

	cardsUsed = 0;								// Deck is now full again
}

Card *Deck::dealCard() {
	if (cardsUsed >= 24)						// If all the cards have been used
		throw std::logic_error("No more cards left in the deck!");

	return cards[cardsUsed++];					// Return the next card in the deck and increment cardsUsed
}

Card *Deck::flipTop() {
	if (cardsUsed >= 24)						// If all the cards have been used
		throw std::logic_error("No more cards left in the deck!");

	return cards[cardsUsed];					// Return the next card in the deck and increment cardsUsed
}

void Deck::topToBottom() {
	std::swap(cards[cardsUsed], cards[0]);
}

void Deck::put_back(Card *card) {

}
