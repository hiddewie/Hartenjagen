/*
 * CardData.h
 *
 *  Created on: Feb 18, 2013
 *      Author: koen
 */

#ifndef CARDDATA_H_
#define CARDDATA_H_

#include "standard_includes_defines.h"

#include "KoenHidde.h"

class CardData {
private:
	/** Initializes the queen variable */
	void findQueen();
	/** Initializes the sortedHand list */
	void sortHand();
	/** Initializes the numberOfCardsPerSuit array */
	void findNumberOfCardsPerSuit();
	/** Initializes the suitsSortedOnNumberOfCards array */
	void sortSuitsOnNumberOfCards();

public:
	CardData(KoenHidde*);
	~CardData() {
	}
	;

	KoenHidde* koenHidde;

	vector<Card> sortedHand[4];
	int numberOfCardsPerSuit[4];
	int suitsSortedOnNumberOfCards[3];
	float cardDist[4][52];
	bool playedCards[52];
	bool haveQueen;
	vector<Card> passedCards;
	bool heartsBroken;
	int numberOfPointCardsTaken[4];

	/** Finds the queen, sorts the hand, initializes and fills data structures */
	void evaluateHand();

	/** Returns the indices (in the hand) belonging to the given cards. Throws if cards are not found */
	vector<int> getIndices(vector<Card>);
	/** Returns the index of the given cards in the hand */
	int getIndex(Card card);

	/** Update the card distribution for each player for each card, based on the cards in the given trick */
	void updateCardDist(const Trick&);

	/** Returns whether or not we are trying to avoid another players pit */
	bool avoidPit();

	/** Sorts all the cards in the hand based on PWin from high to low */
	vector<Card> sortHandOnPWin(const Trick&);
	/** Returns all cards from the given suit with a PWin smaller than bound from high to low */
	vector<Card> sortSuitOnConditionalPWin(const Trick&, int, float);

	/** Returns the chance the given player can match the given suit */
	float PSuitMatch(int id, int suit);
	/** Returns the chance the given player can play a higher card than the given card */
	float PHigherCard(int id, Card card);
	/** Returns the chance the given player has the given card */
	float PHave(int id, Card card);
	/** Returns the expected number of cards of the given suit in the hand of the given player */
	float ENumberOfCardsOfSuit(int id, int suit);
	/** Returns the chance that that person will play the given card */
	float PPlay(int id, int playedSuit, Card card);
	float PPlay(int playedSuit, Card card);
	float PPlay(const Trick& trick, Card card);

	float EPoints (int playedSuit);
	float EPoints (const Trick& trick);

	float EPoints (int id, int playedSuit);
	/** Returns the chance we can win the current trick with the given card */
	float PWin(const Trick&, Card);
};

#endif /* CARDDATA_H_ */
