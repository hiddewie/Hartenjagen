#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

string Card::ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
string Card::suits[] = { "Hearts", "Diamonds", "Clubs", "Spades" };

Card::Card() { // Card()
	card = 0;
}
Card::Card(int c) { // Card(0), Card(15), ...
	card = c;
}
Card::Card(int suit, int rank) { // Card(HEARTS, KING), Card(SPADES, THREE), ...
	card = suit * 13 + rank;
}

int card;

/** Returns the characteristic name of the card */
string Card::getName() const {
	return Card::ranks[(int) (this->getCardRank())] + " " + Card::suits[(int) (this->getCardSuit())];
}

/** Returns the suit of the card (hearts = 0, diamonds = 1, clubs = 2, spades = 3) */
int Card::getCardSuit() const {
	return (int) (card / 13);
}

/** Returns the rank of the card (jack = 11, queen = 12, king = 13, ace = 14) */
int Card::getCardRank() const {
	return card % 13;
}

/** Returns the number of points a card is worth */
int Card::getPoints() const {
	if (this->getCardSuit() == HEARTS) {
		return 1;
	}
	else if (this->getCardSuit() == SPADES && this->getCardRank() == QUEEN) {
		return 13;
	}
	return 0;
}

/** Returns the integer value of this card */
Card::operator int() const {
	return this->card;
}

void Card::sortCardsAscending(vector<Card>& cards) {
	sort(cards.begin(), cards.end());
}
void Card::sortCardsDescending(vector<Card>& cards) {
	sort(cards.rbegin(), cards.rend());
}

/** print the cards in this hand*/
void Card::printCards(vector<Card>& cards) {
	vector<Card> sortedCards[4];
	int size = cards.size();

	for (int i = 0; i < size; i++) {
		sortedCards[cards.at(i).getCardSuit()].push_back(cards.at(i));
	}
	for (int i = 0; i < 4; i++) {
		//Card::sortCardsDescending(sortedCards[i]);
	}

	for (int i = 0; i < 4; i++) {
		cout << " " << Card::suits[i] << ": ";
		if (sortedCards[i].size() > 0) {
			for (unsigned int j = 0; j < sortedCards[i].size() - 1; j++) {
				cout << Card::ranks[sortedCards[i].at(j).getCardRank()] << ", ";
			}
			cout << Card::ranks[sortedCards[i].at(sortedCards[i].size() - 1).getCardRank()];
		}
		cout << endl;
	}
}
void Card::printCards(vector<PlayedCard>& playedCards) {
	vector<Card> cards;
	for (unsigned int i = 0; i < playedCards.size(); i++) {
		cards.push_back(playedCards.at(i).card);
	}
	Card::printCards(cards);
}
void Card::printCards(vector<Card>& hand, vector<int>& indices) {
	vector<Card> cards;
	for (unsigned int i = 0; i < indices.size(); i++) {
		if (indices.at(i) < (int) hand.size()) {
			cards.push_back(hand.at(i));
		}
	}
	Card::printCards(cards);
}

bool Card::operator <(const Card& c) const {
	return (card < c.card);
}
bool Card::operator >(const Card& c) const {
	return (card > c.card);
}
bool Card::operator <=(const Card& c) const {
	return (card <= c.card);
}
bool Card::operator >=(const Card& c) const {
	return (card >= c.card);
}
bool Card::operator ==(const Card& c) const {
	return (card == c.card);
}
