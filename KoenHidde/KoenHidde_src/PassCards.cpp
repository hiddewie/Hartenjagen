#include "standard_includes_defines.h"

using namespace std;

#include "CardData.h"
#include "PassCards.h"

PassCards::PassCards(KoenHidde* koenHidde) {
	this->koenHidde = koenHidde;
	this->cardData = koenHidde->cardData;
}
;

/** Returns the 3 cards we pass before the game starts */
vector<Card> PassCards::passCards() {
	vector<Card> giveCards;

	if (koenHidde->isCardInHand(Card(SPADES, QUEEN))) { // We have the queen
		if (koenHidde->numberOfSuitInHand(SPADES) >= 5) { // Keep queen because of sufficient number of spades
			for (int s = 0; s < 3; s++) { // Loops over the suits in order
				int currentSuit = cardData->suitsSortedOnNumberOfCards[s];

				for (int n = 0; n < cardData->numberOfCardsPerSuit[currentSuit]; n++) { //loop over the cards in the suit from high to low
					Card card = cardData->sortedHand[currentSuit].at(n);

					if (currentSuit == CLUBS && n == cardData->numberOfCardsPerSuit[currentSuit] - 1) { //last clubs should not be given away
					}
					else {
						giveCards.push_back(card);
						if (giveCards.size() >= 3) {
							return giveCards;
						}
					}

				} // Not enough cards found in hearts, diamonds, clubs, so give away high spades(not queen)
				for (unsigned int i = 0; i < cardData->sortedHand[SPADES].size(); i++) {
					Card card = cardData->sortedHand[SPADES].at(i);
					if (!(cardData->sortedHand[SPADES].at(i) == (Card(SPADES, QUEEN)))) { //it's not the spades queen, and it's high, so give
						giveCards.push_back(card);
						if (giveCards.size() >= 3) {
							return giveCards;
						}
					}
				}
			}
		}
		else { // Give queen because of insufficient number of spades
			Card pCard = Card(SPADES, QUEEN);

			giveCards.push_back(pCard); // Give queen away

			pCard = Card(SPADES, ACE);
			if (koenHidde->isCardInHand(pCard)) { // If ace of spades, give it away
				giveCards.push_back(pCard);
			}

			pCard = Card(SPADES, KING);
			if (koenHidde->isCardInHand(pCard)) { // If king of spades, give it away
				giveCards.push_back(pCard);
				if (giveCards.size() >= 3) {
					return giveCards;
				}
			}

			for (int rank = ACE; rank > 0; rank--) { // Loop over all highest cards to give away (with suit preference: hearts then diamonds then clubs)
				for (int suit = HEARTS; suit <= CLUBS; suit++) {
					Card card = Card(suit, rank);
					if (koenHidde->isCardInHand(card)) {
						if (suit == CLUBS && cardData->numberOfCardsPerSuit[CLUBS] <= 1) { // We don't want to give away the last clubs, so check for it
						}
						else {
							giveCards.push_back(card);
							if (giveCards.size() >= 3) {
								return giveCards;
							}
						}
					}
				}
			}
		}
	}
	else { // We don't have a queen
		Card pCard = Card(SPADES, ACE);
		if (koenHidde->isCardInHand(pCard)) {
			giveCards.push_back(pCard); // If ace of spades, give it away
		}
		pCard = Card(SPADES, KING);
		if (koenHidde->isCardInHand(pCard)) {
			giveCards.push_back(pCard); // If king of spades, give it away
		}
		for (int rank = ACE; rank >= 0; rank--) { // Loop over all highest cards to give away (with suit preference: hearts then diamonds then clubs)
			for (int suit = HEARTS; suit <= CLUBS; suit++) {
				Card card = Card(suit, rank);
				if (koenHidde->isCardInHand(card)) {
					if (suit == CLUBS && cardData->numberOfCardsPerSuit[CLUBS] <= 1) { //We don't want to give away the last clubs, so check for it
					}
					else {
						giveCards.push_back(Card(suit, rank));
						if (giveCards.size() >= 3) {
							return giveCards;
						}
					}
				}
			}
		}
	}
	if (VERBOSE)
		cout << "   ---   ERROR (passCards)   ---   " << endl;
	throw;

	vector<Card> error;
	error.push_back(Card());
	error.push_back(Card());
	error.push_back(Card());

	return error;
}
;
