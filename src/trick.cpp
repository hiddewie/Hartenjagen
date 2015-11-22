#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

Trick::Trick(int trickNumber, bool heartsBroken) {
	this->trickNumber = trickNumber;
	this->heartsBroken = heartsBroken;
}
IPlayer* Trick::getWinner() const {
	PlayedCard firstCard = playedCards.at(0);
	for (int i = 1; i < (int) playedCards.size(); i++) {
		PlayedCard currentCard = playedCards.at(i);
		if (currentCard.card.getCardSuit() == firstCard.card.getCardSuit() && currentCard.card.getCardRank() > firstCard.card.getCardRank()) {
			firstCard = currentCard;
		}
	}
	return firstCard.player;
}

int Trick::getPoints() const {
	int total = 0;
	for (int i = 0; i < (int) playedCards.size(); i++) {
		total += playedCards.at(i).card.getPoints();
	}
	return total;
}
