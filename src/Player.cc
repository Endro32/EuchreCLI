/*
 * Player.cc
 *
 *  Created on: Oct 15, 2019
 *      Author: webk7
 */
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Player.h"

Player::Player(): human(true), tricksWon(0) {
	std::cout << "Please enter a username: ";
	std::cin >> name;

	handStrength = 0, trumpCards = 0;
}

Player::Player(std::string n): name(n), human(false), tricksWon(0) {
	std::cout << "Hello, my name is " << name << std::endl;
}

Player::~Player() { }

// Until the AI is done, all decisions will be made by the user
bool Player::promptPickUp() {
	printHand();

	std::string s;
	std::cout << "Do you want the dealer to pick it up? [Y/n]: ";
	std::cin >> s;
	return std::tolower(s[0]) == 'y';
}

// AI will decide to order it up or not
bool Player::decidePickUp(int tsuit) {
	int trumps = 0, aces = 0, hStrength = 0;
	bool leftBower = false, rightBower = false;
	for (Card *c : hand) {
		int strength = c->getStrength(tsuit);
		hStrength += strength;

		if (c->getSuit() == tsuit)
			trumps++;
		else if (c->getRank() == Card::ACE)
			aces++;

		if (strength == 13)
			leftBower = true;
		else if (strength == 12)
			rightBower= true;
	}

	if ((trumps < 2 || (trumps == 2 && aces < 2) || (trumps == 3 && hStrength < 35)) &&
			!(leftBower && rightBower)) {	// Special cases
		return false;
	}

	trumpCards = trumps;
	aceCards = aces;
	left = leftBower, right = rightBower;
	handStrength = hStrength;
	return true;
}

int Player::promptNameTrump() {
	printHand();

	int t = 5;
	std::string in;

	while (t > 4 || t < -1) {
		if (!in.empty())
			std::cout << "Invalid choice!\n";

		std::cout << "Would you like to name trump? [Spades, Hearts, Clubs, Diamonds, Pass]: ";
		std::cin >> in;

		switch (std::tolower(in[0])) {
		case 's':
			return Card::SPADES;
		case 'h':
			return Card::HEARTS;
		case 'c':
			return Card::CLUBS;
		case 'd':
			return Card::DIAMONDS;
		case 'p':
			return -1;
		}
	} ;

	return t; // Should never hit this point, but just in case
}
int Player::decideNameTrump() {
	return promptNameTrump();
}

bool Player::promptGoAlone() {
	std::string s;
	std::cout << "Do you want to go alone? [Y/n]: ";
	std::cin >> s;
	return std::tolower(s[0]) == 'y';
}
bool Player::decideGoAlone() {
	return promptGoAlone();
}

int Player::promptPlayCard(int suit) {
	std::map<int, Card*> choices;
	std::map<int, Card*>::iterator it;
	Card *c;
	unsigned i;
	std::string in;

	if (suit >= 0) {									// If the player is not leading the trick
		for (i = 0; i < hand.size(); i++) {				// For every card in the player's hand
			c = hand[i];
			if (c->getSuit() == suit) {					// Register index and pointer to map if it follows suit
				choices.insert(std::pair<int, Card*>(i + 1, c));
			}
		}
	}

	if (choices.size() == 1) {							// If player has only one card that follows suit
		it = choices.begin();							// Automatically play the only available card
		c = it->second;
		std::cout << "You played " << c->getRankAsString() << " of " << c->getSuitAsString() << std::endl;
		return it->first - 1;

	} else if (choices.size() == 0) {					// If player has no cards that follow suit or is leading the trick
		i = 0;
		for (i = 0; i < hand.size(); i++) {				// Register all cards in hand to the choices map
			choices.insert(std::pair<int, Card*>(i + 1, hand[i]));
		}
	}

	for (std::pair<int, Card*> pair : choices) {		// Print choices
		std::cout << pair.first << ". " <<
				pair.second->getRankAsString() << " of " << pair.second->getSuitAsString() << std::endl;
	}

	do {
		if (!in.empty())									// If not first iteration of loop
			std::cout << "That's not a valid card!\n";

		std::cout << name << ", choose a card to play: ";
		std::cin >> in;
		i = in[0] - '0';									// Convert first character to integer
		it = choices.find(i);
	} while (it == choices.end());							// While choices map doesn't contain the key

	return it->first - 1;									// Return key - 1 (as array index)
}
int Player::decidePlayCard(int suit) {
	return promptPlayCard(suit);
}

int Player::promptDiscard() {
	printHand();

	std::string s;
	int i;

	do {
		if (!s.empty())
			std::cout << "That's not a valid card!\n";

		std::cout << "Which card would you like to discard? [1-" << hand.size() << "]: ";
		std::cin >> s;
		i = s[0] - '0';
	} while (i > static_cast<int>(hand.size()) || i <= 0);

	return i - 1;
}
int Player::decideDiscard() {
	return promptDiscard();
}

void Player::printHand() {
	std::cout << name << ", your cards are:\n";
	Card *c;
	for (unsigned i = 0; i < hand.size(); i++) {
		c = hand[i];
		std::cout << i + 1 << ". " << c->getRankAsString() << " of " << c->getSuitAsString() << std::endl;
	}
}

std::string Player::getName() {
	return name;
}

// Clears the player's hand
void Player::clearHand() {
	hand.clear();
}

void Player::resetTricksWon() {
	tricksWon = 0;
}

/** Pass a pointer to the player. If the hand isn't full, the pointer will be added to the hand vector
 *  If it fails, it will return false
 */
bool Player::giveCard(Card *c) {
	if(hand.size() >= 5)
		return false;

	hand.push_back(c);
	return true;
}

// Get the number cards in the player's hand
std::vector<Card*>::size_type Player::getHandSize() {
	return hand.size();
}

Card *Player::getLastPlayedCard() {
	return lastPlayed;
}

// Request that the player decide to have the dealer pick it up or not
bool Player::wantPickUp(int tsuit) {
	if (human)
		return promptPickUp();
	else
		return decidePickUp(tsuit);
}

int Player::nameTrump() {
	if (human)
			return promptNameTrump();
		else
			return decideNameTrump();
}

// Ask if the player wants to go alone
bool Player::goingAlone() {
	if (human)
		return promptGoAlone();
	else
		return decideGoAlone();
}

/**
 * Asks player to play a card
 * User or AI decides which card to play. That card is then removed from the hand and returned
 */
Card *Player::playCard(int suit) {
	int i;
	if (hand.size() == 1) {
		i = 0;
		std::cout << name << " played " << hand[0]->getRankAsString() << " of " << hand[0]->getSuitAsString() << std::endl;
	} else if (human) {
		i = promptPlayCard(suit);
	} else {
		i = decidePlayCard(suit);
	}

	lastPlayed = hand[i];
	hand.erase(hand.begin() + i);

	return lastPlayed;
}

/**
 * Asks player to discard a card
 * User or AI decides which card to discard. That card is then removed from the hand and returned
 */
Card *Player::discard() {
	int i;
	if (hand.size() == 1) {			// If there is only one card left, which shouldn't ever happen
		i = 0;
	} else if (human) {
		i = promptDiscard();
	} else {
		i = decideDiscard();
	}

	Card* ret = hand[i];
	hand.erase(hand.begin() + i);

	return ret;
}

/**
 * Returns a pointer to the last played card by the player
 * Clears that pointer internally to prevent the card from being duplicated
 */
Card *Player::takeLastPlayed() {
	Card *ret = lastPlayed;
	lastPlayed = 0;
	return ret;
}

// Increments the player's tricksWon value
void Player::winTrick() {
	tricksWon++;
}

// Gets the player's tricksWon value
int Player::getTricksWon() {
	return tricksWon;
}
