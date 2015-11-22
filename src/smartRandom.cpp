#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

SmartRandom::SmartRandom(int id, int round) :
		IPlayer(id, round) {
	this->heartsBroken = false;
}

int SmartRandom::getLowestRankIndex(bool heartsBroken) {
	int highestCardRank = 1000, lowestCardIndex = 0;
	Card currentCard;
	for (unsigned int i = 0; i < hand.size(); i++) {
		currentCard = hand.at(i);
		if (currentCard.getCardRank() < highestCardRank && (heartsBroken || currentCard.getCardSuit() != HEARTS)) {
			highestCardRank = currentCard.getCardRank();
			lowestCardIndex = i;
		}
	}
	return lowestCardIndex;
}
int SmartRandom::getHighestRankIndex(bool heartsBroken) {
	int highestCardRank = 0, highestCardIndex = 0;
	Card currentCard;
	for (unsigned int i = 0; i < hand.size(); i++) {
		currentCard = hand.at(i);
		if (currentCard.getCardRank() > highestCardRank && (heartsBroken || currentCard.getCardSuit() != HEARTS)) {
			highestCardRank = currentCard.getCardRank();
			highestCardIndex = i;
		}
	}
	return highestCardIndex;
}
int SmartRandom::getIndex(Card c) {
	for (unsigned int i = 0; i < hand.size(); i++) {
		if (c == hand.at(i)) {
			return i;
		}
	}
	throw Error("Card " + c.getName() + " not in hand.");
}

vector<int> SmartRandom::passCards() {
	int highestPoints[] = { 0, 0, 0 };
	int highestCardsIndidces[] = { 0, 0, 0 };
	Card currentCard;
	for (unsigned int i = 0; i < hand.size(); i++) {
		currentCard = hand.at(i);
		if (currentCard.getCardRank() >= highestPoints[0]) {
			highestPoints[2] = highestPoints[1];
			highestPoints[1] = highestPoints[0];
			highestPoints[0] = currentCard.getCardRank();
			highestCardsIndidces[2] = highestCardsIndidces[1];
			highestCardsIndidces[1] = highestCardsIndidces[0];
			highestCardsIndidces[0] = i;
		}
		else if (currentCard.getCardRank() >= highestPoints[1]) {
			highestPoints[2] = highestPoints[1];
			highestPoints[1] = currentCard.getCardRank();
			highestCardsIndidces[2] = highestCardsIndidces[1];
			highestCardsIndidces[1] = i;
		}
		else if (currentCard.getCardRank() >= highestPoints[2]) {
			highestPoints[2] = currentCard.getCardRank();
			highestCardsIndidces[2] = i;
		}
	}
	vector<int> ret;
	for (int i = 0; i < 3; i++) {
		ret.push_back(highestCardsIndidces[i]);
	}
	return ret;
}
int SmartRandom::decide(const Trick& trick) {
	Card::sortCardsDescending(hand);

	for (int i = 0; i < 4; i++) {
		sortedHand[i].clear();
	}
	for (unsigned int i = 0; i < hand.size(); i++) {
		sortedHand[hand.at(i).getCardSuit()].push_back(hand.at(i));
	}
	for (int i = 0; i < 4; i++) {
		Card::sortCardsDescending(sortedHand[i]);
	}

	int cardsPlayed = trick.playedCards.size();
	if (cardsPlayed == 0) { // first player
		if (hand.size() == 13) { // first trick, first player
			for (unsigned int i = 0; i < hand.size(); i++) {
				Card clubs2 = Card(CLUBS, TWO);
				if (clubs2 == this->hand.at(i)) {
					return i;
				}
			}
		}
		else { // first player, not first trick
			int i = hand.size() - 1;
			while (i > 0 && (!heartsBroken && hand.at(i).getCardSuit() == HEARTS)) {
				i--;
			}
			return i;
		}
	}
	else { // second, third or last player
		int firstSuit = trick.playedCards.at(0).card.getCardSuit();
		if (sortedHand[firstSuit].size() > 0) { // match suit
			for (int i = (int) sortedHand[firstSuit].size() - 1; i>= 0; i--) {
				if (sortedHand[firstSuit].at(i).getCardRank() < trick.playedCards.at(0).card.getCardRank()) {
					return i;
				}
			}
			return getIndex(sortedHand[firstSuit].at(0));
		}
		else { // cannot match suit
			if (heartsBroken && sortedHand[HEARTS].size() > 0) {
				return getIndex(sortedHand[HEARTS].at(0));
			}
			else {
				int i = hand.size() - 1;
				while (i > 0 && hand.at(i).getCardSuit() == HEARTS) {
					i --;
				}
				return i;
			}
		}
	}
	throw Error("SmartRandom error!");
}
void SmartRandom::endOfTrick(const Trick& trick) {
	if (!heartsBroken) {
		for (int i = 0; i < 4; i++) {
			if (trick.playedCards.at(i).card.getCardSuit() == HEARTS) {
				heartsBroken = true;
			}
		}
	}
}
