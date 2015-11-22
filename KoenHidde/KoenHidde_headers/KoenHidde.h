/*
 * koenHidde.h
 *
 *  Created on: Feb 17, 2013
 *      Author: koen
 */

#ifndef KOENHIDDE_H_
#define KOENHIDDE_H_

#include "standard_includes_defines.h"

using namespace std;

class CardData;

class KoenHidde : public IPlayer {
public:
	KoenHidde(int, int);
	virtual ~KoenHidde();

	CardData *cardData;
	int roundNumber;

	/** Returns the indices to the cards in the hand that should be passed */
	vector<int> passCards();

	/** Virtual function that defines the behaviour of evaluation after the cards have passed */
	void evaluateHand();

	/** Returns the index of the card that should be played based on the cards played so far */
	int decide(const Trick&);

	/** Virtual function that defines the behaviour of evaluation at the end of a round */
	void endOfTrick(const Trick&);

};
#endif /* KOENHIDDE_H_ */
