#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

IPlayer::IPlayer(int id, int round) {
	this->id = id;
	this->points = 0;
}
int IPlayer::getPoints() const {
	return this->points;
}
bool IPlayer::isCardInHand(Card c) const {
	for (int i = 0; i < (int) hand.size(); i++) {
		Card currentCard = hand.at(i);
		if (currentCard.getCardRank() == c.getCardRank() && currentCard.getCardSuit() == c.getCardSuit()) {
			return true;
		}
	}
	return false;
}

int IPlayer::numberOfSuitInHand(int suit) const {
	int n = 0;
	for (unsigned int i = 0; i < hand.size(); i++) {
		if (hand.at(i).getCardSuit() == suit) {
			n++;
		}
	}
	return n;
}
void IPlayer::removeCard(int position) {
	this->hand.erase(this->hand.begin() + position);
}
void IPlayer::removeCard(Card card) {
	for (unsigned int i = 0; i < this->hand.size(); i++) {
		if (card == this->hand.at(i)) {
			this->removeCard(i);
		}
	}
}
void IPlayer::addCard(Card card) {
	this->hand.push_back(card);
}
vector<Card> IPlayer::getCards(std::vector<int> indices) {
	std::vector<Card> cards;
	for (unsigned i = 0; i < indices.size(); i++) {
		cards.push_back(hand[indices.at(i)]);
	}
	return cards;
}
bool IPlayer::isValidMove(const Trick& trick, Card card) const {
	// If you are the first to play
	if (trick.playedCards.size() == 0) {
		// If you have a two of clubs
		if (isCardInHand(Card(CLUBS, TWO))) {
			// If you want to play the two of clubs, you can
			if (card == Card(CLUBS, TWO)) {
				return true;
			}
			// Otherwise, you can't
			else {
				return false;
			}
		}
		// If you want to play hearts
		else if (card.getCardSuit() == HEARTS) {
			// If hearts are broken, or the only cards you have left are hearts, you can
			if (trick.heartsBroken || (int) hand.size() == numberOfSuitInHand(HEARTS)) {
				return true;
			}
			// Otherwise, you can't
			else {
				return false;
			}
		}
		// If you don't have a two of clubs or want to play hearts, the move is always valid
		else {
			return true;
		}
	}

	// If you are not the first to play
	else {
		// If you have cards matching the suit of the first card
		if (numberOfSuitInHand(trick.playedCards[0].card.getCardSuit()) > 0) {
			// If you are playing a card of the same suit as the first card, you can
			if (card.getCardSuit() == trick.playedCards[0].card.getCardSuit()) {
				return true;
			}
			// Otherwise, you can't
			else {
				return false;
			}
		}
		// If you don't have cards matching the suit of the first card
		else {
			// If this is the first trick, you can't play points, unless you only have points in your hand
			if (trick.playedCards[0].card == Card(CLUBS, TWO) && (card.getCardSuit() == HEARTS || card == Card(SPADES, QUEEN))) {
				for (unsigned i = 0; i < this->hand.size(); i++) {
					if (this->hand.at(i).getPoints() == 0) {
						return false;
					}
				}
				return true;
			}
			// Otherwise, any card will be legal to play
			else {
				return true;
			}
		}
	}
}
