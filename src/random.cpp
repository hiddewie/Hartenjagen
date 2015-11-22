#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

Random::Random(int id, int round) :
		IPlayer(id, round) {
	this->heartsBroken = false;
}
vector<int> Random::passCards() {
	vector<int> ret;
	for (int i = 0; i < 3; i++) {
		ret.push_back(i);
	}
	return ret;
}
int Random::decide(const Trick& trick) {
	int cardsPlayed = trick.playedCards.size();
	if (cardsPlayed == 0) {
		if (this->hand.size() == 13) {
			for (unsigned int i = 0; i < this->hand.size(); i++) {
				Card clubs2 = Card(CLUBS, TWO);
				if (clubs2 == this->hand.at(i)) {
					return i;
				}
			}
		}
		else {
			if (this->heartsBroken) {
				return 0;
			}
			else {
				unsigned int i = 0;
				while (i < this->hand.size() - 1 && this->hand.at(i).getCardSuit() == HEARTS) {
					i++;
				}
				return i;
			}
		}
	}
	else {
		Card firstCard = trick.playedCards.at(0).card;
		if (this->numberOfSuitInHand(firstCard.getCardSuit()) > 0) {
			for (unsigned int i = 0; i < this->hand.size(); i++) {
				if (this->hand.at(i).getCardSuit() == firstCard.getCardSuit()) {
					return i;
				}
			}
		}
		else {
			if (this->heartsBroken) {
				return 0;
			}
			else {
				unsigned int i = 0;
				while (i < this->hand.size() - 1 && this->hand.at(i).getCardSuit() == HEARTS) {
					i++;
				}
				return i;
			}
		}
	}
	cerr << "RANDOM ERROR !!! " << endl;
	throw 10;
	return -1;
}
void Random::endOfTrick(const Trick& trick) {
	if (!this->heartsBroken) {
		for (int i = 0; i < 4; i++) {
			if (trick.playedCards.at(i).card.getCardSuit() == HEARTS) {
				this->heartsBroken = true;
			}
		}
	}
}
