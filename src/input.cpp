#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "hearts.h"

using namespace std;

Input::Input(int id, int round) :
		IPlayer(id, round) {
}

vector<int> Input::passCards() {
	Card::sortCardsAscending(this->hand);

	cout << "Your current hand is: " << endl;
	for (unsigned int i = 0; i < this->hand.size(); i++) {
		cout << i << ": " << this->hand.at(i).getName() << endl;
	}

	cout << "Enter the 3 indices of the cards to give: " << endl;
	vector<int> ret;
	int index;
	for (int i = 0; i < 3; i++) {
		cin >> index;
		while (std::cin.fail() || index < 0 || index > 12 || count(ret.begin(), ret.end(), index) > 0) {
			std::cout << "Invalid Entry.\n" << std::endl;
			std::cin.clear();
			std::cin >> index;
		}
		ret.push_back(index);
		cout << "You are giving: " << this->hand.at(ret.at(i)).getName() << endl;
	}
	return ret;
}

/** Returns the index of the card that should be played based on the cards played so far */
int Input::decide(const Trick& trick) {
	Card::sortCardsAscending(this->hand);

	cout << "Your current hand is: " << endl;
	for (unsigned int i = 0; i < this->hand.size(); i++) {
		cout << i << ": " << this->hand.at(i).getName() << endl;
	}
	cout << "The played cards in this trick so far are: " << endl;
	for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
		cout << i << ": " << trick.playedCards.at(i).card.getName() << endl;
	}
	cout << "Enter the index of the card to play: " << endl;
	int decide;
	cin >> decide;
	Card decidedCard = this->hand.at(decide);
	cout << "You played: " << decidedCard.getName() << endl;
	return decide;
}
void Input::endOfTrick(const Trick& trick) {
	cout << "The end of the trick has been reached. The played cards are: " << endl;
	for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
		cout << trick.playedCards.at(i).card.getName() << ", ";
	}
	cout << endl;
}
