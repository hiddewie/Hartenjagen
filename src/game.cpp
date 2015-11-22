#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <signal.h>

#include "hearts.h"

// Players
#include "KoenHidde.h"

using namespace std;

static const int localPassIndices[] = { 1, 3, 2, 0 };
;
vector<int> Game::passIndices(localPassIndices, localPassIndices + sizeof(localPassIndices) / sizeof(localPassIndices[0])); // = {1, 3, 2, 0};
Game::Game() {
	this->seed = 0;
	this->roundIndex = 0;
}
Game::~Game() {
	for (unsigned int i = 0; i < this->players.size(); i++) {
		delete this->players.at(i).player;
	}
}

void Game::addPlayer(IPlayer *player) {
	PlayerData newPlayer;
	newPlayer.player = player;
	newPlayer.points = 0;
	this->players.push_back(newPlayer);
}

vector<int> Game::playGame(vector<sig_atomic_t> playerIDs, bool viewRound) {
	vector<int> totalPoints(4, 0);

	bool keepPlaying = true;
	if (viewRound) {
		cout << " --- Starting game --- " << endl;
	}
	while (keepPlaying) {
		this->clearPlayers();

		for (int i = 0; i < 4; i++) {
			switch (playerIDs.at(i)) {
				case 0:
					addPlayer(new Random(i, roundIndex));
					break;
				case 1:
					addPlayer(new SmartRandom(i, roundIndex));
					break;
				case 2:
					addPlayer(new KoenHidde(i, roundIndex));
					break;
				default:
					throw Error("Invalid player ID ("+Error::getNumberAsString(playerIDs.at(i))+").");
					break;
			}
		}

		this->shuffle();

		this->passCards(viewRound);

		vector<int> roundPoints = this->playRound(viewRound);
		int pit = -1; // the id of the player which played pit, -1 otherwise
		for (int i = 0; i < 4; i++) {
			if (roundPoints.at(i) == 26) {
				pit = i;
			}
		}
		if (pit == -1) { // there was no pit
			for (int i = 0; i < 4; i++) {
				totalPoints.at(i) += roundPoints.at(i);
			}
		}
		else { // there was a pit
			if (viewRound) {
				cout << "Pit has been played" << endl;
			}
			for (int i = 0; i < 4; i++) {
				if (i != pit) {
					totalPoints.at(i) += 26;
				}
			}
		}
		if (ROUND) {
			for (int i = 0; i < 3; i++) {
				cout << roundPoints.at(i) << ", ";
			}
			cout << roundPoints.at(3) << endl;
			for (int i = 0; i < 3; i++) {
				cout << totalPoints.at(i) << ", ";
			}
			cout << totalPoints.at(3) << endl;
		}
		for (int i = 0; i < 4; i++) {
			if (totalPoints.at(i) >= MAX_POINTS) {
				keepPlaying = false;
			}
		}
		if (viewRound) {
			keepPlaying = false;
		}
		this->roundIndex++;
	}
	if (viewRound) {
		cout << endl << " --- Game over --- " << endl;
	}

	vector<int> losers;
	for (int i = 0; i < 4; i++) {
		if (totalPoints.at(i) >= MAX_POINTS) {
			losers.push_back(i);
		}
	}
	return losers;
}
vector<int> Game::playRound(bool viewRound) {
	vector<int> errorReturn(4, 0);
	int tricksPlayed = 0;
	vector<Trick> tricks;
	bool playedCards[52];
	int startingPlayer;

	if (viewRound) {
		cout << " --- Starting round " << (roundIndex + 1) << " --- " << endl;
	}
	if (players.size() != 4) {
		throw Error("Invalid number of players");
	}

	for (int i = 0; i < 52; i++) {
		playedCards[i] = false;
	}

	for (int i = 0; i < 4; i++) {
		if (players.at(i).player->isCardInHand(Card(CLUBS, TWO))) {
			startingPlayer = i;
			break;
		}
	}

	if (viewRound) {
		cout << "Player " << startingPlayer << " begins" << endl;
	}
	bool heartsBroken = false;
	while (tricksPlayed < 13) {
		if (viewRound) {
			cout << endl << " --- Playing trick nr " << (tricksPlayed + 1) << " --- \n";
		}

		Trick currentTrick(tricksPlayed, heartsBroken);

		for (int i = 0; i < 4; i++) {
			PlayedCard pc;
			pc.player = players.at((startingPlayer + i) % 4).player;
			int decideIndex = pc.player->decide(currentTrick);
			pc.card = pc.player->hand.at(decideIndex);
			if (viewRound) {
				cout << "Player " << (startingPlayer + i) % 4 << " plays " << pc.card.getName() << endl;
			}
			if (playedCards[(int) pc.card] == true) {
				throw Error(
						"Player " + Error::getNumberAsString((startingPlayer + i) % 4)
								+ " disqualified because card was already played, game aborting.");
			}
			if (tricksPlayed == 0 && pc.card.getPoints() > 0) {
				int hearts = pc.player->numberOfSuitInHand(HEARTS);
				if (!(hearts == 13 || (hearts != 12 && pc.player->isCardInHand(Card(SPADES, QUEEN))))) {
					if (ERROR) {
						cout << pc.card.getName() << endl;
						Card::printCards(pc.player->hand);
					}
					throw Error(
							"Player " + Error::getNumberAsString(pc.player->id)
									+ " is disqualified because of playing points in the first round, while having other possibilities.");
				}
			}
			playedCards[(int) pc.card] = true;
			currentTrick.playedCards.push_back(pc);

			if (i > 0 && pc.card.getCardSuit() != currentTrick.playedCards.at(i).card.getCardSuit()
					&& pc.player->numberOfSuitInHand(pc.card.getCardSuit()) > 0) {
				throw Error(
						"Player " + Error::getNumberAsString((startingPlayer + i) % 4)
								+ " disqualified because he must play the played suit, game aborting.");
			}
		}
		if (!heartsBroken
				&& (currentTrick.playedCards.at(0).card.getCardSuit() == HEARTS
						&& currentTrick.playedCards.at(0).player->numberOfSuitInHand(HEARTS) != 13 - tricksPlayed)) {
			throw Error(
					"Player " + Error::getNumberAsString(startingPlayer) + " disqualified because the hearts were not yet broken, game aborting.");
		}

		if (tricksPlayed == 0 && !(currentTrick.playedCards.at(0).card == Card(CLUBS, TWO))) {
			throw Error(
					"ID: " + Error::getNumberAsString(currentTrick.playedCards.at(0).player->id)
							+ ", first played card was not the 2 of clubs, game aborting.");
		}
		for (int i = 0; i < 4; i++) {
			currentTrick.playedCards.at(i).player->removeCard(currentTrick.playedCards.at(i).card);
		}

		if (VERBOSE) {
			cout << "Cards played in this trick: ";

			for (unsigned int i = 0; i < currentTrick.playedCards.size(); i++) {
				cout << currentTrick.playedCards.at(i).card.getName() << ", ";
			}
			cout << endl << "------" << endl;
		}
		if (viewRound) {
			cout << "Winner is player " << currentTrick.getWinner()->id << endl;
			cout << "Points of this trick: " << currentTrick.getPoints() << endl;
		}
		players.at(currentTrick.getWinner()->id).player->points += currentTrick.getPoints();
		// For the next trick:
		startingPlayer = currentTrick.getWinner()->id;
		tricksPlayed++;

		if (!heartsBroken) {
			for (int i = 0; i < 4; i++) {
				if (currentTrick.playedCards.at(i).card.getCardSuit() == HEARTS) {
					heartsBroken = true;
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			players.at(i).player->endOfTrick(currentTrick);
		}
	}

	vector<int> roundPoints;

	if (viewRound) {
		cout << endl << " --- Round " << roundIndex + 1 << " finished --- " << endl;
	}
	for (int i = 0; i < 4; i++) {
		roundPoints.push_back(players.at(i).player->getPoints());
		if (viewRound) {
			cout << "Player " << i << " has " << roundPoints.at(i) << " points" << endl;
		}
	}

	return roundPoints;
}

unsigned long long Game::getRandomNumber() {
	//return 941223932;

	unsigned int lo, hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((((unsigned long long) hi << 32) | lo) % time(0));
}
void Game::shuffle() {
	vector<int> deck;

	for (int i = 0; i < 52; i++) { // Create new sorted full deck of cards
		deck.push_back(i);
	}
	unsigned long long temp = getRandomNumber();

	srand(temp);

	if (ROUND) {
		cout << "Random Seed:" << temp << endl;
	}

	random_shuffle(deck.begin(), deck.end()); // Shuffle full deck of cards

	for (int i = 0; i < (int) deck.size(); i++) {
		this->players.at((int) (i / 13)).player->hand.push_back(Card(deck.at(i)));
	}
}

void Game::passCards(bool viewRound) {
	vector<int> passedCardPositions[4];
	vector<Card> passedCards[4];
	for (int i = 0; i < 4; i++) {
		passedCardPositions[i] = players.at(i).player->passCards();

		sort(passedCardPositions[i].rbegin(), passedCardPositions[i].rend());

		vector<Card> playerHand = players.at(i).player->hand;
		for (int j = 0; j < 3; j++) {
			passedCards[i].push_back(playerHand.at(passedCardPositions[i].at(j)));
			players.at(i).player->removeCard(passedCardPositions[i].at(j));
		}
		if (viewRound) {
			cout << "Player " << i << " gives: ";
			for (int j=0; j<3; j++) {
				cout << passedCards[i].at(j).getName() << " ";
			}
			cout << endl;
		}
	}
	string passing[] = { "left", "right", "straight", "same person" };
	if (ROUND) {
		cout << "Passing cards to the " << passing[roundIndex % 4] << endl;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			players.at((i + Game::passIndices[roundIndex % 4]) % 4).player->addCard(passedCards[i].at(j));
		}
	}

	if (ROUND) {
		cout << " --- All cards have been given ---" << endl;
	}

	for (int i = 0; i < 4; i++) {
		players.at(i).player->evaluateHand();
		if (viewRound) {
			cout << "Hand of player " << i << " after giving: " << endl;
			Card::printCards(players.at(i).player->hand);
		}
	}
}

void Game::printPlayers() {
	for (int i = 0; i < (int) players.size(); i++) {
		Card::printCards(players.at(i).player->hand);
	}
}

void Game::clearPlayers() {
	for (unsigned int i = 0; i < this->players.size(); i++) {
		delete this->players.at(i).player;
	}
	this->players.clear();
}

