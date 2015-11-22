/*
 * Decide.h
 *
 *  Created on: Feb 20, 2013
 *      Author: koen
 */

#ifndef DECIDE_H_
#define DECIDE_H_

#include "standard_includes_defines.h"

#include "KoenHidde.h"

class Decide {
public:
	Decide(KoenHidde* koenHidde);
	~Decide() {
	}
	;

	KoenHidde* koenHidde;
	CardData* cardData;

	/** Returns the card we play in the given trick without taking any points */
	Card decideWithoutPoints(const Trick&);

	/** Returns the cards we play, when we want to take points to avoid pit */
	Card decideWithPoints(const Trick&);
};

#endif /* DECIDE_H_ */
