#include "KoenHidde.h"

using namespace std;

#include "CardData.h"
#include "PassCards.h"
#include "Decide.h"

/** Returns the indices to the cards in the hand that should be passed */

KoenHidde::KoenHidde(int id, int round) :
		IPlayer(id, round) {
	this->cardData = new CardData(this);
	this->roundNumber = round;
}
;
KoenHidde::~KoenHidde() {
	delete this->cardData;
}

vector<int> KoenHidde::passCards() {
	this->evaluateHand();

	PassCards *temp = new PassCards(this);
	cardData->passedCards = temp->passCards();
	vector<int> passing = cardData->getIndices(cardData->passedCards);
	delete temp;

	return passing;
}
;

/** Virtual function that defines the behaviour of evaluation after the cards have passed */
void KoenHidde::evaluateHand() {
	this->cardData->evaluateHand();
}
;

/** Returns the index of the card that should be played based on the cards played so far */
int KoenHidde::decide(const Trick& trick) {
	if (!cardData->heartsBroken) {
		for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
			if (trick.playedCards.at(i).card.getCardSuit() == HEARTS) {
				cardData->heartsBroken = true;
			}
		}
	}

	cardData->evaluateHand();
	cardData->updateCardDist(trick);

	Decide *temp = new Decide(this);
	bool avoidPit = cardData->avoidPit();
	Card decidedCard;
	if (avoidPit) {
		decidedCard = temp->decideWithPoints(trick);
	}
	else {
		decidedCard = temp->decideWithoutPoints(trick);
	}
	delete temp;

	return cardData->getIndex(decidedCard);
}
;

/** Virtual function that defines the behaviour of evaluation at the end of a round */
void KoenHidde::endOfTrick(const Trick& trick) {
	cardData->updateCardDist(trick);
	cardData->evaluateHand();

	if (!cardData->heartsBroken) {
		for (int i = 0; i < 4; i++) {
			if (trick.playedCards.at(i).card.getCardSuit() == HEARTS) {
				cardData->heartsBroken = true;
			}
		}
	}

	int winnerID = trick.getWinner()->id;
	for (int i = 0; i < 4; i++) {
		if (trick.playedCards.at(i).card.getPoints() > 0) {
			cardData->numberOfPointCardsTaken[winnerID]++;
		}
	}
}
;
