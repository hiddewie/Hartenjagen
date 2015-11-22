#include "standard_includes_defines.h"

#include "CardData.h"

#include "KoenHidde.h"

#include <map>

CardData::CardData(KoenHidde* koenHidde) {
	this->koenHidde = koenHidde;
	this->haveQueen = false;
	for (int i = 0; i < 52; i++) {
		this->playedCards[i] = false;
	}
	heartsBroken = false;
	for (int i = 0; i < 4; i++) {
		this->numberOfPointCardsTaken[i] = 0;
	}
}
;

/** Initializes the numberOfCardsPerSuit array */
void CardData::findNumberOfCardsPerSuit() { // run in constructor?
	for (int i = 0; i < 4; i++) {
		numberOfCardsPerSuit[i] = this->koenHidde->numberOfSuitInHand(i);
	}
}
/** Initializes the suitsSortedOnNumberOfCards array */
void CardData::sortSuitsOnNumberOfCards() {
	for (int i = 0; i < 3; i++) {
		suitsSortedOnNumberOfCards[i] = i;
	}

	for (int i = 0; i < 2; i++) { // sort with bubble sort
		for (int j = 0; j < 2; j++) {
			if (koenHidde->numberOfSuitInHand(suitsSortedOnNumberOfCards[j]) > koenHidde->numberOfSuitInHand(suitsSortedOnNumberOfCards[j + 1])) {
				// swap
				int temp = suitsSortedOnNumberOfCards[j];
				suitsSortedOnNumberOfCards[j] = suitsSortedOnNumberOfCards[j + 1];
				suitsSortedOnNumberOfCards[j + 1] = temp;
			}
			else if (koenHidde->numberOfSuitInHand(suitsSortedOnNumberOfCards[j])
					== koenHidde->numberOfSuitInHand(suitsSortedOnNumberOfCards[j + 1])) {
				if (suitsSortedOnNumberOfCards[j] < suitsSortedOnNumberOfCards[j + 1]) { //                                               ---------- CHECK ME PLEASE ----------
					// swap
					int temp = suitsSortedOnNumberOfCards[j];
					suitsSortedOnNumberOfCards[j] = suitsSortedOnNumberOfCards[j + 1];
					suitsSortedOnNumberOfCards[j + 1] = temp;
				}
			}
		}
	}
}
;

/** Initializes the sortedHand list */
void CardData::sortHand() {
	for (int i = 0; i < 4; i++) {
		sortedHand[i] = vector<Card>();
	}

	for (unsigned int i = 0; i < koenHidde->hand.size(); i++) { //sort cards in hand by suits
		Card c = Card(koenHidde->hand.at(i));
		sortedHand[c.getCardSuit()].push_back(c);
	}

	for (unsigned int i = 0; i < 4; i++) { // sort each suit from large to small with bubble sort
		for (unsigned int k = 0; k < sortedHand[i].size(); k++) {
			for (unsigned int j = 0; j < sortedHand[i].size() - 1; j++) {
				if (sortedHand[i].at(j) < sortedHand[i].at(j + 1)) {
					//swap
					Card temp = sortedHand[i].at(j);
					sortedHand[i].at(j) = sortedHand[i].at(j + 1);
					sortedHand[i].at(j + 1) = temp;
				}
			}
		}
	}
}
;

/** Initializes the queen variable */
void CardData::findQueen() {
	this->haveQueen = this->koenHidde->isCardInHand(Card(SPADES, QUEEN));
}

/** Finds the queen, sorts the hand, initializes and fills data structures */
void CardData::evaluateHand() {
	this->findQueen();
	this->sortHand();
	this->findNumberOfCardsPerSuit();
	this->sortSuitsOnNumberOfCards();

	if (passedCards.size() == 3 && koenHidde->hand.size() == 13) {
		// Initialize card distribution (cardDist)
		for (int i = 0; i < 4; i++) { // All cards have a 1/3 chance of being with a player
			for (int j = 0; j < 52; j++) {
				cardDist[i][j] = koenHidde->id == i ? 0 : (1.0/ 3.0);
			}
		}
		for (unsigned int i = 0; i < 13; i++) { // we are sure of the cards we have ourselves
			unsigned int cardInt = (int) koenHidde->hand.at(i);
			for (int j = 0; j < 4; j++) {
				cardDist[j][cardInt] = j == koenHidde->id ? 1.0 : 0;
			}
		}
		for (unsigned int i = 0; i < 3; i++) { // we are sure of the cards we have given
			unsigned int cardInt = (int) passedCards.at(i);
			for (int j = 0; j < 4; j++) {
				cardDist[j][cardInt] = j == (koenHidde->id + Game::passIndices[koenHidde->roundNumber % 4] % 4) ? 1.0 : 0;
			}
		}

		if (VERBOSE)
			cout << "Hand of Player " << koenHidde->id << ":" << endl;
		if (VERBOSE)
			Card::printCards(koenHidde->hand);
	}
}

/** Returns the indices (in the hand) belonging to the given cards. Throws if cards are not found */
vector<int> CardData::getIndices(vector<Card> cards) {
	vector<int> indices;
	for (unsigned int i = 0; i < koenHidde->hand.size(); i++) {
		for (unsigned int j = 0; j < cards.size(); j++) {
			if (cards.at(j) == koenHidde->hand.at(i)) {
				indices.push_back(i);
			}
		}
	}

	if (indices.size() != cards.size()) {
		throw;
	}
	return indices;
}
;
/** Returns the index of the given cards in the hand */
int CardData::getIndex(Card card) {
	vector<Card> temp;
	temp.push_back(card);
	return this->getIndices(temp).at(0);
}

/** Update the card distribution for each player for each card, based on the cards in the given trick */
void CardData::updateCardDist(const Trick& trick) {
	for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
		int cardInt = (int) trick.playedCards.at(i).card;
		this->playedCards[cardInt] = true;

		for (int j = 0; j < 4; j++) {
			cardDist[j][cardInt] = 0;
		}
	}
	if (trick.playedCards.size() > 1) { //two or more cards played(updating cardDist with the cards of players who cannot match suit)
		int baseSuit = trick.playedCards.at(0).card.getCardSuit();
		float PToBeDivided, norm;

		for (unsigned int i = 1; i < trick.playedCards.size(); i++) {
			if (trick.playedCards.at(i).card.getCardSuit() != baseSuit) { // player did not admit suit: he has no *baseSuit* suited cards left so divide his prob propoortionally over the rest
				for (int j = baseSuit * 13; j < (baseSuit + 1) * 13; j++) {

					PToBeDivided = cardDist[trick.playedCards.at(i).player->id][j];
					norm = 1.0 - PToBeDivided;
					for (int p = 0; p < 4; p++) {
						if (p != trick.playedCards.at(i).player->id) { // loop over relevant players to modify
							cardDist[p][j] += (cardDist[p][j] / norm) * PToBeDivided; // proportioneelheidsfactor * PToBeDivided
						}
					} // done dividing
					cardDist[trick.playedCards.at(i).player->id][j] = 0;
				}
			}
		}
	}
}

bool CardData::avoidPit() {
	int numberOfPlayersWithNoPointCards = 0;
	int numberOfPlayedPointCards = 0;
	for (int i = 0; i < 4; i++) {
		if (numberOfPointCardsTaken[i] == 0) {
			numberOfPlayersWithNoPointCards++;
		}
		numberOfPlayedPointCards += numberOfPointCardsTaken[i];
	}
	if (numberOfPlayedPointCards >= 3 && numberOfPlayersWithNoPointCards == 3 && numberOfPointCardsTaken[koenHidde->id] == 0) {
		return true;
	}
	return false;
}

/** Sorts all the cards in the hand based on PWin from high to low */
vector<Card> CardData::sortHandOnPWin(const Trick& trick) {
	vector<Card> sortedHandOnPWin = koenHidde->hand;
	vector<Card> sortedHandOnLowPWin;
	map<Card, float> cardPwinMap;

	for (unsigned int i = 0; i < sortedHandOnPWin.size(); i++) {
		cardPwinMap[sortedHandOnPWin.at(i)] = PWin(trick, sortedHandOnPWin.at(i));
	}

	Card temp;
	for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { // remove all with <= PWIN_SUB PWin
		if (cardPwinMap[sortedHandOnPWin.at(i)] <= P_SUB) {
			sortedHandOnLowPWin.push_back(sortedHandOnPWin.at(i));
			sortedHandOnPWin.erase(sortedHandOnPWin.begin() + i);
		}
	}
	for (unsigned int i = 0; i < sortedHandOnPWin.size(); i++) { // sort with bubble sort
		for (unsigned int j = 0; j < sortedHandOnPWin.size() - 1; j++) {
			temp = sortedHandOnPWin.at(j);
			if (cardPwinMap[temp] < cardPwinMap[sortedHandOnPWin.at(j + 1)]) {
				// swap!
				sortedHandOnPWin.at(j) = sortedHandOnPWin.at(j + 1);
				sortedHandOnPWin.at(j + 1) = temp;
			}
		}
	}

	for (unsigned int i = 0; i < sortedHandOnLowPWin.size(); i++) { // sort with bubble sort
		for (unsigned int j = 0; j < sortedHandOnLowPWin.size() - 1; j++) {
			temp = sortedHandOnLowPWin.at(j);
			if (temp.getCardRank() < sortedHandOnLowPWin.at(j + 1).getCardRank()) {
				// swap!
				sortedHandOnLowPWin.at(j) = sortedHandOnLowPWin.at(j + 1);
				sortedHandOnLowPWin.at(j + 1) = temp;
			}
		}
	}

	// Concatenate SHOLP to the end of SHOP
	sortedHandOnPWin.insert(sortedHandOnPWin.end(), sortedHandOnLowPWin.begin(), sortedHandOnLowPWin.end());

	return sortedHandOnPWin;
}

/** Returns all cards from the given suit with a PWin smaller than bound from high to low */
vector<Card> CardData::sortSuitOnConditionalPWin(const Trick& trick, int suit, float bound) {
	vector<Card> sortedHand;
	Card currentCard;
	for (unsigned int i = 0; i < koenHidde->hand.size(); i++) {
		currentCard = koenHidde->hand.at(i);
		if (currentCard.getCardSuit() == suit && PWin(trick, currentCard) <= bound) {
			sortedHand.push_back(currentCard);
		}
	}

	Card::sortCardsDescending(sortedHand);

	return sortedHand;
}

/** Returns the chance the given player can match the given suit */
float CardData::PSuitMatch(int id, int suit) {
	float negSuitMatch = 1.0;
	for (int i = suit * 13; i < (suit + 1) * 13; i++) {
		negSuitMatch *= (1.0 - cardDist[id][i]);
	}
	return 1.0 - negSuitMatch;
}
/** Returns the chance the given player can play a higher card than the given card */
float CardData::PHigherCard(int id, Card card) {
	float lower = 0;
	float higher = 0;
	for (int i = 13 * card.getCardSuit(); i < (card.getCardSuit() + 1) * 13; i++) {
		if (i % 13 > card.getCardRank()) {
			higher += cardDist[id][i];
		}
		else {
			lower += cardDist[id][i];
		}
	}

	if (higher == 0 && lower == 0) {
		return 0;
	}
	else {
		return higher / (lower + higher);
	}
}
float CardData::PHave(int id, Card card) {
	return cardDist[id][card];
}
/** Works! */
float CardData::ENumberOfCardsOfSuit(int id, int suit) {
	float E = 0;
	for (int i = 13 * suit; i < 13 * (suit + 1); i++) {
		E += cardDist[id][i];
	}
	return E;
}
/** Works! */
float CardData::PPlay(int id, int playedSuit, Card card) {
	float PSuitMatch = this->PSuitMatch(id, playedSuit);
	float PHave = this->PHave(id, card);

	if (PHave <= P_SUB) {
		//cout << "(PPlay base) PHave = 0 for player " << id << endl;
		return 0;
	}

	// The chance the card is played when the player is matching
	float matchChance = 0;
	//cout << "PSuitMatch: " << PSuitMatch << ", ENumberOfCardsOfSuit(id, suit): " << ENumberOfCardsOfSuit(id, playedSuit) << ", PlayedSuit: " << playedSuit << endl;
	if (PSuitMatch >= P_SUB && playedSuit == card.getCardSuit() && ENumberOfCardsOfSuit(id, playedSuit) >= P_SUB) {
		matchChance = PSuitMatch * PHave / ENumberOfCardsOfSuit(id, playedSuit);
	}

	float ENumberOfCardsOfOtherSuits = 0;
	for (int suit = 0; suit < 4; suit++) {
		if (suit != playedSuit) {
			ENumberOfCardsOfOtherSuits += ENumberOfCardsOfSuit(id, suit);
		}
	}
	// The chance the card is played when the player is not matching
	float notMatchChance = 0;
	if (1.0 - PSuitMatch >= P_SUB && playedSuit != card.getCardSuit() && ENumberOfCardsOfOtherSuits >= P_SUB) {
		notMatchChance = (1.0 - PSuitMatch) * PHave / ENumberOfCardsOfOtherSuits;
	}
	//cout << "(PPlay base) Player " << id << " " << matchChance << " " << notMatchChance << endl;
	return matchChance + notMatchChance;
}
/** Works! */
float CardData::PPlay(int playedSuit, Card card) {
	// 3 players to go.
	float chance = 0;
	for (int i=0; i<4; i++) {
		if (i != koenHidde->id) {
			float P = PPlay(i, playedSuit, card);
			//cout << "(PPlay all) Player " << i << " " << P << endl;
			chance += P;
		}
	}

	return chance;
}
float CardData::PPlay(const Trick& trick, Card card) {
	if (trick.playedCards.size() == 0) {
		throw Error ("There are no played cards");
	}
	float P = 0;
	for (int i=1; i<4 - (int) trick.playedCards.size(); i++) {
		// player id = (our ID + i) % 4
		P += PPlay((koenHidde->id + i) % 4, trick.playedCards.at(0).card.getCardSuit(), card);
	}
	return P;
}
/** Expected number of points NOT including the queen */
float CardData::EPoints (int playedSuit) {
	float E = 0;
	for (int i=0; i<4; i++) {
		if (i != koenHidde->id) {
			for (int c = Card(HEARTS, TWO); c <= Card(HEARTS, ACE); c++) {
				E += 1.0 * PPlay((koenHidde->id + i) % 4, playedSuit, Card(c));
			}
		}
	}
	return E;
}
float CardData::EPoints (const Trick& trick) {
	if (trick.playedCards.size() == 0) {
		throw Error ("There are no played cards");
	}
	float E = 0;
	for (int i=1; i<4 - (int) trick.playedCards.size(); i++) {
		// player id = (our ID + i) % 4
		for (int c = Card(HEARTS, TWO); c <= Card(HEARTS, ACE); c++) {
			E += 1.0 * PPlay((koenHidde->id + i) % 4, trick.playedCards.at(0).card.getCardSuit(), Card(c));
		}
	}
	return E;
}
/** Returns the chance we can win the current trick with the given card */
float CardData::PWin(const Trick& trick, Card card) {
	int playedCards = trick.playedCards.size();

	// If the card doesn't match suit, we cannot win
	if (playedCards > 0 && trick.playedCards.at(0).card.getCardSuit() != card.getCardSuit()) { // we are not matching suit
		return 0;
	}
	// We are matching suit, or no cards have been played
	for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
		if (trick.playedCards.at(i).card.getCardSuit() == card.getCardSuit() && card < trick.playedCards.at(i).card) {
			return 0;
		}
	}
	// We are still matching suit, and our card beats all played cards
	float winChance = 1.0;
	int nextPlayers = 4 - 1 - playedCards;

	// TODO: implement different card choosing strategies when matching cards of other players
	// Here we assume that people throw different cards of the same suit with the same chance when matching suit
	for (int i = 0; i < nextPlayers; i++) {
		int playerID = (koenHidde->id + 1 + i) % 4;
		winChance *= (1.0 - (PHigherCard(playerID, card) * PSuitMatch(playerID, card.getCardSuit())));
	}
	return winChance;
}
