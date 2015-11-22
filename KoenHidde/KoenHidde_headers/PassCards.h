/*
 * passCards.h
 *
 *  Created on: Feb 18, 2013
 *      Author: koen
 */

#ifndef PASSCARDS_H_
#define PASSCARDS_H_

#include "standard_includes_defines.h"

class KoenHidde;
class CardData;

class PassCards {
public:
	PassCards(KoenHidde* koenHidde);
	~PassCards() {
	}

	CardData* cardData;
	KoenHidde* koenHidde;

	/** Returns the 3 cards we pass before the game starts */
	vector<Card> passCards();
};

#endif /* PASSCARDS_H_ */
