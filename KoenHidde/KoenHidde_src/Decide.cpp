#include "standard_includes_defines.h"

#include "Decide.h"

#include "CardData.h"

Decide::Decide(KoenHidde* koenHidde) {
	this->koenHidde = koenHidde;
	this->cardData = koenHidde->cardData;
}
;

/** Returns the index of the card we play in the given trick */
Card Decide::decideWithoutPoints(const Trick& trick) {
	int numberOfPlayedCards = trick.playedCards.size();
	vector<Card> sortedHandOnPWin = cardData->sortHandOnPWin(trick);

	if (trick.trickNumber == 0) { // First trick
		if (trick.playedCards.size() == 0) { // First player
			return Card(CLUBS, TWO);
		}
		else { // First trick, not first player
			if (this->cardData->numberOfCardsPerSuit[CLUBS] > 0) { // We have clubs, play high card clubs
				return this->cardData->sortedHand[CLUBS].at(0);
			}
			else { // First trick, not first player, no clubs
				if (this->cardData->haveQueen) { // Fist trick, not first player, no clubs, queen in hand
					if (cardData->numberOfCardsPerSuit[DIAMONDS] > 0) {
						return this->cardData->sortedHand[DIAMONDS].at(0);
					}
					for (int i = cardData->numberOfCardsPerSuit[SPADES] - 1; i >= 0; i--) {
						if (!cardData->sortedHand[SPADES].at(i) == Card(SPADES, QUEEN)) {
							return cardData->sortedHand[SPADES].at(i);
						}
					}
					return Card(SPADES, QUEEN); // If we only have hearts and a queen, give queen
				}
				else { // Fist trick, not first player, no clubs, no queen
					if (koenHidde->isCardInHand(Card(SPADES, ACE))) {
						return Card(SPADES, ACE);
					}
					else if (koenHidde->isCardInHand(Card(SPADES, KING))) {
						return Card(SPADES, KING);
					}
					else { // Throw away highest
						Card highestCard;

						if (cardData->numberOfCardsPerSuit[DIAMONDS] > 0 && cardData->numberOfCardsPerSuit[SPADES] > 0) {
							if (cardData->sortedHand[DIAMONDS].at(0).getCardRank() >= cardData->sortedHand[SPADES].at(0).getCardRank()) {
								highestCard = cardData->sortedHand[DIAMONDS].at(0);
							}
							else {
								highestCard = cardData->sortedHand[SPADES].at(0);
							}
						}
						else if (cardData->numberOfCardsPerSuit[DIAMONDS] > 0) {
							highestCard = cardData->sortedHand[DIAMONDS].at(0);
						}
						else if (cardData->numberOfCardsPerSuit[SPADES] > 0) {
							highestCard = cardData->sortedHand[SPADES].at(0);
						}
						else {
							highestCard = cardData->sortedHand[HEARTS].at(0);
						}

						return highestCard;
					}
				}
			}
		}
	}
	else { // Not the first trick
		if (numberOfPlayedCards == 0) { // Not first trick, first player
			if (cardData->haveQueen) { // Not first trick, first player, have queen
				for (int i = 0; i < 3; i++) {
					if (cardData->numberOfCardsPerSuit[cardData->suitsSortedOnNumberOfCards[i]] > 0
							&& (cardData->heartsBroken || (cardData->suitsSortedOnNumberOfCards[i] != HEARTS))) { //only play hearts if hearts have been broken
						return cardData->sortedHand[cardData->suitsSortedOnNumberOfCards[i]].at(0); //return largest of the pile we want to clear
					}
				}
				// TODO: unsigned int? Never ending loop?
				for (int i = cardData->sortedHand[SPADES].size() - 1; i >= 0; i--) { // Try to play all spades, queen last
					if (!cardData->sortedHand[SPADES].at(i) == Card(SPADES, QUEEN)) {
						return cardData->sortedHand[SPADES].at(i);
					}
				}
				return Card(SPADES, QUEEN); // Only hearts and queen left, so play queen
			}
			else { // Not first trick, first player, don't have queen
				Card currentCard;
				// TODO: efficiency
				for (unsigned int i = 0; i < sortedHandOnPWin.size(); i++) { // play the first card which has a smaller than P_SUB PWin
					currentCard = sortedHandOnPWin.at(i);
					if ((currentCard.getCardSuit() != HEARTS || cardData->heartsBroken) && cardData->PWin(trick, currentCard) < P_SUB) {
						return currentCard;
					}
				}
				for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { // play the first card which has a smaller than 1P_SUB PWin
					currentCard = sortedHandOnPWin.at(i);
					if (currentCard.getCardSuit() != HEARTS || cardData->heartsBroken) {
						return currentCard;
					}
				}
				// We have no cards which have 0 chance to take the trick, so we play card with the lowest PWin
				return sortedHandOnPWin.at(sortedHandOnPWin.size() - 1);
			}
		}
		else if (numberOfPlayedCards != 3) { // Not the first trick, not the first player && not last player
			int firstPlayedSuit = trick.playedCards.at(0).card.getCardSuit();

			if (cardData->numberOfCardsPerSuit[firstPlayedSuit] > 0) { // We must check suit
				if (cardData->haveQueen) { // Not first trick, not first player, not last player, must check suit, have queen
					if (firstPlayedSuit == SPADES && cardData->PWin(trick, Card(SPADES, QUEEN)) <= P_SUB) {
						return Card(SPADES, QUEEN);
					}

					Card currentCard;
					for (unsigned int i = 0; i < sortedHandOnPWin.size(); i++) { // Try to get the trick, but if possible without playing the queen.
						currentCard = sortedHandOnPWin.at(i);
						if (currentCard.getCardSuit() == firstPlayedSuit && !(currentCard == Card(SPADES, QUEEN))) {
							return currentCard;
						}
					}
					return Card(SPADES, QUEEN);
				}
				else { // Not first trick, not first player, not last player, must check suit, no queen
					int points = trick.getPoints();
					if (points > 0) { // There are points in this trick, we don't want this trick.
						// TODO: refine strategy (expected points)
						vector<Card> sortedHandOnConditionalPWin = cardData->sortSuitOnConditionalPWin(trick, firstPlayedSuit, P_SUB);
						if (sortedHandOnConditionalPWin.size() > 0) {
							return sortedHandOnConditionalPWin.at(0);
						}
						/*for (unsigned int i=0; i<sortedHandOnPWin.size(); i++) { // play the first card which matches and has a smaller than P_SUB PWin
						 currentCard = sortedHandOnPWin.at(i);
						 int PWin = cardData -> PWin(trick, currentCard);
						 if (currentCard.getCardSuit() == firstPlayedSuit && PWin < P_SUB) {
						 return currentCard;
						 }
						 }*/

						Card currentCard;
						for (unsigned int i = sortedHandOnPWin.size() - 1;; i--) {
							currentCard = sortedHandOnPWin.at(i);
							if (currentCard.getCardSuit() == firstPlayedSuit) { //play smallest allowed card
								return currentCard;
							}
						}
					}
					else { // No points in this trick until now
						   // TODO: refine stategy (expected points)
						vector<Card> sortedHand = cardData->sortSuitOnConditionalPWin(trick, firstPlayedSuit, P_SUB);
						if (sortedHand.size() > 0) {
							return sortedHand.at(0);
						}

						/*for (unsigned int i=0; i<sortedHandOnPWin.size(); i++) { // play the first card which matches and has a smaller than P_SUB PWin
						 currentCard = sortedHandOnPWin.at(i);
						 if (currentCard.getCardSuit() == firstPlayedSuit && cardData -> PWin(trick, currentCard) < P_SUB) {
						 cout << sortedHandOnPWin.at(i).getName() << " returning" << endl;
						 return cardData -> getIndex(currentCard);
						 }
						 }*/
						Card currentCard;
						for (unsigned int i = sortedHandOnPWin.size() - 1;; i--) {
							currentCard = sortedHandOnPWin.at(i);
							if (currentCard.getCardSuit() == firstPlayedSuit) { //play smallest allowed card
								return currentCard;
							}
						}
					}
				}
			}
			else { // We don't have the suit, we can play any card(2nd or 3rd player)
				if (cardData->haveQueen) {
					return Card(SPADES, QUEEN);
				}
				else { // We have no queen, possibly play hearts, first high spades
					if (!cardData->playedCards[(int) Card(SPADES, QUEEN)]) { // Queen still in game, try to lost high spades
						if (koenHidde->isCardInHand(Card(SPADES, ACE))) {
							return Card(SPADES, ACE);
						}
						else if (koenHidde->isCardInHand(Card(SPADES, KING))) {
							return Card(SPADES, KING);
						}
					}
					if (cardData->numberOfCardsPerSuit[HEARTS] > 0) { // If we have hearts, play them
						return cardData->sortedHand[HEARTS].at(0);
					}
					else { // Pwin highest
						return sortedHandOnPWin.at(0);
					}
				}

			}
		}
		else { // Not first trick, last player
			int firstPlayedSuit = trick.playedCards.at(0).card.getCardSuit();

			if (cardData->numberOfCardsPerSuit[firstPlayedSuit] > 0) { // We must check suit
				if (trick.getPoints() > 0) { // There are points in the suit, try to dive
					/*Card currentCard;
					 for (unsigned int i=0; i<sortedHandOnPWin.size(); i++) { // Play the first card which matches and has a smaller than P_SUB PWin
					 currentCard = sortedHandOnPWin.at(i);
					 if (currentCard.getCardSuit() == firstPlayedSuit && cardData->PWin(trick, currentCard) < P_SUB) {
					 return currentCard;
					 }
					 }*/
					vector<Card> sortedHandOnConditionalPwin = cardData->sortSuitOnConditionalPWin(trick, firstPlayedSuit, P_SUB);
					if (sortedHandOnConditionalPwin.size() > 0) {
						return sortedHandOnConditionalPwin.at(0);
					}

					Card currentCard;
					for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) {
						currentCard = sortedHandOnPWin.at(i);
						if (currentCard.getCardSuit() == firstPlayedSuit) {
							return currentCard;
						}
					}
				}
				else { // No points, win trick with high card of required suit
					return cardData->sortedHand[firstPlayedSuit].at(0);
				}
			}
			else { // Not first trick, last player, no need to check suit
				   // Hearts or queen, else Pwin
				if (cardData->haveQueen) {
					return Card(SPADES, QUEEN);
				}
				else { // we have no queen, possibly play hearts, first high spades
					if (!cardData->playedCards[(int) Card(SPADES, QUEEN)]) { // Queen still in game, try to lost high spades
						if (koenHidde->isCardInHand(Card(SPADES, ACE))) {
							return Card(SPADES, ACE);
						}
						else if (koenHidde->isCardInHand(Card(SPADES, KING))) {
							return Card(SPADES, KING);
						}
					}
					if (cardData->numberOfCardsPerSuit[HEARTS] > 0) { // If we have hearts, play them
						return cardData->sortedHand[HEARTS].at(0);
					}
					else { // Pwin highest
						return cardData->sortHandOnPWin(trick).at(0);
					}
				}
			}
		}
	}
	if (ERROR) {
		cerr << "   ---   ERROR (decideWithoutPoints)   ---" << endl;
	}
	throw;
	return koenHidde->hand.at(0);
}

Card Decide::decideWithPoints(const Trick& trick) {

	if (EXPLAIN) {
		cout << "##### HAND:" << endl;

		Card::printCards(koenHidde->hand);
	}
	//*** 													DATA INIT 													***//

	if (EXPLAIN)
		cout << "Deciding with points." << endl;
	int numberOfPlayedCards = trick.playedCards.size();
	vector<Card> sortedHandOnPWin = cardData->sortHandOnPWin(trick);
	Card potentialCardToPlay;

	/*
	 cout << "Hand: " << endl;
	 Card::printCards(koenHidde -> hand);
	 */
	vector<int> availableSuits;
	// add all the available suits for which playing it does not yield the queen.
	if (numberOfPlayedCards == 0) {
		for (int i = 0; i < 4; i++) {
			if (koenHidde->numberOfSuitInHand(i) > 0
					&& (i != HEARTS || cardData->heartsBroken || koenHidde->numberOfSuitInHand(HEARTS) == (int) koenHidde->hand.size())) {
				if (cardData->PPlay(i, Card(SPADES, QUEEN)) <= P_SUB) {
					availableSuits.push_back(i);
				}
			}
		}
		if (availableSuits.size() == 0) {
			if (EXPLAIN)
				cout << "No suit safe from potential queen play, so return without points." << endl;
			return this->decideWithoutPoints(trick);
		}
	}
	else {
		if (koenHidde->numberOfSuitInHand(trick.playedCards.at(0).card.getCardSuit()) > 0 && cardData->PPlay(trick, Card(SPADES, QUEEN)) >= P_SUB) {
			if (EXPLAIN)
				cout << "No suit safe from potential queen play, so return without points." << endl;
			return this->decideWithoutPoints(trick);
		}
		else {
			for (int i = 0; i < 4; i++) {
				if (koenHidde->numberOfSuitInHand(i) > 0) {
					availableSuits.push_back(i);
				}
			}
		}
	}

// if the number of available suits is 0, return without points

	Card maxPWinCards[4]; //fill maxPWinCards[4]
	Card currentCard;
	for (int i = (int) sortedHandOnPWin.size() - 1; i >= 0; i--) {
		currentCard = sortedHandOnPWin.at(i);
		maxPWinCards[currentCard.getCardSuit()] = currentCard;
	}
	/*
	 cout << "maxPWinCards:" << endl;
	 for(int i=0;i<4;i++){
	 cout << maxPWinCards[i].getName() << "; PWin: " << cardData->PWin(trick, maxPWinCards[i]) << "; EPoints: " << cardData->EPoints(i) << endl;
	 }
	 */

//fill maxPWinEPoints and cardToPlay for which maxPWinEPoints
	float maxPWinEPoints = 0;
	Card cardToPlay;
	for (unsigned int i = 0; i < availableSuits.size(); i++) {
		float PWinEPoints =
				cardData->PWin(trick, maxPWinCards[availableSuits.at(i)]) * cardData->EPoints(availableSuits.at(i))
						* !(availableSuits.at(i) == HEARTS && !cardData->heartsBroken
								&& koenHidde->numberOfSuitInHand(HEARTS) != (int) koenHidde->hand.size());
		//cout << "PWinEPoints(" << Card::suits[availableSuits.at(i)] << "): " <<  PWinEPoints << endl;
		if (PWinEPoints > maxPWinEPoints && PWinEPoints >= PWIN_EPOINTS_MIN) {
			maxPWinEPoints = PWinEPoints;
			cardToPlay = maxPWinCards[availableSuits.at(i)];
		}
		//cout << "maxPwinEPoints: " << maxPWinEPoints << endl;
	}

//*** 													END DATA INIT 												***//

// if a queen has been played, do NOT take the trick
	for (unsigned int i = 0; i < trick.playedCards.size(); i++) {
		if (trick.playedCards.at(i).card == Card(SPADES, QUEEN)) {
			if (EXPLAIN)
				cout << "A queen has been played, so we DON'T want this trick." << endl;
			return this->decideWithoutPoints(trick);
		}
	}

	if (EXPLAIN)
		cout << "A Queen has not yet been played in this trick." << endl;

	if (numberOfPlayedCards == 0) { // First player
		// loop over alle beschikbare suits:
		// als we de koningin kunnen pakken:
		// return withoutPoints
		// else:
		// save this suit
		// loop over de opgeslagen suits:
		// zoek de suit die maximale waarde geeft van: max{Pwin(suit)} * ExpectedPoints(suit)
		// als de waarde van max{Pwin(suit)} *  ExpectedPoints(suit) >= PWIN_EPOINTS_MIN
		// speel de kaart met max PWin binnen die suit.
		// else :
		// play withoutPoints

		if (EXPLAIN)
			cout << "We are the first player." << endl;

		if (maxPWinEPoints > 0) { //we have a good enough maxPWinEPoints to play with a certain card, so we play that card
			/*
			 cout << "Hand ############################################# " << endl;
			 Card::printCards(koenHidde->hand);
			 cout << cardToPlay.getName(ber of expected points times our chance to win them is large enough, so ) << endl;
			 */
			if (EXPLAIN)
				cout << "maxPWinEpoints > 0, The number of expected points times our chance to win them is large enough, so we try to take the trick."
						<< endl;
			//TODO: add the one point if we play hearts ourselves? is that needed?
			return cardToPlay;
		}
		else { //we don't have a good enough maxPWinEPoints to play, so we throw away a low PWin card
			   //cout << "returning! ############################################# " << endl;
			if (EXPLAIN)
				cout
						<< "maxPWinEpoints == 0, The number of expected points times our chance to win them is not large enough, so we don't try to take the trick."
						<< endl;

			if (!cardData->heartsBroken && koenHidde->numberOfSuitInHand(HEARTS) != (int) koenHidde->hand.size()) {
				int i = cardData->sortHandOnPWin(trick).size() - 1;
				while (i > 0 && sortedHandOnPWin.at(i).getCardSuit() == HEARTS) {
					i--;
				}
				return sortedHandOnPWin.at(i);
			}
			return sortedHandOnPWin.at(cardData->sortHandOnPWin(trick).size() - 1); //lowest PWin card in hand
		}
	}
	else { // Second, third or last player

		if (EXPLAIN)
			cout << "We are not the first player." << endl;

		//if we can't admin color, just play a expected low PWin card
		if (cardData->numberOfCardsPerSuit[trick.playedCards.at(0).card.getCardSuit()] == 0) {
			for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { //try to play future low PWin card, excluding hearts if appropriate.
				potentialCardToPlay = sortedHandOnPWin.at(i);
				if (potentialCardToPlay.getCardSuit() != HEARTS || cardData->heartsBroken
						|| (int) sortedHandOnPWin.size() == cardData->numberOfCardsPerSuit[HEARTS]) {

					if (EXPLAIN)
						cout << "We can't admin color anyway, so play a low card." << endl;

					return potentialCardToPlay;
				}
			}
		}
		if (cardData->PPlay(trick, Card(SPADES, QUEEN)) < P_SUB) { // no queen will be played

			if (EXPLAIN)
				cout << "We think no Queen will be played." << endl;

			if (trick.getPoints() > 0) { //are there already points? if yes:
				// try to take the trick

				if (EXPLAIN)
					cout << "There are points in this trick! We will try to take it." << endl;

				//if all pwins are 0, don't throw away a high card!

				if (cardData->PWin(trick, maxPWinCards[trick.playedCards.at(0).card.getCardSuit()]) < P_SUB) {

					if (EXPLAIN)
						cout << "Alas, we can't possibly take the trick anyway, so we won't waste a high card.1" << endl;

					for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { //try to play future low PWin card, excluding hearts if appropriate.
						potentialCardToPlay = sortedHandOnPWin.at(i);
						if (potentialCardToPlay.getCardSuit() != HEARTS || cardData->heartsBroken
								|| (int) sortedHandOnPWin.size() == cardData->numberOfCardsPerSuit[HEARTS]) {
							return potentialCardToPlay;
						}
					}
				}
				else { //high enough PWIN, yay!
					return maxPWinCards[trick.playedCards.at(0).card.getCardSuit()];
				}
			}
			else { //no points yet, will there be?
				float EPoints = cardData->EPoints(trick);
				if (EPoints >= EPOINTS_MIN && EPoints <= EPOINTS_MAX) { // the expected points are reasonable, so we take the trickif(EXPLAIN) cout << "" << endl;
					// Take the trick

					if (EXPLAIN)
						cout << "The expected points in this trick are reasonable, so we try to take it." << endl;

					//if all pwins <= 0, don't throw away a high card

					//cout << "PWin of " << maxPWinCards[trick.playedCards.at(0).card.getCardSuit()].getName() << " is: " << cardData->PWin(trick, maxPWinCards[trick.playedCards.at(0).card]) << endl;

					if (cardData->PWin(trick, maxPWinCards[trick.playedCards.at(0).card.getCardSuit()]) < P_SUB) {

						if (EXPLAIN)
							cout << "Alas, we can't possibly take the trick anyway, so we won't waste a high card.2" << endl;

						for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { //try to play future low PWin card, excluding hearts if appropriate.
							potentialCardToPlay = sortedHandOnPWin.at(i);
							if (potentialCardToPlay.getCardSuit() != HEARTS || cardData->heartsBroken
									|| (int) sortedHandOnPWin.size() == cardData->numberOfCardsPerSuit[HEARTS]) {
								return potentialCardToPlay;
							}
						}
					}
					else { //high enough PWIN, yay!
						return maxPWinCards[trick.playedCards.at(0).card.getCardSuit()];
					}
				}
				else { // too many or too little points, don't take trick, don't waste high PWin cards.
					Card potentialCardToPlay;
					for (int i = sortedHandOnPWin.size() - 1; i >= 0; i--) { //try to play lowest PWin card
						potentialCardToPlay = sortedHandOnPWin.at(i);
						if ((potentialCardToPlay.getCardSuit() == trick.playedCards.at(0).card.getCardSuit())) {

							if (EXPLAIN)
								cout << "The expected points in this trick are not reasonable, so we try don't to take it." << endl;

							return potentialCardToPlay;
						}
					}
				}
			}
		}
		else { // a queen will be played

			if (EXPLAIN)
				cout << "A queen might be played, so we don't want the trick." << endl;

			return this->decideWithoutPoints(trick);
		}
	}

	return this->decideWithoutPoints(trick);
}

