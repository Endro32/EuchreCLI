/*
 * Game.cc
 *
 *  Created on: Nov 1, 2019
 *      Author: webk7
 */

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Game.h"

Game::Game() {
	srand((unsigned) time(0)); 					// Set the random seed

	deck = new Deck();
	dealer = rand() % 4;
	maker = 0, leader = 0;
	goingAlone = false;
	trump = 0, curSuit = 0;
	top = nullptr;

	player0 = new Player();
	player1 = new Player("Friday");
	player2 = new Player("Jarvis");
	player3 = new Player("Ultron");

	player02Score = 0;
	player13Score = 0;
}

Game::~Game() {
	delete deck;

	delete player0;
	delete player1;
	delete player2;
	delete player3;
}

/**
 * The only public non-constructor function in the class
 * Plays as many hands as necessary for one team to reach ten points
 */
void Game::play() {
	playHand();
}

// Resets and shuffles deck, deals each player five cards after clearing their hand
void Game::dealHand() {
	deck->shuffle();
	Player *cur;
	for (int p = 0; p < 4; p++) {		// For each player
		cur = getPlayer(p);
		cur->clearHand();
		cur->resetTricksWon();
		for (int c = 0; c < 5; c++) {	// For five cards
			cur->giveCard(deck->dealCard());
		}
	}

	std::cout << "Cards have been dealt!\n";
}

// Plays a hand!
void Game::playHand() {
	if (++dealer > 4) // Increment dealer and loop back to zero if necessary
		dealer = 0;
	Player *dealerO = getPlayer(dealer);		// Dealer Object
	std::cout << dealerO->getName() << " is dealer.\n";

	dealHand();
	top = deck->flipTop();
	std::cout << "Top card is " << top->getSuitAsString() << std::endl;

	Player *player;
	int playerIndex;
	for (int i = 1; i < 10; i++) {		// Should never terminate here, but just in case, it won't be an infinite loop
		playerIndex = (i + dealer) % 4;
		player = getPlayer(playerIndex);

		if (i <= 4) {
			if (player->wantPickUp()) {	// If player wants dealer to pick it up
				trump = top->getSuit();
				goingAlone = player->goingAlone();
				deck->put_back(dealerO->discard());
				dealerO->giveCard(deck->dealCard());
				std::cout << player->getName() << " made the dealer pick it up.\n";
				maker = playerIndex;
				break;

			} else {	// Player passed
				std::cout << player->getName() << " passed.\n";
				continue;
			}

		} else if (i <= 8) {
			// nameTrump() has built-in validation, so none is needed here
			int t = player->nameTrump();
			if (t != -1) {	// If player named trump
				trump = t;
				std::cout << player->getName() << " named trump.\n";
				maker = playerIndex;
				goingAlone = player->goingAlone();
				break;

			} else {	// Player passed
				std::cout << player->getName() << " passed.\n";
				continue;
			}

		} else {
			std::cout << "\nReturning...\n";
			return;
		}
	}

	std::cout << "Trump is " << Card::getSuitAsString(trump) << std::endl;

	leader = (dealer + 1) % 4;								// Leader of the first trick is immediately left of dealer
	if (goingAlone && getPartnerIndex(maker) == leader)		// If that player's partner is going alone
		leader = (leader + 1) % 4;							// Leader of the first trick is directly across from the dealer

	for (int t = 0; t < 5; t++) {		// Play 5 tricks
		playTrick();
	}

	int makerTricks = getPlayer(maker)->getTricksWon() + getPartner(maker)->getTricksWon();
	int *makerScore, *defenderScore;

	if (maker == 0 || maker == 2) {		// Determine which score is which
		makerScore = &player02Score;
		defenderScore = &player13Score;
	} else if (maker == 1 || maker == 3) {
		makerScore = &player13Score;
		defenderScore = &player02Score;
	} else
		throw std::logic_error("I'm a dolphin");

	std::string output = "";

	if (makerTricks >= 5) {				// Makers win all 5 tricks
		if (goingAlone) {				// While playing alone
			*makerScore += 4;
			output = " while going alone. They get four points.";
		} else {						// Together
			*makerScore += 2;
			output = ". They get two points.";
		}
	} else if(makerTricks >= 3) {		// Makers win 3 or 4 tricks
		(*makerScore)++;
		output = ". They get one point.";
	} else {							// Makers win less than 3 tricks; Euchre!
		*defenderScore += 2;
		output = ". That means they got euchred, and their opponents get two points!";
	}

	std::cout << getPlayer(maker)->getName() + " and " + getPartner(maker)->getName() << " won " << makerTricks << output << std::endl;
	std::cout << player0->getName() << " and " << player2->getName() << " now have " << player02Score << " points.\n";
	std::cout << player1->getName() << " and " << player3->getName() << " now have " << player13Score << " points.\n";

}

/*
 * Plays a trick!
 * Makes each player play a card, starting with the leader
 * At the end, whoever played the highest card wins the trick and will be the leader of the next trick
 */
void Game::playTrick() {
	Card *curCard;
	int strongest, winner, index;
																// For the leader of the trick
	curCard = getPlayer(leader)->playCard(-1);					// Play a card
	curSuit = curCard->getSuit();								// Set the suit of the current trick
	strongest = curCard->getStrength(trump, curSuit);			// Currently strongest card is the one first played (duh)
	winner = leader;											// The leader played that card and has the lead

	for (int p = 1; p < 4; p++) {								// For the other three players
		index = (p + leader) % 4;								// Calculate the index of the player

		if (goingAlone && index == getPartnerIndex(maker))		// If this player's partner is going alone
			continue;											// Skip!

		curCard = getPlayer(index)->playCard(curSuit);
		int strength = curCard->getStrength(trump, curSuit);
		if (strength > strongest) {								// If it is the strongest card thus far
			strongest = strength;
			winner = index;
		}
	}										// Now all four players have played their cards, and we know who won the trick

	getPlayer(winner)->winTrick();
	std::cout << getPlayer(winner)->getName() << " won the trick\n\n";
	leader = winner;
}

// Gets player by index (0 is human, then clockwise)
Player *Game::getPlayer(int i) {
	if (i >= 4)
		i = i % 4;

	if (i <= 0)
		return player0;
	else if (i == 1)
		return player1;
	else if (i == 2)
		return player2;
	else
		return player3;
}

// Gets a player by their partner's index
Player *Game::getPartner(int i) {
	return getPlayer(getPartnerIndex(i));
}

// Gets a player's index by their partner's index
int Game::getPartnerIndex(int i) {
	if (i >= 4)
		i = i % 4;

	if (i <= 0)				// Player 0
		return 2;
	else if (i == 1)		// Player 1
		return 3;
	else if (i == 2)		// Player 2
		return 0;
	else					// Player 3
		return 1;
}
